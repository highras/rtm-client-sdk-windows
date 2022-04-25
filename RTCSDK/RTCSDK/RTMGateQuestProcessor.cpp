#include "RTMGateQuestProcessor.h"
#include "FPJson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "./RTMUnitis/RTMConfig.h"
#include "./RTMUnitis/RTMClientManager.h"

using namespace std;

extern RTMConfig rtmConfig;
extern RTMRegressiveStrategy rtmRegressiveStrategy;
RTMGateQuestProcessor::RTMGateQuestProcessor(shared_ptr<RTMEventHandler> rtm):
	rtmEventHandler(rtm)
{
	registerMethod("pushEnterRTCRoom", &RTMGateQuestProcessor::pushEnterRTCRoom);
	registerMethod("pushExitRTCRoom", &RTMGateQuestProcessor::pushExitRTCRoom);
	registerMethod("pushRTCRoomClosed", &RTMGateQuestProcessor::pushRTCRoomClosed);
	registerMethod("pushInviteIntoRTCRoom", &RTMGateQuestProcessor::pushInviteIntoRTCRoom);
	registerMethod("pushKickOutRTCRoom", &RTMGateQuestProcessor::pushKickOutRTCRoom);
	registerMethod("pushPullIntoRTCRoom", &RTMGateQuestProcessor::pushPullIntoRTCRoom);
	registerMethod("pushAdminCommand", &RTMGateQuestProcessor::pushAdminCommand);
	registerMethod("pushRoomEvent", &RTMGateQuestProcessor::pushRoomEvent);
	registerMethod("ping", &RTMGateQuestProcessor::ping);

	registerMethod("kickout", &RTMGateQuestProcessor::kickout);
	registerMethod("kickoutroom", &RTMGateQuestProcessor::kickoutRoom);
	registerMethod("pushmsg", &RTMGateQuestProcessor::pushMsg);
	registerMethod("pushgroupmsg", &RTMGateQuestProcessor::pushGroupMsg);
	registerMethod("pushroommsg", &RTMGateQuestProcessor::pushRoomMsg);
	registerMethod("pushbroadcastmsg", &RTMGateQuestProcessor::pushBroadcastMsg);

	registerMethod("pushP2PRTCRequest", &RTMGateQuestProcessor::pushP2PRTCRequest);
	registerMethod("pushP2PRTCEvent", &RTMGateQuestProcessor::pushP2PRTCEvent);
}


FPAnswerPtr RTMGateQuestProcessor::pushEnterRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t uid = args->wantInt("uid");
	int64_t rid = args->wantInt("rid");
	int64_t mtime = args->wantInt("mtime");
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnUserEnterRTCRoom(uid, rid, mtime);

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushExitRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t uid = args->wantInt("uid");
	int64_t rid = args->wantInt("rid");
	int64_t mtime = args->wantInt("mtime");
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnUserExitRTCRoom(uid, rid, mtime);

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushRTCRoomClosed(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t rid = args->wantInt("rid");
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnRTCRoomClosed(rid);

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushInviteIntoRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t uid = args->wantInt("fromUid");
	int64_t rid = args->wantInt("rid");
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnInviteIntoRTCRoom(uid, rid);

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushKickOutRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t uid = args->wantInt("fromUid");
	int64_t rid = args->wantInt("rid");
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnKickOutFromRTCRoom(uid, rid);

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushPullIntoRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t rid = args->wantInt("rid");
	string token = args->wantString("token");
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnPullIntoRTCRoom(rid, token);

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushAdminCommand(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int32_t command = args->wantInt("command");
	vector<int64_t> uids = args->get("uids", vector<int64_t>());
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnAdminCommand((RTCEventHandler::AdminCommand)command, uids);

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushRoomEvent(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t rid = args->wantInt("rid");
	vector<unsigned char> event_ = args->get("event", vector<unsigned char>());
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnRoomEvent(rid, (RTCEventHandler::RoomEvent)0, event_);

	return nullptr;
}


/*********************************rtm event****************************************/
void RTMGateQuestProcessor::updatePingTime()
{
	lastPingTime.store(slack_real_sec(), std::memory_order_relaxed);
}

bool RTMGateQuestProcessor::isConnectionAlive()
{
	bool alive = true;

	long lastPingSec = lastPingTime.load(std::memory_order_relaxed);
	if (slack_real_sec() - lastPingSec > rtmConfig.lostConnectionAfterLastPingInSeconds) {
		alive = false;
	}

	return alive;
}

void RTMGateQuestProcessor::connnectionClosed(int32_t closeError)
{
	if (rtmEventHandler)
		rtmEventHandler->connnectionClosed(closeError);
}

void RTMGateQuestProcessor::reloginCompleted(bool successful, bool retryAgain, int errorCode, int retriedCount)
{
	if (rtmEventHandler)
		rtmEventHandler->reloginCompleted(successful, retryAgain, errorCode, retriedCount);
}

bool RTMGateQuestProcessor::reloginWillStart(int lastErrorCode, int retriedCount)
{
	if (rtmEventHandler)
		return rtmEventHandler->reloginWillStart(lastErrorCode, retriedCount);

	return true;
}

void RTMGateQuestProcessor::connected(const ConnectionInfo& connInfo, bool connected)
{
	if (!rtmClient.expired())
	{
		if (rtmType == RTMClient::RTMType::RT_RTM)
			rtmClient.lock()->connected(connInfo, connected);
		else if (rtmType == RTMClient::RTMType::RT_FILE)
			rtmClient.lock()->connectedForFileClient(connInfo, connected);
	}
}

void RTMGateQuestProcessor::connectionWillClose(const ConnectionInfo& connInfo, bool closeError)
{
	if (!rtmClient.expired())
	{
		if (rtmType == RTMClient::RTMType::RT_RTM)
			rtmClient.lock()->connectionWillClose(connInfo, closeError);
		else if (rtmType == RTMClient::RTMType::RT_FILE)
			rtmClient.lock()->connectionWillClose(connInfo, closeError);
	}
}

FPAnswerPtr RTMGateQuestProcessor::ping(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	lastPingTime.store(slack_real_sec(), std::memory_order_relaxed);
	return FPAWriter::emptyAnswer(quest);
}

FPAnswerPtr RTMGateQuestProcessor::kickout(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	if (rtmEventHandler)
		rtmEventHandler->OnKickout();

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::kickoutRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t rid = args->wantInt("rid");
	if (rtmEventHandler)
		rtmEventHandler->OnKickoutRoom(rid);

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushMsg(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t from = args->wantInt("from");
	int64_t to = args->wantInt("to");
	int64_t mid = args->wantInt("mid");

	if (rtmMsgFilter.isContainMsg(from, mid))
		return nullptr;

	RTMMsg rtmMsg = parseRTMsg(args, from, to, mid);

	if (rtmEventHandler)
	{
		if (rtmMsg.msgType == RTMMsgType::RMT_Chat)
			rtmEventHandler->OnPushChat(from, to, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		if (rtmMsg.msgType == RTMMsgType::RMT_Cmd)
			rtmEventHandler->OnPushCmd(from, to, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		else if (rtmMsg.msgType >= 40 && rtmMsg.msgType <= 50)
			rtmEventHandler->OnPushFile(from, to, rtmMsg.msgType, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		else
			rtmEventHandler->OnPushMsg(from, to, rtmMsg.msgType, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
	}

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushGroupMsg(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t from = args->wantInt("from");
	int64_t gid = args->wantInt("gid");
	int64_t mid = args->wantInt("mid");

	if (rtmMsgFilter.isContainMsgGroupMsg(gid, from, mid))
		return nullptr;

	RTMMsg rtmMsg = parseRTMsg(args, from, gid, mid);

	if (rtmEventHandler)
	{
		if (rtmMsg.msgType == RTMMsgType::RMT_Chat)
			rtmEventHandler->OnPushGroupChat(from, gid, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		if (rtmMsg.msgType == RTMMsgType::RMT_Cmd)
			rtmEventHandler->OnPushGroupCmd(from, gid, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		else if (rtmMsg.msgType >= 40 && rtmMsg.msgType <= 50)
			rtmEventHandler->OnPushGroupFile(from, gid, rtmMsg.msgType, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		else
			rtmEventHandler->OnPushGroupMsg(from, gid, rtmMsg.msgType, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
	}

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushRoomMsg(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t from = args->wantInt("from");
	int64_t rid = args->wantInt("rid");
	int64_t mid = args->wantInt("mid");

	if (rtmMsgFilter.isContainMsgRoomMsg(rid, from, mid))
		return nullptr;

	RTMMsg rtmMsg = parseRTMsg(args, from, rid, mid);

	if (rtmEventHandler)
	{
		if (rtmMsg.msgType == RTMMsgType::RMT_Chat)
			rtmEventHandler->OnPushRoomChat(from, rid, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		if (rtmMsg.msgType == RTMMsgType::RMT_Cmd)
			rtmEventHandler->OnPushRoomCmd(from, rid, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		else if (rtmMsg.msgType >= 40 && rtmMsg.msgType <= 50)
			rtmEventHandler->OnPushRoomFile(from, rid, rtmMsg.msgType, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		else
			rtmEventHandler->OnPushRoomMsg(from, rid, rtmMsg.msgType, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
	}

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushBroadcastMsg(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t from = args->wantInt("from");;
	int64_t mid = args->wantInt("mid");

	if (rtmMsgFilter.isContainMsgBroadcastMsg(from, mid))
		return nullptr;

	RTMMsg rtmMsg = parseRTMsg(args, from, 0, mid);

	if (rtmEventHandler)
	{
		if (rtmMsg.msgType == RTMMsgType::RMT_Chat)
			rtmEventHandler->OnPushBroadcastChat(from, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		if (rtmMsg.msgType == RTMMsgType::RMT_Cmd)
			rtmEventHandler->OnPushBroadcastCmd(from, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		else if (rtmMsg.msgType >= 40 && rtmMsg.msgType <= 50)
			rtmEventHandler->OnPushBroadcastFile(from, rtmMsg.msgType, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
		else
			rtmEventHandler->OnPushBroadcastMsg(from, rtmMsg.msgType, mid, rtmMsg.msgText, rtmMsg.attrs, rtmMsg.mtime);
	}

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushP2PRTCRequest(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t callid = args->wantInt("callId");
	int64_t peerUid = args->wantInt("peerUid");
	int32_t type = args->wantInt("type");
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnPushP2PRTCRequest(callid, peerUid, type);

	return nullptr;
}

FPAnswerPtr RTMGateQuestProcessor::pushP2PRTCEvent(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
	int64_t callId = args->wantInt("callId");
	int64_t peerUid = args->wantInt("peerUid");
	int32_t type = args->wantInt("type");
	int32_t event_ = args->wantInt("event");
	sendAnswer(FPAWriter::emptyAnswer(quest));

	if (rtcEventHandler)
		rtcEventHandler->OnPushP2PRTCEvent(callId, peerUid, type, event_);

	return nullptr;
}

RTMGateQuestProcessor::~RTMGateQuestProcessor()
{
}

void RTMGateQuestProcessor::SetRTCEventHandler(shared_ptr<RTCEventHandler> rtchandler)
{
	rtcEventHandler = rtchandler;
}

void RTMGateQuestProcessor::SetClientPtr(RTMClientPtr clientPtr, RTMClient::RTMType rType)
{
	rtmType = rType;
	rtmClient = clientPtr;
}


/*****************************************************/
RTMTranslatedMsg RTMGateQuestProcessor::parseChatMsg(const FPReaderPtr args)
{
	RTMTranslatedMsg tm;

	OBJECT msgObj = args->getObject("msg"); 
	FPReader msgRead(msgObj);
	if (msgRead.existKey("source"))
	{
		tm.sourceLanguage = msgRead.wantString("source");
	}
	if (msgRead.existKey("target"))
	{
		tm.targetLanguage = msgRead.wantString("target");
	}
	if (msgRead.existKey("sourceText"))
	{
		tm.sourceText = msgRead.wantString("sourceText");
	}
	if (msgRead.existKey("targetText"))
	{
		tm.targetText = msgRead.wantString("targetText");
	}

	return tm;
}

void RTMGateQuestProcessor::parseFileMsg(RTMMsg& rtmMsg)
{
	rapidjson::Document document;
	if (document.Parse(rtmMsg.msgText.c_str()).HasParseError())
		return;
	if (document.IsObject() == false)
		return;
	if (!document.IsNull())
	{
		if (document.HasMember("url") && document["url"].IsString())
			rtmMsg.fileInfo.url = document["url"].GetString();

		if (document.HasMember("size") && document["size"].IsInt())
			rtmMsg.fileInfo.size = document["size"].GetInt();

		if (rtmMsg.msgType == RTMMsgType::RMT_ImageFile)
			if (document.HasMember("surl") && document["surl"].IsString())
				rtmMsg.fileInfo.surl = document["surl"].GetString();
	}
}

void RTMGateQuestProcessor::parseFileAttrsMsg(RTMMsg& rtmMsg)
{
	rapidjson::Document document;
	if (document.Parse(rtmMsg.attrs.c_str()).HasParseError())
		return;
	if (document.IsObject() == false)
		return;
	if (!document.IsNull())
	{
		if (document.HasMember("rtm"))
		{
			rapidjson::Value& rtmAttrs = document["rtm"];
			std::string msgType;
			if (rtmAttrs.HasMember("type") && rtmAttrs["type"].IsString())
				msgType = rtmAttrs["type"].GetString();
			if (msgType == "audiomsg")
				rtmMsg.fileInfo.isRTMAudio = true;

			if (rtmMsg.fileInfo.isRTMAudio)
			{
				if (rtmAttrs.HasMember("lang") && rtmAttrs["lang"].IsString())
					rtmMsg.fileInfo.lang = rtmAttrs["lang"].GetString();
				if (rtmAttrs.HasMember("codec") && rtmAttrs["codec"].IsString())
					rtmMsg.fileInfo.codec = rtmAttrs["codec"].GetString();
				if (rtmAttrs.HasMember("duration") && rtmAttrs["duration"].IsInt())
					rtmMsg.fileInfo.duration = rtmAttrs["duration"].GetInt();
				if (rtmAttrs.HasMember("srate") && rtmAttrs["srate"].IsInt())
					rtmMsg.fileInfo.srate = rtmAttrs["srate"].GetInt();
			}
		}

		if (document.HasMember("custom"))
		{
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			document["custom"].Accept(writer);
			rtmMsg.attrs = buffer.GetString();
		}
	}
}

RTMMsg RTMGateQuestProcessor::parseRTMsg(const FPReaderPtr args, long from, long to, long mid)
{
	RTMMsg rtmMsg;
	rtmMsg.msgType = args->wantInt("mtype");
	rtmMsg.attrs = args->wantString("attrs");
	rtmMsg.mtime = args->wantInt("mtime");

	if (rtmMsg.msgType == RTMMsgType::RMT_Chat)
	{
		rtmMsg.translatedInfo = parseChatMsg(args);

		if (!rtmMsg.translatedInfo.targetText.empty())
			rtmMsg.msgText = rtmMsg.translatedInfo.targetText;
		else
			rtmMsg.msgText = rtmMsg.translatedInfo.sourceText;
	}
	else if (rtmMsg.msgType == RTMMsgType::RMT_Cmd)
	{
		rtmMsg.msgText = args->wantString("msg");
	}
	else if (rtmMsg.msgType >= 40 && rtmMsg.msgType <= 50)
	{
		rtmMsg.msgText = args->wantString("msg");
		parseFileMsg(rtmMsg);
		parseFileAttrsMsg(rtmMsg);
	}
	else
	{
		rtmMsg.msgText = args->wantString("msg");
		/*OBJECT msgObj = args->getObject("msg");
		if (msgObj.type == msgpack::type::BIN)
		{
			if (msgObj.via.bin.ptr)
			{
				if (rtmMsg.msgText.capacity() < msgObj.via.bin.size)
					rtmMsg.msgText.reserve(msgObj.via.bin.size);
				rtmMsg.msgText.assign(msgObj.via.bin.ptr, msgObj.via.bin.size);
			}
		}
		else
		{
			if (msgObj.via.str.ptr)
			{
				if (rtmMsg.msgText.capacity() < msgObj.via.str.size)
					rtmMsg.msgText.reserve(msgObj.via.str.size);
				rtmMsg.msgText.assign(msgObj.via.str.ptr, msgObj.via.str.size);
			}
		}*/
	}

	return rtmMsg;
}
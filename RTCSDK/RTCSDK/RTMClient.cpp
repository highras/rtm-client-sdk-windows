#include "RTMClient.h"
#include "RTMGateQuestProcessor.h"
#include "./RTMUnitis/RTMClientManager.h"


RTMClient::RTMClient(string host, unsigned short port, int64_t pid, int64_t uid)
    : rtmClient(TCPClient::createClient(host, port))
{
    rtmProcessor = make_shared<RTMGateQuestProcessor>(nullptr);
    rtmClient->setQuestProcessor(rtmProcessor);
    this->pid = pid;
    this->uid = uid;
}

RTMClient::RTMClient(string host, unsigned short port, int64_t pid, int64_t uid, shared_ptr<RTMEventHandler> rtmhandler, bool isAutoReloginValid)
    : rtmClient(TCPClient::createClient(host, port))
{
    rtmProcessor = make_shared<RTMGateQuestProcessor>(rtmhandler);
	rtmClient->setQuestProcessor(rtmProcessor);

    connEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    rtmAutoReloginInfo.isAutoReloginValid = isAutoReloginValid;

    connStatus = ConnStatus::Closed;

    this->pid = pid;
    this->uid = uid;
}

RTMClient::~RTMClient()
{
}

void RTMClient::SetRTCEventHandler(shared_ptr<RTCEventHandler> rtchandler)
{
    dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->SetRTCEventHandler(rtchandler);
}

string RTMClient::EnterRTCRoom(int64_t rid)
{
    string result;
    FPQWriter qw(1, "enterRTCRoom");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("EnterRTCRoom", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("EnterRTCRoom", answer);
    if (answer && answer->status() == 0)
    {
        FPAReader ar(answer);
        result = ar.getString("token");
    }
	return result;
}

string RTMClient::CreateRTCRoom(int32_t type, int64_t rid, int32_t enableRecord)
{
    string result;
    FPQWriter qw(3, "createRTCRoom");
    qw.param("rid", rid);
    qw.param("type", 2);
    qw.param("enableRecord", 0);

    FPQuestPtr quest = qw.take();
    printQuestJson("CreateRTCRoom", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("CreateRTCRoom", answer);
    if (answer && answer->status() == 0)
    {
        FPAReader ar(answer);
        result = ar.getString("token");
    }
    return result;
}

bool RTMClient::ExitRTCRoom(int64_t rid)
{
    FPQWriter qw(1, "exitRTCRoom");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("ExitRTCRoom", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("ExitRTCRoom", answer);
    if (answer && answer->status() != 0)
    {
        return false;
    }

    return true;
}

void RTMClient::SubscribeVideo(int64_t rid, unordered_set<int64_t> uids)
{
    FPQWriter qw(2, "subscribeVideo");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("SubscribeVideo", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("SubscribeVideo", answer);
    if (answer && answer->status() != 0)
    {
    }
    else
    {
    }
}

void RTMClient::UnsubscribeVideo(int64_t rid, unordered_set<int64_t> uids)
{
    FPQWriter qw(2, "unsubscribeVideo");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("UnsubscribeVideo", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("UnsubscribeVideo", answer);
    if (answer && answer->status() != 0)
    {
    }
    else
    {
    }
}

void RTMClient::InviteUserIntoRTCRoom(int64_t rid, unordered_set<int64_t> uids)
{
    FPQWriter qw(2, "inviteUserIntoRTCRoom");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("InviteUserIntoRTCRoom", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("InviteUserIntoRTCRoom", answer);
    if (answer && answer->status() != 0)
    {
    }
    else
    {
    }
}

RTMClient::RTCRoomMembers RTMClient::GetRTCRoomMembers(int64_t rid)
{
    FPQWriter qw(1, "getRTCRoomMembers");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRTCRoomMembers", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("GetRTCRoomMembers", answer);
    RTCRoomMembers result;
    if (answer && answer->status() != 0)
    {
        return result;
    }
    FPAReader ar(answer);
    result.administrators = ar.get("administrators", result.administrators);
    result.owner = ar.get("owner", result.owner);
    result.uids = ar.get("uids", result.uids);

    return result;
}

int32_t RTMClient::GetRTCRoomMemberCount(int64_t rid)
{
    FPQWriter qw(1, "getRTCRoomMembers");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRTCRoomMemberCount", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("GetRTCRoomMemberCount", answer);
    int32_t result;
    if (answer && answer->status() != 0)
    {
        return -1;
    }
    FPAReader ar(answer);
    result = (int32_t)ar.getInt("count");

    return result;
}

void RTMClient::BlockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids)
{
    FPQWriter qw(2, "blockUserVoiceInRTCRoom");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("BlockUserVoiceInRTCRoom", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("BlockUserVoiceInRTCRoom", answer);
    if (answer && answer->status() != 0)
    {
    }
    else
    {
    }
}

void RTMClient::UnblockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids)
{
    FPQWriter qw(2, "unblockUserVoiceInRTCRoom");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("UnblockUserVoiceInRTCRoom", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("UnblockUserVoiceInRTCRoom", answer);
    if (answer && answer->status() != 0)
    {
    }
    else
    {
    }
}

void RTMClient::AdminCommand(int64_t rid, unordered_set<int64_t> uids, RTCEventHandler::AdminCommand command)
{
    FPQWriter qw(3, "adminCommand");
    qw.param("rid", rid);
    qw.param("uids", uids);
    qw.param("command", (int32_t)command);

    FPQuestPtr quest = qw.take();
    printQuestJson("AdminCommand", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("AdminCommand", answer);
    if (answer && answer->status() != 0)
    {
    }
    else 
    {
    }
}

void RTMClient::RoomEvent(int64_t rid, vector<unsigned char> revent)
{
    FPQWriter qw(2, "roomEvent");
    qw.param("rid", rid);
    qw.param("event", revent);

    FPQuestPtr quest = qw.take();
    printQuestJson("RoomEvent", quest);
    FPAnswerPtr answer = rtmClient->sendQuest(quest);
    printAnswerJson("RoomEvent", answer);
    if (answer && answer->status() != 0)
    {
    }
    else
    {
    }
}

/***********************************************Asynchronous interface*****************************************************/
void RTMClient::EnterRTCRoom(int64_t rid, function<void(int errorCode, string token)> callback)
{
    string result;
    FPQWriter qw(1, "enterroom");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("EnterRTCRoom", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("EnterRTCRoom", answer);
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, "");
        }
        else
        {
            FPAReader ar(answer);
            string token = ar.getString("token");
            callback(errorCode, token);
        }
        });

}

void RTMClient::CreateRTCRoom(int32_t type, int64_t rid, int32_t enableRecord, function<void(int errorCode, string token)> callback)
{
    string result;
    FPQWriter qw(3, "createRTCRoom");
    qw.param("rid", rid);
    qw.param("type", 2);
    qw.param("enableRecord", 0);

    FPQuestPtr quest = qw.take();
    printQuestJson("CreateRTCRoom", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("CreateRTCRoom", answer);
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, "");
        }
        else
        {
            FPAReader ar(answer);
            string token = ar.getString("token");
            callback(errorCode, token);
        }
        });
}

void RTMClient::ExitRTCRoom(int64_t rid, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "exitRTCRoom");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("ExitRTCRoom", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("ExitRTCRoom", answer);
        callback(errorCode);
        });
}

void RTMClient::SubscribeVideo(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "subscribeVideo");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("SubscribeVideo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("SubscribeVideo", answer);
        callback(errorCode);
        });
}

void RTMClient::UnsubscribeVideo(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "unsubscribeVideo");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("UnsubscribeVideo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("UnsubscribeVideo", answer);
        callback(errorCode);
        });
}

void RTMClient::InviteUserIntoRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "inviteUserIntoRTCRoom");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("InviteUserIntoRTCRoom", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("InviteUserIntoRTCRoom", answer);
        callback(errorCode);
        });
}

void RTMClient::GetRTCRoomMembers(int64_t rid, function<void(int errorCode, RTCRoomMembers roomMembers)> callback)
{
    FPQWriter qw(1, "getRTCRoomMembers");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRTCRoomMembers", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetRTCRoomMembers", answer);
        RTCRoomMembers result;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, result);
            return;
        }
        FPAReader ar(answer);
        result.administrators = ar.get("administrators", result.administrators);
        result.owner = ar.get("owner", result.owner);
        result.uids = ar.get("uids", result.uids);
        callback(errorCode, result);
        });
}

void RTMClient::GetRTCRoomMemberCount(int64_t rid, function<void(int errorCode, int32_t count)> callback)
{
    FPQWriter qw(1, "getRTCRoomMemberCount");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRTCRoomMemberCount", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetRTCRoomMemberCount", answer);
        int32_t result = 0;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, result);
            return;
        }
        FPAReader ar(answer);
        result = ar.getInt("count");
        callback(errorCode, result);
        });
}

void RTMClient::BlockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "blockUserVoiceInRTCRoom");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("BlockUserVoiceInRTCRoom", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("BlockUserVoiceInRTCRoom", answer);
        callback(errorCode);
        });
}

void RTMClient::UnblockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "unblockUserVoiceInRTCRoom");
    qw.param("rid", rid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("UnblockUserVoiceInRTCRoom", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("UnblockUserVoiceInRTCRoom", answer);
        callback(errorCode);
        });
}

void RTMClient::AdminCommand(int64_t rid, unordered_set<int64_t> uids, RTCEventHandler::AdminCommand command, function<void(int errorCode)> callback)
{
    FPQWriter qw(3, "adminCommand");
    qw.param("rid", rid);
    qw.param("uids", uids);
    qw.param("command", (int32_t)command);

    FPQuestPtr quest = qw.take();
    printQuestJson("AdminCommand", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("AdminCommand", answer);
        callback(errorCode);
        });
}

void RTMClient::RoomEvent(int64_t rid, vector<unsigned char> revent, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "roomEvent");
    qw.param("rid", rid);
    qw.param("event", revent);

    FPQuestPtr quest = qw.take();
    printQuestJson("RoomEvent", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("RoomEvent", answer);
        callback(errorCode);
        });
}


void RTMClient::RequestP2PRTC(int32_t type, int64_t peerUid, function<void(int errorCode, int64_t callId)> callback)
{
	FPQWriter qw(2, "requestP2PRTC");
	qw.param("type", type);
	qw.param("peerUid", peerUid);

    FPQuestPtr quest = qw.take();
    printQuestJson("RequestP2PRTC", quest);
	rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("RequestP2PRTC", answer);
		int64_t callId = 0;
        if (errorCode != FPNN_EC_OK)
		{
            callback(errorCode, callId);
            return;
		}
        FPAReader ar(answer);
        callId = ar.getInt("callId");
		callback(errorCode, callId);
		});
}

void RTMClient::CancelP2PRTC(int64_t callId, function<void(int errorCode)> callback)
{
	FPQWriter qw(1, "cancelP2PRTC");
	qw.param("callId", callId);

    FPQuestPtr quest = qw.take();
    printQuestJson("CancelP2PRTC", quest);
	rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("CancelP2PRTC", answer);
		callback(errorCode);
		});
}

void RTMClient::CloseP2PRTC(int64_t callId, function<void(int errorCode)> callback)
{
	FPQWriter qw(1, "closeP2PRTC");
	qw.param("callId", callId);

    FPQuestPtr quest = qw.take();
    printQuestJson("CloseP2PRTC", quest);
	rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("CloseP2PRTC", answer);
		callback(errorCode);
		});
}

void RTMClient::AcceptP2PRTC(int64_t callId, function<void(int errorCode)> callback)
{
	FPQWriter qw(1, "acceptP2PRTC");
	qw.param("callId", callId);

    FPQuestPtr quest = qw.take();
    printQuestJson("AcceptP2PRTC", quest);
	rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("AcceptP2PRTC", answer);
		callback(errorCode);
		});
}

void RTMClient::RefuseP2PRTC(int64_t callId, function<void(int errorCode)> callback)
{
	FPQWriter qw(1, "refuseP2PRTC");
	qw.param("callId", callId);

    FPQuestPtr quest = qw.take();
    printQuestJson("RefuseP2PRTC", quest);
	rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("RefuseP2PRTC", answer);
		callback(errorCode);
		});
}


void RTMClient::AddAttrs(unordered_map<string, string> attrs, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "addattrs");
    qw.param("attrs", attrs);

    FPQuestPtr quest = qw.take();
    printQuestJson("AddAttrs", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("AddAttrs", answer);
        callback(errorCode);
    });
}

void RTMClient::GetAttrs(function<void(int errorCode, unordered_map<string, string> attrs)> callback)
{
    rtmClient->sendQuest(FPQWriter::emptyQuest("getattrs"), [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("AddAttrs", answer);
        unordered_map<string, string> attrs;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, attrs);
            return;
        }
        FPAReader ar(answer);
        attrs = ar.get("attrs", attrs);
        callback(errorCode, attrs);
    });
}

void RTMClient::AddDebugLog(string msg, string attrs, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "adddebuglog");
    qw.param("msg", msg);
    qw.param("attrs", attrs);

    FPQuestPtr quest = qw.take();
    printQuestJson("AddDebugLog", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("AddDebugLog", answer);
        callback(errorCode);
    });
}

void RTMClient::GetServerTime(function<void(int errorCode, int64_t mts)> callback)
{
    rtmClient->sendQuest(FPQWriter::emptyQuest("getservertime"), [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetServerTime", answer);
        int64_t mts = 0;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, mts);
            return;
        }
        FPAReader ar(answer);
        mts = ar.getInt("mts", mts);
        callback(errorCode, mts);
    });
}

void RTMClient::SendChat(int64_t to, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    SendMsg(to, mid, 30, msg, attrs, callback);
}

void RTMClient::SendCmd(int64_t to, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    SendMsg(to, mid, 32, msg, attrs, callback);
}

void RTMClient::SendGroupChat(int64_t gid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    SendGroupMsg(gid, mid, 30, msg, attrs, callback);
}

void RTMClient::SendGroupCmd(int64_t gid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    SendGroupMsg(gid, mid, 32, msg, attrs, callback);
}

void RTMClient::SendRoomChat(int64_t rid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    SendRoomMsg(rid, mid, 30, msg, attrs, callback);
}

void RTMClient::SendRoomCmd(int64_t rid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    SendRoomMsg(rid, mid, 32, msg, attrs, callback);
}

void RTMClient::GetGroupChat(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    unordered_set<int8_t> mtypes;
    mtypes.insert(30);
    GetGroupMsg(gid, desc, num, begin, end, lastid, mtypes, callback);
}

void RTMClient::GetRoomChat(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    unordered_set<int8_t> mtypes;
    mtypes.insert(30);
    GetRoomMsg(rid, desc, num, begin, end, lastid, mtypes, callback);
}

void RTMClient::GetBroadcastChat(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    unordered_set<int8_t> mtypes;
    mtypes.insert(30);
    GetBroadcastMsg(desc, num, begin, end, lastid, mtypes, callback);
}

void RTMClient::GetP2PChat(int64_t uid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    unordered_set<int8_t> mtypes;
    mtypes.insert(30);
    GetP2PMsg(uid, desc, num, begin, end, lastid, mtypes, callback);
}

void RTMClient::DelChat(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode)> callback)
{
    DelMsg(from, mid, xid, 30, callback);
}

void RTMClient::GetChat(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime)> callback)
{
    GetMsg(from, mid, xid, 30, callback);
}

     
void RTMClient::SendMsg(int64_t to, int64_t mid, int8_t mtype, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    FPQWriter qw(5, "sendmsg");
    qw.param("to", to);
    qw.param("mid", mid);
    qw.param("mtype", mtype);
    qw.param("msg", msg);
    qw.param("attrs", attrs);

    FPQuestPtr quest = qw.take();
    printQuestJson("SendMsg", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("SendMsg", answer);
        int64_t mtime = 0;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, mtime);
            return;
        }
        FPAReader ar(answer);
        mtime = ar.getInt("mtime", mtime);
        callback(errorCode, mtime);
    });
}

void RTMClient::SendGroupMsg(int64_t gid, int64_t mid, int8_t mtype, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    FPQWriter qw(5, "sendgroupmsg");
    qw.param("gid", gid);
    qw.param("mid", mid);
    qw.param("mtype", mtype);
    qw.param("msg", msg);
    qw.param("attrs", attrs);

    FPQuestPtr quest = qw.take();
    printQuestJson("SendGroupMsg", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("SendGroupMsg", answer);
        int64_t mtime = 0;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, mtime);
            return;
        }
        FPAReader ar(answer);
        mtime = ar.getInt("mtime", mtime);
        callback(errorCode, mtime);
    });
}

void RTMClient::SendRoomMsg(int64_t rid, int64_t mid, int8_t mtype, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    FPQWriter qw(5, "sendroommsg");
    qw.param("rid", rid);
    qw.param("mid", mid);
    qw.param("mtype", mtype);
    qw.param("msg", msg);
    qw.param("attrs", attrs);

    FPQuestPtr quest = qw.take();
    printQuestJson("SendRoomMsg", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("SendRoomMsg", answer);
        int64_t mtime = 0;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, mtime);
            return;
        }
        FPAReader ar(answer);
        mtime = ar.getInt("mtime", mtime);
        callback(errorCode, mtime);
    });
}

     
void RTMClient::GetGroupMsg(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes,
    function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    FPQWriter qw(7, "getgroupmsg");
    qw.param("gid", gid);
    qw.param("desc", desc);
    qw.param("num", num);
    qw.param("begin", begin);
    qw.param("end", end);
    qw.param("lastid", lastid);
    qw.param("mtypes", mtypes);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetGroupMsg", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetGroupMsg", answer);
        int16_t num = 0;
        int64_t lastid = 0;
        int64_t begin = 0;
        int64_t end = 0;
        vector<CommonMsg> msgs;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, num, lastid, begin, end, msgs);
            return;
        }

        FPAReader ar(answer);
        num = ar.getInt("num", num);
        lastid = ar.getInt("lastid", lastid);
        begin = ar.getInt("begin", begin);
        end = ar.getInt("end", end);
        msgs = ar.get("msgs", msgs);

        callback(errorCode, num, lastid, begin, end, msgs);
    });
}

void RTMClient::GetRoomMsg(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, 
    function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    FPQWriter qw(7, "getroommsg");
    qw.param("rid", rid);
    qw.param("desc", desc);
    qw.param("num", num);
    qw.param("begin", begin);
    qw.param("end", end);
    qw.param("lastid", lastid);
    qw.param("mtypes", mtypes);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRoomMsg", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetRoomMsg", answer);
        int16_t num = 0;
        int64_t lastid = 0;
        int64_t begin = 0;
        int64_t end = 0;
        vector<CommonMsg> msgs;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, num, lastid, begin, end, msgs);
            return;
        }

        FPAReader ar(answer);
        num = ar.getInt("num", num);
        lastid = ar.getInt("lastid", lastid);
        begin = ar.getInt("begin", begin);
        end = ar.getInt("end", end);
        msgs = ar.get("msgs", msgs);

        callback(errorCode, num, lastid, begin, end, msgs);
    });
}

void RTMClient::GetBroadcastMsg(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, 
    function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    FPQWriter qw(6, "getbroadcastmsg");
    qw.param("desc", desc);
    qw.param("num", num);
    qw.param("begin", begin);
    qw.param("end", end);
    qw.param("lastid", lastid);
    qw.param("mtypes", mtypes);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetBroadcastMsg", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetBroadcastMsg", answer);
        int16_t num = 0;
        int64_t lastid = 0;
        int64_t begin = 0;
        int64_t end = 0;
        vector<CommonMsg> msgs;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, num, lastid, begin, end, msgs);
            return;
        }

        FPAReader ar(answer);
        num = ar.getInt("num", num);
        lastid = ar.getInt("lastid", lastid);
        begin = ar.getInt("begin", begin);
        end = ar.getInt("end", end);
        msgs = ar.get("msgs", msgs);

        callback(errorCode, num, lastid, begin, end, msgs);
    });
}

void RTMClient::GetP2PMsg(int64_t ouid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes,
    function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    FPQWriter qw(7, "getp2pmsg");
    qw.param("ouid", ouid);
    qw.param("desc", desc);
    qw.param("num", num);
    qw.param("begin", begin);
    qw.param("end", end);
    qw.param("lastid", lastid);
    qw.param("mtypes", mtypes);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetP2PMsg", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetP2PMsg", answer);
        int16_t num = 0;
        int64_t lastid = 0;
        int64_t begin = 0;
        int64_t end = 0;
        vector<CommonMsg> msgs;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, num, lastid, begin, end, msgs);
            return;
        }
        
        FPAReader ar(answer);
        num = ar.getInt("num", num);
        lastid = ar.getInt("lastid", lastid);
        begin = ar.getInt("begin", begin);
        end = ar.getInt("end", end);
        msgs = ar.get("msgs", msgs);

        callback(errorCode, num, lastid, begin, end, msgs);
    });
}

void RTMClient::DelMsg(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode)> callback)
{
    FPQWriter qw(4, "delmsg");
    qw.param("from", from);
    qw.param("mid", mid);
    qw.param("xid", xid);
    qw.param("type", type);

    FPQuestPtr quest = qw.take();
    printQuestJson("DelMsg", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("DelMsg", answer);
        callback(errorCode);
    });
}

void RTMClient::GetMsg(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime)> callback)
{
    FPQWriter qw(4, "getmsg");
    qw.param("from", from);
    qw.param("mid", mid);
    qw.param("xid", xid);
    qw.param("type", type);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetMsg", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetMsg", answer);
        int64_t id = 0;
        int8_t mtype = 0;
        int64_t mtime = 0;
        string msg, attrs;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, id, mtype, msg, attrs, mtime);
            return;
        }
        FPAReader ar(answer);
        id = ar.getInt("id", id);
        mtype = ar.getInt("mtype", mtype);
        msg = ar.getString("msg", msg);
        attrs = ar.getString("attrs", attrs);
        mtime = ar.getInt("mtime", mtime);
        callback(errorCode, id, mtype, msg, attrs, mtime);
    });
}

void RTMClient::GetUnread(bool clean, function<void(int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group)> callback)
{
    FPQWriter qw(1, "getunread");
    qw.param("clean", clean);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetUnread", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetUnread", answer);
        unordered_set<int64_t> p2p,group;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, p2p, group);
            return;
        }
        FPAReader ar(answer);
        p2p = ar.get("p2p", p2p);
        group = ar.get("group", group);
        callback(errorCode, p2p, group);
    });
}

void RTMClient::CleanUnread(function<void(int errorCode)> callback)
{
    rtmClient->sendQuest(FPQWriter::emptyQuest("cleanunread"), [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("CleanUnread", answer);
        callback(errorCode);
        });
}

void RTMClient::GetSession(function<void(int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group)> callback)
{
    rtmClient->sendQuest(FPQWriter::emptyQuest("getsession"), [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetSession", answer);
        unordered_set<int64_t> p2p, group;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, p2p, group);
            return;
        }

        FPAReader ar(answer);
        p2p = ar.get("p2p", p2p);
        group = ar.get("group", group);
        callback(errorCode, p2p, group);
    });
}

void RTMClient::GetP2PUnread(unordered_set<int64_t> uids, int64_t mtime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime)> callback)
{
    FPQWriter qw(3, "getp2punread");
    qw.param("uids", uids);
    qw.param("mtime", mtime);
    qw.param("mtypes", mtypes);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetP2PUnread", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetP2PUnread", answer);
        unordered_map<string, int32_t> p2p;
        unordered_map<string, int64_t> ltime;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, p2p, ltime);
            return;
        }

        FPAReader ar(answer);
        p2p = ar.get("p2p", p2p);
        ltime = ar.get("ltime", ltime);
        callback(errorCode, p2p, ltime);
    });
}

void RTMClient::GetGroupUnread(unordered_set<int64_t> gids, int64_t mtime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime)> callback)
{
    FPQWriter qw(3, "getgroupunread");
    qw.param("gids", gids);
    qw.param("mtime", mtime);
    qw.param("mtypes", mtypes);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetGroupUnread", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetGroupUnread", answer);
        unordered_map<string, int32_t> group;
        unordered_map<string, int64_t> ltime;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, group, ltime);
            return;
        }

        FPAReader ar(answer);
        group = ar.get("group", group);
        ltime = ar.get("ltime", ltime);
        callback(errorCode, group, ltime);
    });
}

// imlib
void RTMClient::CleanTargetUnread(unordered_set<int64_t> uids, unordered_set<int64_t> gids, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "cleantargetunread");
    qw.param("uids", uids);
    qw.param("gids", gids);

    FPQuestPtr quest = qw.take();
    printQuestJson("CleanTargetUnread", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("CleanTargetUnread", answer);
        callback(errorCode);
    });
}

// imlib
void RTMClient::GetP2PUnreadByTime(unordered_map<string, int64_t> uidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime)> callback)
{
    FPQWriter qw(2, "getp2punreadbytime");
    qw.param("uid_time", uidTime);
    qw.param("mtypes", mtypes);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetP2PUnreadByTime", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetP2PUnreadByTime", answer);
        unordered_map<string, int32_t> p2p;
        unordered_map<string, int64_t> ltime;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, p2p, ltime);
            return;
        }

        FPAReader ar(answer);
        p2p = ar.get("p2p", p2p);
        ltime = ar.get("ltime", ltime);
        callback(errorCode, p2p, ltime);
    });
}

// imlib
void RTMClient::GetGroupUnreadByTime(unordered_map<string, int64_t> gidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime)> callback)
{
    FPQWriter qw(2, "getgroupunreadbytime");
    qw.param("gid_time", gidTime);
    qw.param("mtypes", mtypes);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetGroupUnreadByTime", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetGroupUnreadByTime", answer);
        unordered_map<string, int32_t> group;
        unordered_map<string, int64_t> ltime;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, group, ltime);
            return;
        }

        FPAReader ar(answer);
        group = ar.get("group", group);
        ltime = ar.get("ltime", ltime);
        callback(errorCode, group, ltime);
    });
}

    
void RTMClient::SetLang(string lang, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "setlang");
    qw.param("lang", lang);

    FPQuestPtr quest = qw.take();
    printQuestJson("SetLang", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("SetLang", answer);
        callback(errorCode);
    });
}

void RTMClient::Translate(string text, string src, string dst, string type, string profanity, function<void(int errorCode, string source, string target, string sourceText, string targetText)> callback)
{
    FPQWriter qw(5, "translate");
    qw.param("text", text);
    qw.param("src", src);
    qw.param("dst", dst);
    qw.param("type", type);
    qw.param("profanity", profanity);

    FPQuestPtr quest = qw.take();
    printQuestJson("Translate", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("Translate", answer);
        string source, target, sourceText, targetText;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, source, target, sourceText, targetText);
            return;
        }
        FPAReader ar(answer);
        source = ar.getString("source", source);
        target = ar.getString("target", target);
        sourceText = ar.getString("sourceText", sourceText);
        targetText = ar.getString("targetText", targetText);
        callback(errorCode, source, target, sourceText, targetText);
    });
}

void RTMClient::Profanity(string text, bool classify, function<void(int errorCode, string text, vector<string> classification)> callback)
{
    FPQWriter qw(2, "profanity");
    qw.param("text", text);
    qw.param("classify", classify);

    FPQuestPtr quest = qw.take();
    printQuestJson("Profanity", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("Profanity", answer);
        string text;
        vector<string> classification;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, text, classification);
            return;
        }
        FPAReader ar(answer);
        text = ar.getString("text", text);
        classification = ar.get("classification", classification);
        callback(errorCode, text, classification);
    });
}

void RTMClient::Speech2Text(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, string text, string lang)> callback)
{
    FPQWriter qw(5, "speech2text");
    qw.param("audio", audio);
    qw.param("type", type);
    qw.param("lang", lang);
    qw.param("codec", codec);
    qw.param("srate", srate);

    FPQuestPtr quest = qw.take();
    printQuestJson("Speech2Text", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("Speech2Text", answer);
        string text, lang;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, text, lang);
            return;
        }

        FPAReader ar(answer);
        text = ar.getString("text", text);
        lang = ar.getString("lang", lang);
        callback(errorCode, text, lang);
    });
}

void RTMClient::TCheck(string text, function<void(int errorCode, int32_t result, string text, vector<string> tags, vector<string> wlist)> callback)
{
    FPQWriter qw(1, "tcheck");
    qw.param("text", text);

    FPQuestPtr quest = qw.take();
    printQuestJson("TCheck", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("TCheck", answer);
        int32_t result = 0;
        string text;
        vector<string> tags;
        vector<string> wlist;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, result, text, tags, wlist);
            return;
        }

        FPAReader ar(answer);
        result = ar.getInt("result", result);
        text = ar.getString("text", text);
        tags = ar.get("tags", tags);
        wlist = ar.get("wlist", wlist);

        callback(errorCode, result, text, tags, wlist);
    });
}

void RTMClient::ICheck(string image, int32_t type, function<void(int errorCode, int32_t result, vector<string> tags)> callback)
{
    FPQWriter qw(2, "icheck");
    qw.param("image", image);
    qw.param("type", type);

    FPQuestPtr quest = qw.take();
    printQuestJson("ICheck", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("ICheck", answer);
        int32_t result = 0;
        vector<string> tags;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, result, tags);
            return;
        }

        FPAReader ar(answer);
        result = ar.getInt("result", result);
        tags = ar.get("tags", tags);
        callback(errorCode, result, tags);
    });
}

void RTMClient::ACheck(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, int32_t result, vector<string> tags)> callback)
{
    FPQWriter qw(5, "acheck");
    qw.param("audio", audio);
    qw.param("type", type);
    qw.param("lang", lang);
    qw.param("codec", codec);
    qw.param("srate", srate);

    FPQuestPtr quest = qw.take();
    printQuestJson("ACheck", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("ACheck", answer);
        int32_t result = 0;
        vector<string> tags;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, result, tags);
            return;
        }

        FPAReader ar(answer); 
        result = ar.getInt("result", result);
        tags = ar.get("tags", tags);

        callback(errorCode, result, tags);
    });
}

void RTMClient::VCheck(string video, int32_t type, string videoName, function<void(int errorCode, int32_t result, vector<string> tags)> callback)
{
    FPQWriter qw(3, "vcheck");
    qw.param("video", video);
    qw.param("type", type);
    qw.param("videoName", videoName);

    FPQuestPtr quest = qw.take();
    printQuestJson("VCheck", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("VCheck", answer);
        int32_t result = 0;;
        vector<string> tags;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, result, tags);
            return;
        }

        FPAReader ar(answer);
        result = ar.getInt("result", result);
        tags = ar.get("tags", tags);

        callback(errorCode, result, tags);
    });
}
    
void RTMClient::GetOnlineUsers(unordered_set<int64_t> uids, function<void(int errorCode, unordered_set<int64_t> uids)> callback)
{
    FPQWriter qw(1, "getonlineusers");
    qw.param("uids", uids);
 
    FPQuestPtr quest = qw.take();
    printQuestJson("GetOnlineUsers", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetOnlineUsers", answer);
        unordered_set<int64_t> uids;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, uids);
            return;
        }

        FPAReader ar(answer);
        uids = ar.get("uids", uids);
        callback(errorCode, uids);
    });
}

void RTMClient::SetUserInfo(string oinfo, string pinfo, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "setuserinfo");
    qw.param("oinfo", oinfo);
    qw.param("pinfo", pinfo);

    FPQuestPtr quest = qw.take();
    printQuestJson("SetUserInfo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("SetUserInfo", answer);
        callback(errorCode);
    });
}

void RTMClient::GetUserInfo(function<void(int errorCode, string oinfo, string pinfo)> callback)
{
    rtmClient->sendQuest(FPQWriter::emptyQuest("getuserinfo"), [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetUserInfo", answer);
        string oinfo, pinfo;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, oinfo, pinfo);
            return;
        }

        FPAReader ar(answer);
        oinfo = ar.getString("oinfo", oinfo);
        pinfo = ar.getString("pinfo", pinfo);
        callback(errorCode, oinfo, pinfo);
    });
}

void RTMClient::GetUserOpenInfo(unordered_set<int64_t> uids, function<void(int errorCode, unordered_map<string, string> info)> callback)
{
    FPQWriter qw(1, "getuseropeninfo");
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetUserOpenInfo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetUserOpenInfo", answer);
        unordered_map<string, string> info;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, info);
            return;
        }

        FPAReader ar(answer);
        info = ar.get("info", info);
        callback(errorCode, info);
    });
}

     
void RTMClient::AddFriends(unordered_set<int64_t> friends, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "addfriends");
    qw.param("friends", friends);

    FPQuestPtr quest = qw.take();
    printQuestJson("AddFriends", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("AddFriends", answer);
        callback(errorCode);
    });
}

void RTMClient::DelFriends(unordered_set<int64_t> friends, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "delfriends");
    qw.param("friends", friends);

    FPQuestPtr quest = qw.take();
    printQuestJson("DelFriends", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("DelFriends", answer);
        callback(errorCode);
    });
}

void RTMClient::GetFriends(function<void(int errorCode, unordered_set<int64_t> uids)> callback)
{
    rtmClient->sendQuest(FPQWriter::emptyQuest("getfriends"), [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetFriends", answer);
        unordered_set<int64_t> uids;
        if (errorCode != FPNN_EC_OK)
        {
            return;
        }
        FPAReader ar(answer);
        uids = ar.get("uids", uids);
        callback(errorCode, uids);
    });
}

void RTMClient::AddBlacks(unordered_set<int64_t> blacks, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "addblacks");
    qw.param("blacks", blacks);

    FPQuestPtr quest = qw.take();
    printQuestJson("AddBlacks", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("AddBlacks", answer);
        callback(errorCode);
    });
}

void RTMClient::DelBlacks(unordered_set<int64_t> blacks, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "delblacks");
    qw.param("blacks", blacks);

    FPQuestPtr quest = qw.take();
    printQuestJson("DelBlacks", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("DelBlacks", answer);
        callback(errorCode);
    });
}

void RTMClient::GetBlacks(function<void(int errorCode, unordered_set<int64_t> uids)> callback)
{
    rtmClient->sendQuest(FPQWriter::emptyQuest("getblacks"), [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetBlacks", answer);
        unordered_set<int64_t> uids;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, uids);
            return;
        }

        FPAReader ar(answer);
        uids = ar.get("uids", uids);
        callback(errorCode, uids);
    });
}

     
void RTMClient::AddGroupMembers(int64_t gid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "addgroupmembers");
    qw.param("gid", gid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("AddGroupMembers", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("AddGroupMembers", answer);
        callback(errorCode);
    });
}

void RTMClient::DelGroupMembers(int64_t gid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "delgroupmembers");
    qw.param("gid", gid);
    qw.param("uids", uids);

    FPQuestPtr quest = qw.take();
    printQuestJson("DelGroupMembers", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("DelGroupMembers", answer);
        callback(errorCode);
    });
}

void RTMClient::GetGroupMembers(int64_t gid, bool online, function<void(int errorCode, unordered_set<int64_t> uids, unordered_set<int64_t> onlines)> callback)
{
    FPQWriter qw(2, "getgroupmembers");
    qw.param("gid", gid);
    qw.param("online", online);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetGroupMembers", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetGroupMembers", answer);
        unordered_set<int64_t> uids, onlines;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, uids, onlines);
            return;
        }

        FPAReader ar(answer);
        uids = ar.get("uids", uids);
        onlines = ar.get("onlines", onlines);
        callback(errorCode, uids, onlines);
    });
}

void RTMClient::GetGroupCount(int64_t gid, bool online, function<void(int errorCode, int32_t cn, int32_t onlines)> callback)
{
    FPQWriter qw(2, "getgroupcount");
    qw.param("gid", gid);
    qw.param("online", online);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetGroupCount", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetGroupCount", answer);
        int32_t cn = 0;
        int32_t onlines = 0;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, cn, onlines);
            return;
        }

        FPAReader ar(answer);
        ar.getInt("cn", cn);
        ar.getInt("online", onlines);
        callback(errorCode, cn, onlines);
    });
}

void RTMClient::GetUserGroups(function<void(int errorCode, unordered_set<int64_t> gids)> callback)
{
    rtmClient->sendQuest(FPQWriter::emptyQuest("getusergroups"), [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetUserGroups", answer);
        unordered_set<int64_t> gids;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, gids);
            return;
        }

        FPAReader ar(answer);
        gids = ar.get("gids", gids);
        callback(errorCode, gids);
    });
}

void RTMClient::SetGroupInfo(int64_t gid, string oinfo, string pinfo, function<void(int errorCode)> callback)
{
    FPQWriter qw(3, "setgroupinfo");
    qw.param("gid", gid);
    qw.param("oinfo", oinfo);
    qw.param("pinfo", pinfo);

    FPQuestPtr quest = qw.take();
    printQuestJson("SetGroupInfo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("SetGroupInfo", answer);
        callback(errorCode);
    });
}

void RTMClient::GetGroupOpenInfo(int64_t gid, function<void(int errorCode, string oinfo)> callback)
{
    FPQWriter qw(1, "getgroupopeninfo");
    qw.param("gid", gid);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetGroupOpenInfo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetGroupOpenInfo", answer);
        string oinfo;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, oinfo);
            return;
        }

        FPAReader ar(answer);
        oinfo = ar.getString("oinfo", oinfo);
        callback(errorCode, oinfo);
    });
}

void RTMClient::GetGroupsOpenInfo(unordered_set<int64_t> gids, function<void(int errorCode, unordered_map<string, string> info)> callback)
{
    FPQWriter qw(1, "getgroupsopeninfo");
    qw.param("gids", gids);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetGroupsOpenInfo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetGroupsOpenInfo", answer);
        unordered_map<string, string> info;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, info);
            return;
        }

        FPAReader ar(answer);
        info = ar.get("info", info);
        callback(errorCode, info);
    });
}

     
void RTMClient::EnterRoom(int64_t rid, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "enterroom");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("EnterRoom", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("EnterRoom", answer);
        callback(errorCode);
    });
}

void RTMClient::LeaveRoom(int64_t rid, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "leaveroom");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("LeaveRoom", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("LeaveRoom", answer);
        callback(errorCode);
    });
}

void RTMClient::GetUserRooms(function<void(int errorCode, unordered_set<int64_t> rooms)> callback)
{
    rtmClient->sendQuest(FPQWriter::emptyQuest("getuserrooms"), [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetUserRooms", answer);
        unordered_set<int64_t> rooms;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, rooms);
            return;
        }

        FPAReader ar(answer);
        rooms = ar.get("rooms", rooms);
        callback(errorCode, rooms);
    });
}

void RTMClient::SetRoomInfo(int64_t rid, string oinfo, string pinfo, function<void(int errorCode)> callback)
{
    int32_t size = 3;
    if (oinfo.empty())
        size -= 1;
    if (pinfo.empty())
        size -= 1;

    FPQWriter qw(size, "setroominfo");
    qw.param("rid", rid);
    if (!oinfo.empty())
        qw.param("oinfo", oinfo);
    if (!pinfo.empty())
        qw.param("pinfo", pinfo);

    FPQuestPtr quest = qw.take();
    printQuestJson("SetRoomInfo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("SetRoomInfo", answer);
        callback(errorCode);
    });
}

void RTMClient::GetRoomInfo(int64_t rid, function<void(int errorCode, string oinfo, string pinfo)> callback)
{
    FPQWriter qw(1, "getroominfo");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRoomInfo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetRoomInfo", answer);
        string oinfo, pinfo;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, oinfo, pinfo);
            return;
        }
        FPAReader ar(answer);
        oinfo = ar.getString("oinfo", oinfo);
        pinfo = ar.getString("pinfo", pinfo);
        callback(errorCode, oinfo, pinfo);
    });
}

void RTMClient::GetRoomOpenInfo(int64_t rid, function<void(int errorCode, string oinfo)> callback)
{
    FPQWriter qw(1, "getroomopeninfo");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRoomOpenInfo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetRoomOpenInfo", answer);
        string oinfo;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, oinfo);
            return;
        }

        FPAReader ar(answer);
        oinfo = ar.getString("oinfo", oinfo);
        callback(errorCode, oinfo);
    });
}

void RTMClient::GetRoomsOpenInfo(unordered_set<int64_t> rids, function<void(int errorCode, unordered_map<string, string> info)> callback)
{
    FPQWriter qw(1, "getroomsopeninfo");
    qw.param("rids", rids);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRoomsOpenInfo", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetRoomsOpenInfo", answer);
        unordered_map<string, string> info;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, info);
            return;
        }

        FPAReader ar(answer);
        info = ar.get("info", info);
        callback(errorCode, info);
    });
}

void RTMClient::GetRoomMemers(int64_t rid, function<void(int errorCode, unordered_set<int64_t> uids)> callback)
{
    FPQWriter qw(1, "getroommembers");
    qw.param("rid", rid);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRoomMemers", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetRoomMemers", answer);
        unordered_set<int64_t> uids;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, uids);
            return;
        }

        FPAReader ar(answer);
        uids = ar.get("uids", uids);
        callback(errorCode, uids);
    });
}

void RTMClient::GetRoomCount(unordered_set<int64_t> rids, function<void(int errorCode, unordered_map<string, int32_t> info)> callback)
{
    FPQWriter qw(1, "getroomcount");
    qw.param("rids", rids);

    FPQuestPtr quest = qw.take();
    printQuestJson("GetRoomCount", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("GetRoomCount", answer);
        unordered_map<string, int32_t> info;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, info);
            return;
        }

        FPAReader ar(answer);
        info = ar.get("cn", info);
        callback(errorCode, info);
    });
}

     
void RTMClient::DataGet(string key, function<void(int errorCode, string val)> callback)
{
    FPQWriter qw(1, "dataget");
    qw.param("key", key);

    FPQuestPtr quest = qw.take();
    printQuestJson("DataGet", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("DataGet", answer);
        string val;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, val);
            return;
        }
        FPAReader ar(answer);
        val = ar.get("val", val);
        callback(errorCode, val);
    });
}

void RTMClient::DataSet(string key, string val, function<void(int errorCode)> callback)
{
    FPQWriter qw(2, "dataset");
    qw.param("key", key);
    qw.param("val", val);

    FPQuestPtr quest = qw.take();
    printQuestJson("DataSet", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("DataSet", answer);
        callback(errorCode);
    });
}

void RTMClient::DataDel(string key, function<void(int errorCode)> callback)
{
    FPQWriter qw(1, "datadel");
    qw.param("key", key);

    FPQuestPtr quest = qw.take();
    printQuestJson("DataDel", quest);
    rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("DataDel", answer);
        callback(errorCode);
    });
}

void printQuestJson(std::string method, FPQuestPtr quest)
{
    if (!quest)
        return;

#ifdef _DEBUG
    std::cout << "/*********************************************************************************" << std::endl;
    std::cout << method << ":" << quest->info() << std::endl;
#endif // _DEBUG
}

void printAnswerJson(std::string method, FPAnswerPtr answer)
{
    if (!answer)
        return;

#ifdef _DEBUG
    std::cout << method << ":" << answer->info() << std::endl;
    std::cout << "*********************************************************************************/" << std::endl;
#endif // _DEBUG
}
RTMConfig rtmConfig;
RTMRegressiveStrategy rtmRegressiveStrategy;

#ifndef RTMGATEQUESTPROCESSOR_H
#define RTMGATEQUESTPROCESSOR_H

#include <atomic>
#include <IQuestProcessor.h>
#include "RTCEventHandler.h"
#include "RTMEventHandler.h"
#include "RTMClient.h"
#include "./RTMUnitis/RTMMsgFilter.h"
#include "./RTMUnitis/RTMMsg.h"

using namespace fpnn;

class RTMGateQuestProcessor;
typedef std::shared_ptr<RTMGateQuestProcessor> RTMGateQuestProcessorPtr;

class RTMGateQuestProcessor :public IQuestProcessor
{
	QuestProcessorClassPrivateFields(RTMGateQuestProcessor);
	shared_ptr<RTCEventHandler> rtcEventHandler;
	shared_ptr<RTMEventHandler> rtmEventHandler;

public:
	RTMGateQuestProcessor(shared_ptr<RTMEventHandler> rtm);
	~RTMGateQuestProcessor();

	void SetRTCEventHandler(shared_ptr<RTCEventHandler> rtchandler);
	void SetClientPtr(RTMClientPtr clientPtr, RTMClient::RTMType rType);

	/*********************by fpnn server called interface**********************/
	FPAnswerPtr pushEnterRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushExitRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushRTCRoomClosed(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushInviteIntoRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushKickOutRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushPullIntoRTCRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushAdminCommand(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushRoomEvent(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	
	FPAnswerPtr kickout(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr kickoutRoom(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushMsg(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushGroupMsg(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushRoomMsg(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushBroadcastMsg(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	
	FPAnswerPtr pushP2PRTCRequest(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
	FPAnswerPtr pushP2PRTCEvent(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);

	FPAnswerPtr ping(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);

	/*********************by fpnn client called interface**********************/
	bool isConnectionAlive();
	void connected(const ConnectionInfo&, bool connected);
	void connectionWillClose(const ConnectionInfo& connInfo, bool closeError);
	void updatePingTime();

	/*********************by rtm client called interface**********************/
	void connnectionClosed(int32_t closeError);
	void reloginCompleted(bool successful, bool retryAgain, int errorCode, int retriedCount);
	bool reloginWillStart(int lastErrorCode, int retriedCount);

	QuestProcessorClassBasicPublicFuncs

public:
	void parseFileMsg(RTMMsg& rtmMsg);
	void parseFileAttrsMsg(RTMMsg& rtmMsg);
	RTMTranslatedMsg parseChatMsg(const FPReaderPtr args);
	RTMMsg parseRTMsg(const FPReaderPtr args, long from, long to, long mid);

private:
	atomic<int64_t> lastPingTime;
	RTMClient::RTMType rtmType;
	RTMClientWeakPtr rtmClient;
	RTMMsgFilter rtmMsgFilter;
};

#endif // RTMGATEQUESTPROCESSOR_H

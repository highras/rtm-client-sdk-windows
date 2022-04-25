#pragma once
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <iostream>

#include "RTCEventHandler.h"
#include "RTMProxy.h"

class RTCClient;
class RTMClient;
class AudioPlayer;
class AudioRecorder;
class OpenH264Decoder;
class D3D12Renderer;

struct HWND__;

using namespace std;

class RTCProxy: public RTMProxy
{
	shared_ptr<RTCClient> rtc;

	shared_ptr<AudioPlayer> player;
	shared_ptr<AudioRecorder> recorder;

	int64_t audioSeq = 0;
	int64_t currentRid = 0;

	bool muted = false;
	mutex drmutex;

	atomic<int8_t> p2pStatus = 0;// 0 not using 1 calling 2 communicating 3 on calling
	atomic<int64_t> p2pCallId = 0;
	atomic<bool> busy = false;
	struct UserData
	{
		OpenH264Decoder* decoder;
		D3D12Renderer* renderer;
		bool isReady;
		list<pair<int64_t, vector<unsigned char>>> frameQueue;
		void* hTimer;
		mutex dataMutex;
		int32_t captureLevel;
	};
	unordered_map<int64_t, UserData*> userMaps;
	UserData* p2pUserData;

	class InternalEventHandler : public RTCEventHandler
	{
		shared_ptr<RTCEventHandler> userEventHandler;
		friend class RTCProxy;
		RTCProxy* rtcProxy;
	public:
		InternalEventHandler(shared_ptr<RTCEventHandler> handler, RTCProxy* proxy);
		virtual ~InternalEventHandler();
		void OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime) override;
		void OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime) override;
		void OnRTCRoomClosed(int64_t rid) override;
		void OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid) override;
		void OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid) override;
		void OnPullIntoRTCRoom(int64_t rid, string token) override;
		void OnAdminCommand(AdminCommand command, vector<int64_t> uids) override;
		void OnRoomEvent(int64_t rid, RoomEvent roomEvent, vector<unsigned char>eventData) override;

		void OnPushP2PRTCRequest(int64_t callId, int64_t peerUid, int32_t type) override;
		void OnPushP2PRTCEvent(int64_t callId, int64_t peerUid, int32_t type, int32_t p2pEvent) override;
	};

	static void TimerProc(void* lpParameter, unsigned char TimerOrWaitFired);
public:

	struct RTCRoomMembers
	{
		vector<int64_t> uids;
		vector<int64_t> administrators;
		int64_t owner;
	};

	RTCProxy(string rtmhost, unsigned short rtmport, int64_t pid, int64_t uid, shared_ptr<RTMEventHandler> rtmhandler, string rtchost, unsigned short rtcport, shared_ptr<RTCEventHandler> rtchandler);
	virtual ~RTCProxy();

	void CreateRTCRoom(int32_t type, int64_t rid, int32_t enableRecord, function<void(int errorCode, bool microphone)> callback);
	void EnterRTCRoom(int64_t rid, function<void(int errorCode, bool microphone)> callback);
	void ExitRTCRoom(function<void(int errorCode)> callback);
	void SubscribeVideo(int64_t rid, int64_t uid, HWND__* hwnd, uint32_t width, uint32_t height, function<void(int errorCode)> callback);
	void UnsubscribeVideo(int64_t rid, int64_t uid, function<void(int errorCode)> callback);

	void InviteUserIntoRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void GetRTCRoomMembers(int64_t rid, function<void(int errorCode, RTCRoomMembers roomMembers)> callback);
	void GetRTCRoomMemberCount(int64_t rid, function<void(int errorCode, int64_t count)> callback);
	void BlockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void UnblockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void AdminCommand(int64_t rid, unordered_set<int64_t> uids, RTCEventHandler::AdminCommand command, function<void(int errorCode)> callback);
	void RoomEvent(int64_t rid, vector<unsigned char> revent, function<void(int errorCode)> callback);

	void RequestP2PRTC(int32_t type, int64_t peerUid, HWND__* hwnd, uint32_t width, uint32_t height, function<void(int errorCode)> callback);
	void CancelP2PRTC(int64_t callId, function<void(int errorCode)> callback);
	void CloseP2PRTC(int64_t callId, function<void(int errorCode)> callback);
	void AcceptP2PRTC(int64_t callId, HWND__* hwnd, uint32_t width, uint32_t height, function<void(int errorCode)> callback);
	void RefuseP2PRTC(int64_t callId, function<void(int errorCode)> callback);

	void Mute();
	void Unmute();
	void StartAudio();
	void StopAudio();
};


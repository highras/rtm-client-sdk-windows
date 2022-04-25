#pragma once
#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

class RTCEventHandler
{
public:
	enum class AdminCommand : int32_t 
	{
		PromoteAdmin,
		DepriveAdmin,
		ForbidSendAudio,
		AllowSendAudio,
		ForbidSendVideo,
		AllowSendVideo,
		CloseUserMicrophone,
		CloseUserCamera
	};
	enum class RoomEvent : int32_t {};
	virtual void OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime) {}
	virtual void OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime) {}
	virtual void OnRTCRoomClosed(int64_t rid) {}
	virtual void OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid) {}
	virtual void OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid) {}
	virtual void OnPullIntoRTCRoom(int64_t rid, string token) {}
	virtual void OnAdminCommand(AdminCommand command, vector<int64_t> uids) {}
	virtual void OnRoomEvent(int64_t rid, RoomEvent roomEvent, vector<unsigned char>eventData) {}

	virtual void OnPushP2PRTCRequest(int64_t callId, int64_t peerUid, int32_t type) {}
	virtual void OnPushP2PRTCEvent(int64_t callId, int64_t peerUid, int32_t type, int32_t p2pEvent) {}
};
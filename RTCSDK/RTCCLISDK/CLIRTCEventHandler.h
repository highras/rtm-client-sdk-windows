#pragma once
#include <RTCEventHandler.h>
#include <msclr/gcroot.h>

namespace RTCCLISDK
{
	public ref class CLIRTCEventHandler
	{
	public:
		CLIRTCEventHandler();
		~CLIRTCEventHandler();

		virtual void OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime);
		virtual void OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime);
		virtual void OnRTCRoomClosed(int64_t rid);
		virtual void OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid);
		virtual void OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid);
		virtual void OnPullIntoRTCRoom(int64_t rid, string token);
		virtual void OnAdminCommand(int32_t command, vector<int64_t> uids);
		virtual void OnRoomEvent(int64_t rid, int32_t roomEvent, vector<unsigned char>eventData);
	};


	class BridgeRTCEventHandler :public RTCEventHandler
	{
		msclr::gcroot<CLIRTCEventHandler^> implement;
	public:
		BridgeRTCEventHandler(msclr::gcroot<CLIRTCEventHandler^> impl);
		~BridgeRTCEventHandler();

		virtual void OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime) override;
		virtual void OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime) override;
		virtual void OnRTCRoomClosed(int64_t rid) override;
		virtual void OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid) override;
		virtual void OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid) override;
		virtual void OnPullIntoRTCRoom(int64_t rid, string token) override;
		virtual void OnAdminCommand(AdminCommand command, vector<int64_t> uids) override;
		virtual void OnRoomEvent(int64_t rid, RoomEvent roomEvent, vector<unsigned char>eventData) override;
	};
}

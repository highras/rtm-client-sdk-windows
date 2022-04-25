#include "CLIRTCEventHandler.h"

namespace RTCCLISDK
{
	BridgeRTCEventHandler::BridgeRTCEventHandler(msclr::gcroot<CLIRTCEventHandler^> impl)
	{
		implement = impl;
	}

	BridgeRTCEventHandler::~BridgeRTCEventHandler()
	{
	}

	void BridgeRTCEventHandler::OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime)
	{
		implement->OnUserEnterRTCRoom(uid, rid, mtime);
	}

	void BridgeRTCEventHandler::OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime)
	{
		implement->OnUserExitRTCRoom(uid, rid, mtime);
	}

	void BridgeRTCEventHandler::OnRTCRoomClosed(int64_t rid)
	{
		implement->OnRTCRoomClosed(rid);
	}

	void BridgeRTCEventHandler::OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid)
	{
		implement->OnInviteIntoRTCRoom(fromUid, rid);
	}

	void BridgeRTCEventHandler::OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid)
	{
		implement->OnKickOutFromRTCRoom(fromUid, rid);
	}

	void BridgeRTCEventHandler::OnPullIntoRTCRoom(int64_t rid, string token)
	{
		implement->OnPullIntoRTCRoom(rid, token);
	}

	void BridgeRTCEventHandler::OnAdminCommand(AdminCommand command, vector<int64_t> uids)
	{
		implement->OnAdminCommand((int32_t)command, uids);
	}

	void BridgeRTCEventHandler::OnRoomEvent(int64_t rid, RoomEvent roomEvent, vector<unsigned char> eventData)
	{
		implement->OnRoomEvent(rid, (int32_t)roomEvent, eventData);
	}

	CLIRTCEventHandler::CLIRTCEventHandler()
	{
		throw gcnew System::NotImplementedException();
	}

	CLIRTCEventHandler::~CLIRTCEventHandler()
	{
		throw gcnew System::NotImplementedException();
	}

	void CLIRTCEventHandler::OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime)
	{
		throw gcnew System::NotImplementedException();
	}

	void CLIRTCEventHandler::OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime)
	{
		throw gcnew System::NotImplementedException();
	}

	void CLIRTCEventHandler::OnRTCRoomClosed(int64_t rid)
	{
		throw gcnew System::NotImplementedException();
	}

	void CLIRTCEventHandler::OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid)
	{
		throw gcnew System::NotImplementedException();
	}

	void CLIRTCEventHandler::OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid)
	{
		throw gcnew System::NotImplementedException();
	}

	void CLIRTCEventHandler::OnPullIntoRTCRoom(int64_t rid, string token)
	{
		throw gcnew System::NotImplementedException();
	}

	void CLIRTCEventHandler::OnAdminCommand(int32_t command, vector<int64_t> uids)
	{
		throw gcnew System::NotImplementedException();
	}

	void CLIRTCEventHandler::OnRoomEvent(int64_t rid, int32_t roomEvent, vector<unsigned char> eventData)
	{
		throw gcnew System::NotImplementedException();
	}
}
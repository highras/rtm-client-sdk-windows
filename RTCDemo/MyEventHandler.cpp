#include "MyEventHandler.h"

#include <QDebug>

void MyEventHandler::OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime)
{
	qDebug() << "User " << uid << " has entered the room " << rid << " at " << mtime;
}

void MyEventHandler::OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime)
{
	qDebug() << "User " << uid << " has exited the room " << rid << " at " << mtime;
}

void MyEventHandler::OnRTCRoomClosed(int64_t rid)
{
	qDebug() << "Room " << rid << " has been closed.";
}

void MyEventHandler::OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid)
{

}

void MyEventHandler::OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid)
{

}

void MyEventHandler::OnPullIntoRTCRoom(int64_t rid, string token)
{

}

void MyEventHandler::OnAdminCommand(AdminCommand command, vector<int64_t> uids)
{

}

void MyEventHandler::OnRoomEvent(int64_t rid, RoomEvent roomEvent, vector<unsigned char> eventData)
{

}

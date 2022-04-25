#pragma once
#include <RTCEventHandler.h>

class MyEventHandler : public RTCEventHandler
{
public:
    virtual void OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime) override;
    virtual void OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime) override;
    virtual void OnRTCRoomClosed(int64_t rid) override;
    virtual void OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid) override;
    virtual void OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid) override;
    virtual void OnPullIntoRTCRoom(int64_t rid, string token) override;
    virtual void OnAdminCommand(AdminCommand command, vector<int64_t> uids) override;
    virtual void OnRoomEvent(int64_t rid, RoomEvent roomEvent, vector<unsigned char>eventData) override;
};

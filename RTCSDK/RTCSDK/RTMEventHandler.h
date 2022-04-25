#pragma once
#include <string>
#include <stdint.h>

using namespace std;

class RTMEventHandler
{
public:
	virtual void OnKickout() {}
	virtual void OnKickoutRoom(int64_t rid) {}

	virtual void OnPushChat(int64_t from, int64_t to, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushCmd(int64_t from, int64_t to, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushMsg(int64_t from, int64_t to, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushFile(int64_t from, int64_t to, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) {}

	virtual void OnPushGroupChat(int64_t from, int64_t gid, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushGroupCmd(int64_t from, int64_t gid, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushGroupFile(int64_t from, int64_t gid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushGroupMsg(int64_t from, int64_t gid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) {}

	virtual void OnPushRoomChat(int64_t from, int64_t rid, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushRoomCmd(int64_t from, int64_t rid, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushRoomFile(int64_t from, int64_t rid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushRoomMsg(int64_t from, int64_t rid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) {}

	virtual void OnPushBroadcastChat(int64_t from, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushBroadcastCmd(int64_t from, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushBroadcastFile(int64_t from, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) {}
	virtual void OnPushBroadcastMsg(int64_t from, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) {}

	virtual void connnectionClosed(int32_t closeError) {}
	virtual void reloginCompleted(bool successful, bool retryAgain, int errorCode, int retriedCount) {}
	virtual bool reloginWillStart(int lastErrorCode, int retriedCount) { return true; }
};


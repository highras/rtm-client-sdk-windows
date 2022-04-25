#pragma once
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <unordered_set>


#include "RTMEventHandler.h"
#include "./RTMUnitis/RTMAudio.h"

using namespace std;

class RTMClient;

struct CommonMsg
{
	int64_t id;
	int64_t from;//发送者uid
	int8_t mtype;
	int64_t mid;
	bool deleted;//即将去掉
	string msg;
	string attrs;
	int64_t mtime;
};

struct P2PMsg
{
	int64_t id;
	int8_t direction;//发送者uid
	int8_t mtype;
	int64_t mid;
	bool deleted;//即将去掉
	string msg;
	string attrs;
	int64_t mtime;
};

class RTMProxy
{
protected:
	shared_ptr<RTMClient> rtm = nullptr;

	int64_t currentUid = 0;
	int64_t currentPid = 0;
public:
	RTMProxy(string rtmhost, unsigned short rtmport, int64_t pid, int64_t uid, shared_ptr<RTMEventHandler> rtmhandler);
	virtual ~RTMProxy();

	int32_t Login(string token, int64_t& pid, int64_t& uid, bool& successed, std::unordered_map<string, string> attr = std::unordered_map<string, string>(), string lang = "", int timeout = 0);
	int32_t Bye();
	int32_t AddAttrs(unordered_map<string, string> attrs);
	int32_t GetAttrs(unordered_map<string, string>& attrs);
	int32_t AddDebugLog(string msg, string attrs);
	int32_t GetServerTime(int64_t& mts);

	int32_t SendChat(int64_t to, int64_t mid, string msg, string attrs, int64_t& mtime);
	int32_t SendCmd(int64_t to, int64_t mid, string msg, string attrs, int64_t& mtime);
	int32_t SendGroupChat(int64_t gid, int64_t mid, string msg, string attrs, int64_t& mtime);
	int32_t SendGroupCmd(int64_t gid, int64_t mid, string msg, string attrs, int64_t& mtime);
	int32_t SendRoomChat(int64_t rid, int64_t mid, string msg, string attrs, int64_t& mtime);
	int32_t SendRoomCmd(int64_t rid, int64_t mid, string msg, string attrs, int64_t& mtime);
	int32_t GetGroupChat(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);
	int32_t GetRoomChat(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);
	int32_t GetBroadcastChat(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid,int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);
	int32_t GetP2PChat(int64_t ouid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);
	int32_t DelChat(int64_t from, int64_t mid, int64_t xid, int8_t type);
	int32_t GetChat(int64_t from, int64_t mid, int64_t xid, int8_t type, int64_t& id, int8_t& mtype, string& msg, string& attrs, int64_t& mtime);

	int32_t SendMsg(int64_t to, int64_t mid, int8_t mtype, string msg, string attrs, int64_t& mtime);
	int32_t SendGroupMsg(int64_t gid, int64_t mid, int8_t mtype, string msg, string attrs, int64_t& mtime);
	int32_t SendRoomMsg(int64_t rid, int64_t mid, int8_t mtype, string msg, string attrs, int64_t& mtime);
	int32_t GetGroupMsg(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);
	int32_t GetRoomMsg(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);
	int32_t GetBroadcastMsg(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);
	int32_t GetP2PMsg(int64_t ouid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);
	int32_t DelMsg(int64_t from, int64_t mid, int64_t xid, int8_t type);
	int32_t GetMsg(int64_t from, int64_t mid, int64_t xid, int8_t type, int64_t& id, int8_t& mtype, string& msg, string& attrs, int64_t& mtime);
	int32_t GetUnread(bool clean, unordered_set<int64_t>& p2p, unordered_set<int64_t>& group);
	int32_t GetSession(unordered_set<int64_t>& p2p, unordered_set<int64_t>& group);
	int32_t GetP2PUnread(unordered_set<int64_t> uids, int64_t mtime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& p2p, unordered_map<string, int64_t>& ltime);
	int32_t GetGroupUnread(unordered_set<int64_t> gids, int64_t mtime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& group, unordered_map<string, int64_t>& ltime);
	int32_t CleanTargetUnread(unordered_set<int64_t> uids, unordered_set<int64_t> gids);
	int32_t GetP2PUnreadByTime(unordered_map<string, int64_t> uidTime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& p2p, unordered_map<string, int64_t>& ltime);
	int32_t GetGroupUnreadByTime(unordered_map<string, int64_t> gidTime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& group, unordered_map<string, int64_t>& ltime);

	int32_t SetLang(string lang);
	int32_t Translate(string text, string src, string dst, string type, string profanity, string& source, string& target, string& sourceText, string& targetText);
	int32_t Profanity(string text, bool classify, string& otext, vector<string>& classification);
	int32_t Speech2Text(string audio, int32_t type, string lang, string codec, int32_t srate, string& text, string& olang);
	int32_t TCheck(string text, int32_t& result, string& otext, vector<string>& tags, vector<string>& wlist);
	int32_t ICheck(string image, int32_t type, int32_t& result, vector<string>& tags);
	int32_t ACheck(string audio, int32_t type, string lang, string codec, int32_t srate, int32_t& result, vector<string>& tags);
	int32_t VCheck(string video, int32_t type, string videoName, int32_t& result, vector<string>& tags);

	int32_t FileToken(int64_t xid, int16_t sendType, string& token, string& endpoint);
	int32_t SendFile(int32_t pid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int64_t& mtime, int timeout = 120);
	int32_t SendGroupFile(int32_t gid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int64_t& mtime, int timeout = 120);
	int32_t SendRoomFile(int32_t rid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int64_t& mtime, int timeout = 120);
	int32_t SendFile(int32_t pid, RTMAudio rtmAudio, int64_t& mtime, int timeout = 120);
	int32_t SendGroupFile(int32_t gid, RTMAudio rtmAudio, int64_t& mtime, int timeout = 120);
	int32_t SendRoomFile(int32_t rid, RTMAudio rtmAudio, int64_t& mtime, int timeout = 120);

	int32_t GetOnlineUsers(unordered_set<int64_t> uids, unordered_set<int64_t>& ouids);
	int32_t SetUserInfo(string oinfo, string pinfo);
	int32_t GetUserInfo(string& oinfo, string& pinfo);
	int32_t GetUserOpenInfo(unordered_set<int64_t> uids, unordered_map<string, string>& info);

	int32_t AddFriends(unordered_set<int64_t> friends);
	int32_t DelFriends(unordered_set<int64_t> friends);
	int32_t GetFriends(unordered_set<int64_t>& uids);
	int32_t AddBlacks(unordered_set<int64_t> blacks);
	int32_t DelBlacks(unordered_set<int64_t> blacks);
	int32_t GetBlacks(unordered_set<int64_t>& uids);

	int32_t AddGroupMembers(int64_t gid, unordered_set<int64_t> uids);
	int32_t DelGroupMembers(int64_t gid, unordered_set<int64_t> uids);
	int32_t GetGroupMembers(int64_t gid, bool online, unordered_set<int64_t>& uids, unordered_set<int64_t>& onlines);
	int32_t GetGroupCount(int64_t gid, bool online, int32_t& cn, int32_t& onlines);
	int32_t GetUserGroups(unordered_set<int64_t>& gids);
	int32_t SetGroupInfo(int64_t gid, string oinfo, string pinfo);
	int32_t GetGroupOpenInfo(int64_t gid, string& oinfo);
	int32_t GetGroupsOpenInfo(unordered_set<int64_t> gids, unordered_map<string, string>& info);

	int32_t EnterRoom(int64_t rid);
	int32_t LeaveRoom(int64_t rid);
	int32_t GetUserRooms(unordered_set<int64_t>& rooms);
	int32_t SetRoomInfo(int64_t rid, string oinfo, string pinfo);
	int32_t GetRoomInfo(int64_t rid, string& oinfo, string& pinfo);
	int32_t GetRoomOpenInfo(int64_t rid, string& oinfo);
	int32_t GetRoomsOpenInfo(unordered_set<int64_t> rids, unordered_map<string, string>& info);
	int32_t GetRoomMemers(int64_t rid, unordered_set<int64_t>& uids);
	int32_t GetRoomCount(unordered_set<int64_t> rids, unordered_map<string, int32_t>& info);

	int32_t DataGet(string key, string& val);
	int32_t DataSet(string key, string val);
	int32_t DataDel(string key);

	// async call
	virtual void Login(function<void(int64_t pid, int64_t uid, int errorCode, bool successed)> callback, string token, std::unordered_map<string, string> attr = std::unordered_map<string, string>(), string lang = "", int timeout = 0);
	void Bye(function<void(int errorCode)> callback);
	void AddAttrs(unordered_map<string, string> attrs, function<void(int errorCode)> callback);
	void GetAttrs(function<void(int errorCode, unordered_map<string, string> attrs)> callback);
	void AddDebugLog(string msg, string attrs, function<void(int errorCode)> callback);
	void GetServerTime(function<void(int errorCode, int64_t mts)> callback);

	void SendChat(int64_t to, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);
	void SendCmd(int64_t to, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);
	void SendGroupChat(int64_t gid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);
	void SendGroupCmd(int64_t gid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);
	void SendRoomChat(int64_t rid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);
	void SendRoomCmd(int64_t rid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);
	void GetGroupChat(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);
	void GetRoomChat(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);
	void GetBroadcastChat(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);
	void GetP2PChat(int64_t ouid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);
	void DelChat(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode)> callback);
	void GetChat(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime)> callback);

	void SendMsg(int64_t to, int64_t mid, int8_t mtype, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);
	void SendGroupMsg(int64_t gid, int64_t mid, int8_t mtype, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);
	void SendRoomMsg(int64_t rid, int64_t mid, int8_t mtype, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);
	void GetGroupMsg(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);
	void GetRoomMsg(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);
	void GetBroadcastMsg(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);
	void GetP2PMsg(int64_t ouid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);
	void DelMsg(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode)> callback);
	void GetMsg(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime)> callback);
	
	void GetUnread(bool clean, function<void(int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group)> callback);
	void CleanUnread(function<void(int errorCode)> callback);
	void GetSession(function<void(int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group)> callback);
	void GetP2PUnread(unordered_set<int64_t> uids, int64_t mtime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime)> callback);
	void GetGroupUnread(unordered_set<int64_t> gids, int64_t mtime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime)> callback);
	void CleanTargetUnread(unordered_set<int64_t> uids, unordered_set<int64_t> gids, function<void(int errorCode)> callback);
	void GetP2PUnreadByTime(unordered_map<string, int64_t> uidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime)> callback);
	void GetGroupUnreadByTime(unordered_map<string, int64_t> gidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime)> callback);

	void SetLang(string lang, function<void(int errorCode)> callback);
	void Translate(string text, string src, string dst, string type, string profanity, function<void(int errorCode, string source, string target, string sourceText, string targetText)> callback);
	void Profanity(string text, bool classify, function<void(int errorCode, string text, vector<string> classification)> callback);
	void Speech2Text(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, string text, string lang)> callback);
	void TCheck(string text, function<void(int errorCode, int32_t result, string text, vector<string> tags, vector<string> wlist)> callback);
	void ICheck(string image, int32_t type, function<void(int errorCode, int32_t result, vector<string> tags)> callback);
	void ACheck(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, int32_t result, vector<string> tags)> callback);
	void VCheck(string video, int32_t type, string videoName, function<void(int errorCode, int32_t result, vector<string> tags)> callback);

	void FileToken(int64_t xid, int16_t sendType, function<void(int errorCode, string token, string endpoint)> callback);
	void SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout = 120);
	void SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout = 120);
	void SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout = 120);
	void SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, RTMAudio rtmAudio, int timeout = 120);
	void SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, RTMAudio rtmAudio, int timeout = 120);
	void SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, RTMAudio rtmAudio, int timeout = 120);

	void GetOnlineUsers(unordered_set<int64_t> uids, function<void(int errorCode, unordered_set<int64_t> uids)> callback);
	void SetUserInfo(string oinfo, string pinfo, function<void(int errorCode)> callback);
	void GetUserInfo(function<void(int errorCode, string oinfo, string pinfo)> callback);
	void GetUserOpenInfo(unordered_set<int64_t> uids, function<void(int errorCode, unordered_map<string, string> info)> callback);

	void AddFriends(unordered_set<int64_t> friends, function<void(int errorCode)> callback);
	void DelFriends(unordered_set<int64_t> friends, function<void(int errorCode)> callback);
	void GetFriends(function<void(int errorCode, unordered_set<int64_t> uids)> callback);
	void AddBlacks(unordered_set<int64_t> blacks, function<void(int errorCode)> callback);
	void DelBlacks(unordered_set<int64_t> blacks, function<void(int errorCode)> callback);
	void GetBlacks(function<void(int errorCode, unordered_set<int64_t> uids)> callback);

	void AddGroupMembers(int64_t gid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void DelGroupMembers(int64_t gid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void GetGroupMembers(int64_t gid, bool online, function<void(int errorCode, unordered_set<int64_t> uids, unordered_set<int64_t> onlines)> callback);
	void GetGroupCount(int64_t gid, bool online, function<void(int errorCode, int32_t cn, int32_t onlines)> callback);
	void GetUserGroups(function<void(int errorCode, unordered_set<int64_t> gids)> callback);
	void SetGroupInfo(int64_t gid, string oinfo, string pinfo, function<void(int errorCode)> callback);
	void GetGroupOpenInfo(int64_t gid, function<void(int errorCode, string oinfo)> callback);
	void GetGroupsOpenInfo(unordered_set<int64_t> gids, function<void(int errorCode, unordered_map<string, string> info)> callback);

	void EnterRoom(int64_t rid, function<void(int errorCode)> callback);
	void LeaveRoom(int64_t rid, function<void(int errorCode)> callback);
	void GetUserRooms(function<void(int errorCode, unordered_set<int64_t> rooms)> callback);
	void SetRoomInfo(int64_t rid, string oinfo, string pinfo, function<void(int errorCode)> callback);
	void GetRoomInfo(int64_t rid, function<void(int errorCode, string oinfo, string pinfo)> callback);
	void GetRoomOpenInfo(int64_t rid, function<void(int errorCode, string oinfo)> callback);
	void GetRoomsOpenInfo(unordered_set<int64_t> rids, function<void(int errorCode, unordered_map<string, string> info)> callback);
	void GetRoomMemers(int64_t rid, function<void(int errorCode, unordered_set<int64_t> uids)> callback);
	void GetRoomCount(unordered_set<int64_t> rids, function<void(int errorCode, unordered_map<string, int32_t> info)> callback);

	void DataGet(string key, function<void(int errorCode, string val)> callback);
	void DataSet(string key, string val, function<void(int errorCode)> callback);
	void DataDel(string key, function<void(int errorCode)> callback);

	bool StartRecordAudio();
	void StopRecordAudio(RTMAudio& recordata);
	bool StartPlayAudio(RTMAudio recordata);
	bool StopPlayAudio();
};


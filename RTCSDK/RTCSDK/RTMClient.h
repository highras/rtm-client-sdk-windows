#pragma once

#include <TCPClient.h>
#include <UDPClient.h>
#include <unordered_set>
#include <future>
#include <thread>

#include "IQuestProcessor.h"
#include "RTCEventHandler.h"
#include "RTMEventHandler.h"
#include "./RTMUnitis/RTMRelogin.h"
#include "./RTMUnitis/RTMError.h"
#include "./RTMUnitis/RTMFile.h"
#include "./RTMUnitis/RTMAudio.h"
#include "./RTMAudio/AmrwbRecorder.h"
#include "./RTMAudio/AmrwbPlayer.h"

using namespace fpnn;
using namespace std;

class RTMClient;
typedef std::shared_ptr<RTMClient> RTMClientPtr; 
typedef std::weak_ptr<RTMClient> RTMClientWeakPtr;
class RTMClient : public std::enable_shared_from_this<RTMClient>
{
public:
	enum class ConnStatus
	{
		Closed,
		Connecting,
		Connected,
	};

	enum class RTMType
	{
		RT_RTM = 0,
		RT_FILE,
	};

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
		MSGPACK_DEFINE(id, from, mtype, mid, deleted, msg, attrs, mtime);
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
		MSGPACK_DEFINE(id, direction, mtype, mid, deleted, msg, attrs, mtime);
	};

	RTMClient(string host, unsigned short port, int64_t pid, int64_t uid);
    RTMClient(string host, unsigned short port, int64_t pid, int64_t uid, shared_ptr<RTMEventHandler> rtmhandler, bool isAutoReloginValid = true);
	virtual ~RTMClient();

	struct RTCRoomMembers
	{
		vector<int64_t> uids;
		vector<int64_t> administrators;
		int64_t owner;
	};

	void SetRTCEventHandler(shared_ptr<RTCEventHandler> rtchandler);

	// sync call
	string EnterRTCRoom(int64_t rid);
	string CreateRTCRoom(int32_t type,int64_t rid,int32_t enableRecord = 0);
	bool ExitRTCRoom(int64_t rid);
	void SubscribeVideo(int64_t rid, unordered_set<int64_t> uids);
	void UnsubscribeVideo(int64_t rid, unordered_set<int64_t> uids);
	void InviteUserIntoRTCRoom(int64_t rid, unordered_set<int64_t> uids);
	RTCRoomMembers GetRTCRoomMembers(int64_t rid);
	int32_t GetRTCRoomMemberCount(int64_t rid);
	void BlockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids);
	void UnblockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids);
	void AdminCommand(int64_t rid, unordered_set<int64_t> uids, RTCEventHandler::AdminCommand command);
	void RoomEvent(int64_t rid, vector<unsigned char> revent);

	// async call
	void EnterRTCRoom(int64_t rid, function<void(int errorCode, string token)> callback);
	void CreateRTCRoom(int32_t type, int64_t rid, int32_t enableRecord, function<void(int errorCode, string token)> callback);
	void ExitRTCRoom(int64_t rid, function<void(int errorCode)> callback);
	void SubscribeVideo(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void UnsubscribeVideo(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void InviteUserIntoRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void GetRTCRoomMembers(int64_t rid, function<void(int errorCode, RTCRoomMembers roomMembers)> callback);
	void GetRTCRoomMemberCount(int64_t rid, function<void(int errorCode, int32_t count)> callback);
	void BlockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void UnblockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);
	void AdminCommand(int64_t rid, unordered_set<int64_t> uids, RTCEventHandler::AdminCommand command, function<void(int errorCode)> callback);
	void RoomEvent(int64_t rid, vector<unsigned char> revent, function<void(int errorCode)> callback);

	void RequestP2PRTC(int32_t type, int64_t peerUid, function<void(int errorCode, int64_t callId)> callback);
	void CancelP2PRTC(int64_t callId, function<void(int errorCode)> callback);
	void CloseP2PRTC(int64_t callId, function<void(int errorCode)> callback);
	void AcceptP2PRTC(int64_t callId, function<void(int errorCode)> callback);
	void RefuseP2PRTC(int64_t callId, function<void(int errorCode)> callback);

	void Bye(function<void(int errorCode)> callback);
	void AddAttrs(unordered_map<string,string> attrs, function<void(int errorCode)> callback);
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
	// imlib
	void CleanTargetUnread(unordered_set<int64_t> uids, unordered_set<int64_t> gids, function<void(int errorCode)> callback);
	// imlib
	void GetP2PUnreadByTime(unordered_map<string,int64_t> uidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime)> callback);
	// imlib
	void GetGroupUnreadByTime(unordered_map<string, int64_t> gidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime)> callback);

	void SetLang(string lang, function<void(int errorCode)> callback);
	void Translate(string text, string src, string dst, string type, string profanity, function<void(int errorCode, string source, string target, string sourceText, string targetText)> callback);
	void Profanity(string text, bool classify, function<void(int errorCode, string text, vector<string> classification)> callback);
	void Speech2Text(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, string text, string lang)> callback);
	void TCheck(string text, function<void(int errorCode, int32_t result, string text, vector<string> tags, vector<string> wlist)> callback);
	void ICheck(string image, int32_t type, function<void(int errorCode, int32_t result, vector<string> tags)> callback);
	void ACheck(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, int32_t result, vector<string> tags)> callback);
	void VCheck(string video, int32_t type, string videoName, function<void(int errorCode, int32_t result, vector<string> tags)> callback);

	bool FileToken(int64_t xid, int16_t fileType, function<void(int errorCode, string token, string endpoint)> callback);
	void SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout=120);
	void SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout = 120);
	void SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout = 120);
	void SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, RTMAudio rtmAudio, int timeout = 120);
	void SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, RTMAudio rtmAudio, int timeout = 120);
	void SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, RTMAudio rtmAudio, int timeout = 120);

	void GetOnlineUsers(unordered_set<int64_t> uids, function<void(int errorCode, unordered_set<int64_t> uids)> callback);
	void SetUserInfo(string oinfo, string pinfo, function<void(int errorCode)> callback);
	void GetUserInfo(function<void(int errorCode, string oinfo, string pinfo)> callback);
	void GetUserOpenInfo(unordered_set<int64_t> uids, function<void(int errorCode, unordered_map<string,string> info)> callback);

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

	bool Login(function<void(int64_t pid, int64_t uid, int32_t errorCode, bool successed)> callback, string token, std::unordered_map<string, string> attr = std::unordered_map<string, string>(), string lang = "", int timeout = 0);
	int32_t Login(string token, bool& ok, std::unordered_map<string, string> attr = std::unordered_map<string, string>(), string lang = "", int timeout = 0);

	bool StartRecordAudio();
	void StopRecordAudio(RTMAudio& recordata);

	bool StartPlayAudio(RTMAudio recordata);
	bool StopPlayAudio();

public:/*login interface*/
	bool isConnectionAlive();
	void connected(const ConnectionInfo& connInfo, bool connected);
	void connectionWillClose(const ConnectionInfo& connInfo, bool closeByError);
	bool isAuthTimeout();
	void auth(bool isTimeout);
	void authAfter(bool authStatus, int errorCode);
	void relogin();
	void reloginAgain();
	void close(bool waitConnectingCannelled = true);
	void close(bool disableRelogin, bool waitConnectingCannelled);

public:/*file interface*/
	void connectedForFileClient(const ConnectionInfo& connInfo, bool connected);
	void connectionWillCloseForFileClient(const ConnectionInfo& connInfo, bool closeByError);

private:/*file interface*/
	void timeout(int32_t& timeout, int64_t& lastActionTimestamp);
	std::string md5(const string& content);
	std::string convertIp4ToIp6(std::string ipv4);
	std::string buildJsonForFileAttrs(RTMFileInfo rtmFileInfo);
	FPQuestPtr buildQuestForSendFile(RTMFileInfo rtmFileInfo, int64_t& msgId);
	int32_t sendFileWithClient(RTMFileInfo rtmFileInfo, RTMClientPtr fileClient);
	int32_t sendFileWithoutClient(RTMFileInfo rtmFileInfo);
	int32_t sendFileWithToken(RTMFileInfo rtmFileInfo, string token, string endpoint);
	bool sendFile(
		function<void(int errorCode, int64_t mtime)> callback,
		RTMFileType fileType,
		int64_t xid,
		int8_t mtype,
		std::string fileText,
		std::string filename,
		std::string fileExt = "",
		string userAttrs = "",
		std::string rtmAttrs = "",
		int timeout = 120);

public:
	std::mutex connMux;
	atomic<ConnStatus> connStatus;
	HANDLE connEvent;
	uint64_t connId = 0;

	int64_t pid = 0;
	int64_t uid = 0;

	IQuestProcessorPtr rtmProcessor;
	ClientPtr rtmClient;
	RTMAutoReloginInfo rtmAutoReloginInfo;
	RTMAuthStatusInfo rtmAuthStatusInfo;
	RTMFileInfo rtmFileInfo;

	unordered_set<int32_t> reloginStopCodes =
	{
		FPNN_EC_CORE_FORBIDDEN,
		RTM_EC_INVALID_PID_OR_UID,
		RTM_EC_INVALID_PID_OR_SIGN,
		RTM_EC_PERMISSION_DENIED,
		RTM_EC_PROJECT_BLACKUSER,
		RTM_EC_ADMIN_LOGIN,
		RTM_EC_ADMIN_ONLY,
		RTM_EC_INVALID_AUTH_TOEKN,
		RTM_EC_IN_BLACKLIST,
	};

	AmrwbRecorder rtmWaveRecorder;
	AmrwbPlayer rtmWavePlayer;
};

void printQuestJson(std::string method, FPQuestPtr quest);
void printAnswerJson(std::string method, FPAnswerPtr answer);

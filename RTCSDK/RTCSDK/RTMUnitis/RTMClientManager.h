#pragma once

#include <mutex>
#include <atomic>
#include <memory>
#include <unordered_map>
#include "../RTMClient.h"
#include "RTMConfig.h"
#include "RTMNetworkNotify.h"

struct RTMClientKey
{
	int64_t pid = 0;
	int64_t uid = 0;

	RTMClientKey(const int64_t pidt, const int64_t uidt)
		: pid(pidt)
		, uid(uidt)
	{

	}
	bool operator < (const RTMClientKey& ct) const 
	{
		return pid < ct.pid&& uid < ct.uid;
	}
};

class RTMClientManager;
typedef std::shared_ptr<RTMClientManager> RTMClientManagerPtr;
typedef std::lock_guard<std::mutex> Lock;
typedef std::map<RTMClientKey, RTMClientPtr> RTMClientKMap;
typedef std::unordered_map<int64_t, RTMClientPtr> RTMClientMap;
typedef std::unordered_map<std::string, std::unordered_map<RTMClientPtr, uint64_t>> RTMFileClientMap;
typedef std::unordered_map<RTMClientPtr, uint64_t> RTMReloginClientMap;
typedef std::unordered_set<RTMClientPtr> RTMClientSet;

class RTMClientManager
{
public:
	static RTMClientManagerPtr instance();
	virtual ~RTMClientManager();

private:
	RTMClientManager();

public:
	void pushClient(uint64_t connId, RTMClientPtr rtmClient);
	void popClient(uint64_t connId);
	RTMClientSet getActiveClients(RTMReloginClientMap& rtmReloginClients);

	void pushClient(const int64_t& pid, const int64_t& uid, RTMClientPtr rtmClient);
	RTMClientPtr getgetClientByPidAndUid(const int64_t& pid, const int64_t& uid);

	void pushReloginClient(RTMClientPtr rtmClient, uint64_t ms);
	void popReloginClient(RTMClientPtr rtmClient);
	RTMClientSet popReloginClientsLessCurrentTime();
	void setReloginClients(RTMReloginClientMap rtmReloginClients);
	RTMReloginClientMap getReloginClients();

	void pushFileClient(std::string endpoint, RTMClientPtr rtmClient);
	RTMClientPtr getFileClient(std::string endpoint);

private:
	void pollThread();
	void pollReLogin();
	void pollFileClient();

private:
	static atomic<bool> createFlag;
	static RTMClientManagerPtr rtmClientManager;
	static std::mutex createMux;
	std::mutex clientMux;
	std::mutex clientKMux;
	std::mutex fileClientMux;
	std::mutex reloginClientMux;

	RTMClientMap rtmClientMap;
	RTMClientKMap rtmClientKMap;
	RTMReloginClientMap rtmReloginClientMap;
	RTMFileClientMap fileClientMap;

	std::thread hPollThread;
	atomic<bool> terminatePollThread;

	RTMNetworkNotify* pNetwork = nullptr;
};


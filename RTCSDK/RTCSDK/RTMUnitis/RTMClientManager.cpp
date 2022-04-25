#include "../RTMClient.h"
#include "../RTMGateQuestProcessor.h"
#include "RTMClientManager.h"
#include "ClientEngine.h"


atomic<bool> RTMClientManager::createFlag = {false};
RTMClientManagerPtr RTMClientManager::rtmClientManager;
std::mutex RTMClientManager::createMux;
extern RTMConfig rtmConfig;
extern RTMRegressiveStrategy rtmRegressiveStrategy;

void networkStatusNotify(bool status)
{
	if (!status || !RTMClientManager::instance())
		return;

	RTMReloginClientMap reloginClients = RTMClientManager::instance()->getReloginClients();
	RTMClientSet avtiveClients = RTMClientManager::instance()->getActiveClients(reloginClients);
	for (auto ac : avtiveClients)
	{
		ac->close(false);
	}
	RTMClientManager::instance()->setReloginClients(reloginClients);
}

RTMClientManager::RTMClientManager()
{
	pNetwork = new RTMNetworkNotify;
	pNetwork->setNetworkStatusCallback(networkStatusNotify);
	RTMNetworkNotify::initNetwork(pNetwork);
	hPollThread = std::thread(&RTMClientManager::pollThread, this);
}

RTMClientManager::~RTMClientManager()
{
	rtmClientManager.reset();
	terminatePollThread.store(true, std::memory_order_relaxed);
	hPollThread.join();

	RTMClientMap rtmClientMapt;
	{
		Lock lck(clientMux);
		rtmClientMapt = rtmClientMap;
	}

	for (auto rtmClient : rtmClientMapt)
	{
		if (rtmClient.second)
			rtmClient.second->close(true, true);
	}
	RTMNetworkNotify::unInitNetwork();
}

RTMClientManagerPtr RTMClientManager::instance()
{
	if (!createFlag.load(std::memory_order_relaxed))
	{
		Lock lck(createMux);
		if (!createFlag.load(std::memory_order_relaxed))
		{
			rtmClientManager.reset(new RTMClientManager());
			createFlag.store(true, std::memory_order_relaxed);
		}
	}

	return rtmClientManager;
}

void RTMClientManager::pushClient(uint64_t connId, RTMClientPtr rtmClient)
{
	Lock lck(clientMux);
	if (rtmClientMap.find(connId) == rtmClientMap.end())
	{
		rtmClientMap.emplace(connId, rtmClient);
	}
}

void RTMClientManager::popClient(uint64_t connId)
{
	Lock lck(clientMux);
	auto c = rtmClientMap.find(connId);
	if (c != rtmClientMap.end())
		rtmClientMap.erase(connId);
}

RTMClientSet RTMClientManager::getActiveClients(RTMReloginClientMap& rtmReloginClients)
{
	RTMClientSet activeClients;

	{
		uint64_t now = slack_real_msec();
		Lock lck(clientMux);
		for (auto c : rtmClientMap)
		{
			if (rtmReloginClients.find(c.second) == rtmReloginClients.end())
			{
				activeClients.emplace(c.second);
				rtmReloginClients.emplace(c.second, now);
			}
		}
	}

	return activeClients;
}

void RTMClientManager::pushClient(const int64_t& pid, const int64_t& uid, RTMClientPtr rtmClient)
{
	Lock lck(clientKMux);
	rtmClientKMap.emplace(RTMClientKey(rtmClient->pid, rtmClient->uid), rtmClient);
}

RTMClientPtr RTMClientManager::getgetClientByPidAndUid(const int64_t& pid, const int64_t& uid)
{
	RTMClientPtr rtmclient = nullptr;
	{
		Lock lck(clientKMux);
		auto c = rtmClientKMap.find(RTMClientKey(pid, uid));
		if (c != rtmClientKMap.end())
			rtmclient = c->second;
	}

	return rtmclient;
}

void RTMClientManager::pushReloginClient(RTMClientPtr rtmClient, uint64_t ms)
{
	Lock lck(reloginClientMux);
	if (rtmReloginClientMap.find(rtmClient) == rtmReloginClientMap.end())
	{
		rtmReloginClientMap.emplace(rtmClient, ms);
	}
	else
	{
		rtmReloginClientMap[rtmClient] = ms;
	}
}

void RTMClientManager::popReloginClient(RTMClientPtr rtmClient)
{
	Lock lck(reloginClientMux);
	rtmReloginClientMap.erase(rtmClient);
}

RTMClientSet RTMClientManager::popReloginClientsLessCurrentTime()
{
	RTMClientSet clientsLessCurrentTime;

	uint64_t now = slack_real_msec();

	/***********find client less than now********************/
	{
		Lock lck(reloginClientMux);
		for (auto rc : rtmReloginClientMap)
		{
			if (rc.second <= now)
			{
				clientsLessCurrentTime.emplace(rc.first);
			}
		}

		for (auto c : clientsLessCurrentTime)
		{
			rtmReloginClientMap.erase(c);
		}
	}

	return clientsLessCurrentTime;
}

void RTMClientManager::setReloginClients(RTMReloginClientMap rtmReloginClients)
{
	Lock lck(reloginClientMux);
	rtmReloginClientMap = rtmReloginClients;
}

RTMReloginClientMap RTMClientManager::getReloginClients()
{
	RTMReloginClientMap clients;

	{
		Lock lck(reloginClientMux);
		clients = rtmReloginClientMap;
	}

	return clients;
}

void RTMClientManager::pushFileClient(std::string endpoint, RTMClientPtr rtmClient)
{
	Lock lck(fileClientMux);
	auto fileClient = fileClientMap.find(endpoint);
	if (fileClient == fileClientMap.end())
	{
		std::unordered_map<RTMClientPtr, uint64_t> mapSecond;
		mapSecond.emplace(rtmClient, slack_real_sec());
		fileClientMap.emplace(endpoint, mapSecond);
	}
	else
	{
		auto clientSecond = fileClient->second.find(rtmClient);
		if (clientSecond != fileClient->second.end())
			clientSecond->second = slack_real_sec();
		else
			fileClient->second.emplace(rtmClient, slack_real_sec());
	}
}

RTMClientPtr RTMClientManager::getFileClient(std::string endpoint)
{
	Lock lck(fileClientMux);
	if (fileClientMap.size() > 0)
	{
		auto fileClient = fileClientMap.find(endpoint);

		if (fileClient->second.size() > 0)
			return fileClient->second.begin()->first;
	}

	return nullptr;
}

void RTMClientManager::pollReLogin()
{
	if (!pNetwork->getNetworkStatus())
		return;

	//ÖØÐÂµÇÂ¼
	RTMClientSet clientsLessCurrentTime = popReloginClientsLessCurrentTime();
	for (auto c : clientsLessCurrentTime)
	{
		ClientEngine::instance()->runTask([=]()
			{
				if (c)
					c->relogin();
			});
	}
}

void RTMClientManager::pollFileClient()
{
	std::unordered_set<std::string> invalidClients;

	Lock lck(fileClientMux);
	{
		long remainTime = slack_real_sec() - rtmConfig.fileGateClientHoldingSeconds;

		for (auto fileClients : fileClientMap)
		{
			RTMClientSet timeoutClients;
			for (auto c : fileClients.second)
			{
				if (c.second <= remainTime)
					timeoutClients.emplace(c.first);
			}

			for (auto tc: timeoutClients)
			{
				fileClients.second.erase(tc);
			}

			if (fileClients.second.size() == 0)
				invalidClients.emplace(fileClients.first);
		}

		for (auto ic : invalidClients)
			fileClientMap.erase(ic);
	}
}

void RTMClientManager::pollThread()
{
	while (!terminatePollThread.load(std::memory_order_relaxed))
	{
		Sleep(1000);
		RTMClientMap rtmClientMapt;
		{
			Lock lck(clientMux);
			rtmClientMapt = rtmClientMap;
		}

		for (auto rtmClient : rtmClientMapt)
			if (rtmClient.second && !rtmClient.second->isConnectionAlive())
				rtmClient.second->close(false, true);

		pollReLogin();
		pollFileClient();
	}
}









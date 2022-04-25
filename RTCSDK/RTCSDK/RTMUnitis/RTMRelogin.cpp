#include "../RTMClient.h"
#include "RTMRelogin.h"
#include "../RTMGateQuestProcessor.h"
#include "RTMClientManager.h"

extern RTMConfig rtmConfig;
extern RTMRegressiveStrategy rtmRegressiveStrategy;
bool RTMClient::isConnectionAlive()
{
	return dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->isConnectionAlive();
}

void RTMClient::connected(const ConnectionInfo& connInfo, bool connected)
{
    if (connected)
    {
        connId = connInfo.uniqueId();
        if (RTMClientManager::instance()) RTMClientManager::instance()->pushClient(connId, shared_from_this());
        auth(true);
    }
    else
    {
        authAfter(false, FPNN_EC_CORE_INVALID_CONNECTION);
    }
}

void printConnStatus(std::string funName, RTMClient::ConnStatus connStatus)
{
#ifdef _DEBUG
    std::string connStr;
    if (connStatus == RTMClient::ConnStatus::Closed)
        connStr = "Closed";
    else if (connStatus == RTMClient::ConnStatus::Connecting)
        connStr = "Connecting";
    else if (connStatus== RTMClient::ConnStatus::Connected)
        connStr = "Connected";
    std::cout << "printConnStatus---->" << "[" << funName << "] " << connStr << std::endl;
#endif // _DEBUG
}

void printRelogin(std::string funName, RTMAutoReloginInfo relogin)
{
#ifdef _DEBUG
    std::cout << "printRelogin++++>" << "[" << funName << "] " << "isAutoReloginValid:" << relogin.isAutoReloginValid
        << ", disabled:" << relogin.disabled
        << ", canRelogin:" << relogin.canRelogin
        << ", reloginCount:" << relogin.reloginCount << std::endl;
#endif // _DEBUG
}

void RTMClient::connectionWillClose(const ConnectionInfo& connInfo, bool closeError)
{

    bool trigger = false;
    bool isConnecting = false;
    bool startRelogin = false;
 
    {
        std::lock_guard<std::mutex> lock(connMux);
        trigger = connId == connInfo.uniqueId();
        if (trigger)
        {
            if (connStatus.load(std::memory_order_relaxed) == ConnStatus::Connecting)
                isConnecting = true;
            else
            {
                connStatus.store(ConnStatus::Closed, std::memory_order_relaxed);
            }

            printConnStatus("connectionWillClose", connStatus.load(std::memory_order_relaxed));
        }

        /*********relogin status********/
        printRelogin("connectionWillClose", rtmAutoReloginInfo);
        if (rtmAutoReloginInfo.isAutoReloginValid)
        {
            startRelogin = (rtmAutoReloginInfo.disabled == false && rtmAutoReloginInfo.canRelogin);
            rtmAutoReloginInfo.lastErrorCode = (closeError ? FPNN_EC_CORE_CONNECTION_CLOSED : FPNN_EC_OK);
        }
    }

    if (trigger)
    {
        /*********when connection will close, need keep connecting status util connected.********/
        if (isConnecting)
            authAfter(false, FPNN_EC_CORE_INVALID_CONNECTION);
        else
        {
            if (startRelogin)
                relogin();
            else
            {
                dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->connnectionClosed(closeError ? FPNN_EC_CORE_UNKNOWN_ERROR : FPNN_EC_OK);
                if (RTMClientManager::instance()) RTMClientManager::instance()->popClient(connId);
            }
        }
    }
}


bool RTMClient::isAuthTimeout()
{
    /********is login to auth timeout?**********/
    if (!rtmAuthStatusInfo.isAuthStatusInfoValid)
        return false;

    long now = slack_real_sec();
    int passSeconds = (int)(now - rtmAuthStatusInfo.lastActionMsecTimeStamp) / 1000;
    rtmAuthStatusInfo.lastActionMsecTimeStamp = now;
    rtmAuthStatusInfo.remainedTimeout -= passSeconds;
    if (rtmAuthStatusInfo.remainedTimeout <= 0)
    {
        authAfter(false, FPNN_EC_CORE_TIMEOUT);
        return false;
    }

    return true;
}

void RTMClient::auth(bool isTimeout)
{
    int16_t paramCnt = 3;
    if (!rtmAuthStatusInfo.lang.empty())
        ++paramCnt;
    if (rtmAuthStatusInfo.attr.size() > 0)
        ++paramCnt;

    FPQWriter qw(paramCnt, "auth");
    qw.param("pid", pid);
    qw.param("uid", uid);
    qw.param("token", rtmAuthStatusInfo.token);
    if (!rtmAuthStatusInfo.lang.empty())
        qw.param("lang", rtmAuthStatusInfo.lang);
    if (rtmAuthStatusInfo.attr.size() > 0)
        qw.param("attrs", rtmAuthStatusInfo.attr);

    FPQuestPtr quest = qw.take();
    printQuestJson("Auth", quest);
    bool status = rtmClient->sendQuest(quest, [=](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("Auth", answer);
        if (errorCode == FPNN_EC_OK)
        {
            FPAReader ar(answer);
            bool ok = ar.getBool("ok");
            authAfter(ok, FPNN_EC_OK);
        }
        else
        {
            authAfter(false, errorCode);
        }
    });
    if (!status)
        authAfter(false, FPNN_EC_CORE_INVALID_CONNECTION);
}

void RTMClient::authAfter(bool authStatus, int errorCode)
{
    long uidt = 0;
    bool isRelogin = false;
    uint64_t connIdt = 0;
    RTMAuthStatusInfo rtmAuthStatusInfot;

    {
        std::lock_guard<std::mutex> lock(connMux);

        /*******update conn info********/
        if (connStatus.load(std::memory_order_relaxed) != ConnStatus::Connecting)
            return;

        if (authStatus)
            connStatus.store(ConnStatus::Connected, std::memory_order_relaxed);
        else
        {
            connStatus.store(ConnStatus::Closed, std::memory_order_relaxed);
            connIdt = connId;
            connId = 0;
        }
        printConnStatus("authAfter", connStatus.load(std::memory_order_relaxed));
        uidt = uid;

        /*******update login info********/
        rtmAutoReloginInfo.updateLoginInfo(authStatus, rtmAuthStatusInfo);
        rtmAuthStatusInfot = rtmAuthStatusInfo;
        rtmAuthStatusInfo = RTMAuthStatusInfo();
        printRelogin("authAfter", rtmAutoReloginInfo);

        /*******update event for close********/
        SetEvent(connEvent);
    }

    /*******update client manager********/
    if (connIdt != 0)
        if (RTMClientManager::instance()) RTMClientManager::instance()->popClient(connIdt);

    /*******update login cb********/
    if (rtmAuthStatusInfot.isAuthStatusInfoValid)
        for (auto c : rtmAuthStatusInfot.authCallbacks)
            c(pid, uidt, errorCode, authStatus);

    /*******update time******/
    if (authStatus)
        dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->updatePingTime();
}


void RTMClient::relogin()
{
    bool launch = dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->reloginWillStart(rtmAutoReloginInfo.lastErrorCode, rtmAutoReloginInfo.reloginCount);
    if (!launch)
    {
        dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->connnectionClosed(rtmAutoReloginInfo.lastErrorCode);
        if (RTMClientManager::instance()) RTMClientManager::instance()->popClient(connId);
        return;
    }

    bool ret = Login([=](long pid, long uid, bool successful, int errorCode)
    {
        if (successful)
        {
            dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->reloginCompleted(true, false, errorCode, rtmAutoReloginInfo.reloginCount);
            rtmAutoReloginInfo.loginSuccessful();
            return;
        }
        else
        {
            bool connected = false;

            {
                std::lock_guard<std::mutex> lock(connMux);
                if (connStatus.load(std::memory_order_relaxed) == ConnStatus::Connected)
                    connected = true;
            }

            if (connected || errorCode == RTM_EC_DUPLCATED_AUTH)
            {
                dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->reloginCompleted(true, false, FPNN_EC_OK, rtmAutoReloginInfo.reloginCount);
                rtmAutoReloginInfo.loginSuccessful();
                return;
            }

            if (errorCode == FPNN_EC_OK)
            {
                dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->reloginCompleted(false, false, RTM_EC_INVALID_AUTH_TOEKN, rtmAutoReloginInfo.reloginCount);
                rtmAutoReloginInfo.disableLogin();
                dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->connnectionClosed(RTM_EC_INVALID_AUTH_TOEKN);
                if (RTMClientManager::instance()) RTMClientManager::instance()->popClient(connId);
                return;
            }

            bool stopRetry = (reloginStopCodes.find(errorCode) != reloginStopCodes.end());

            dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->reloginCompleted(false, !stopRetry, errorCode, rtmAutoReloginInfo.reloginCount);
            if (stopRetry)
            {
                rtmAutoReloginInfo.disableLogin();
                dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->connnectionClosed(errorCode);
                if (RTMClientManager::instance()) RTMClientManager::instance()->popClient(connId);
                return;
            }
            else
                rtmAutoReloginInfo.lastErrorCode = errorCode;

            reloginAgain();
        }
    },
    rtmAutoReloginInfo.token, rtmAutoReloginInfo.attr, rtmAutoReloginInfo.lang);
    printRelogin("relogin", rtmAutoReloginInfo);
}

void RTMClient::reloginAgain()
{
    if (rtmAutoReloginInfo.reloginCount < rtmRegressiveStrategy.startConnectFailedCount)
    {
        relogin();
        return;
    }

    /*******relogin timeout******/
    int regressiveCount = rtmAutoReloginInfo.reloginCount - rtmRegressiveStrategy.startConnectFailedCount;
    long interval = rtmRegressiveStrategy.maxIntervalSeconds * 1000;
    if (regressiveCount > rtmRegressiveStrategy.maxRegressvieCount)
        return;
    if (regressiveCount < rtmRegressiveStrategy.linearRegressiveCount)
    {
        interval = interval * regressiveCount / rtmRegressiveStrategy.linearRegressiveCount;
    }
    if (RTMClientManager::instance()) RTMClientManager::instance()->pushReloginClient(shared_from_this(), slack_real_msec() + interval);
    printRelogin("reloginAgain", rtmAutoReloginInfo);
}

bool RTMClient::Login(function<void(int64_t pid, int64_t uid, int32_t errorCode, bool successed)> callback, string token, std::unordered_map<string, string> attr, string lang, int timeout)
{
    {
        std::lock_guard<std::mutex> lock(connMux);
        if (connStatus.load(std::memory_order_relaxed) == ConnStatus::Connected)
        {
            ClientEngine::instance()->runTask([=]()
            {
                if (callback)
                    callback(pid, uid, FPNN_EC_OK, true);
            });

            return true;
        }

        /**********save multiple callbacks**********/
        if (connStatus.load(std::memory_order_relaxed) == ConnStatus::Connecting)
        {
            rtmAuthStatusInfo.authCallbacks.push_back(callback);
            return true;
        }
        connStatus.store(ConnStatus::Connecting, std::memory_order_relaxed);
        printConnStatus("Login", connStatus.load(std::memory_order_relaxed));

        /**********reset event for close**********/
        ResetEvent(connEvent);

        if (rtmAutoReloginInfo.isAutoReloginValid)
            rtmAutoReloginInfo.login();
        printRelogin("Login", rtmAutoReloginInfo);
        rtmAuthStatusInfo.updateAuthInfo(callback, attr, token, lang, timeout, slack_real_msec(), true);
    }

    if (rtmAuthStatusInfo.remainedTimeout == 0)
        rtmAuthStatusInfo.remainedTimeout = rtmConfig.globalConnectTimeoutSeconds
        + rtmConfig.globalQuestTimeoutSeconds;

    dynamic_cast<RTMGateQuestProcessor*>(rtmProcessor.get())->SetClientPtr(shared_from_this(), RTMType::RT_RTM);
    if (!rtmClient->asyncConnect())
    {
        if (callback)
            callback(pid, uid, FPNN_EC_CORE_INVALID_CONNECTION, true);
    }
    else
    {
        //when logout already called, if client auto reconnect is true, 
        //user call sendQuest that client will be auto reconnect,
        //then user call login that won't trigger connected, so need 
        bool connStatus = rtmClient->connected();
        if (connStatus)
            auth(true);
    }

    return true;
}

int32_t RTMClient::Login(string token, bool& ok, std::unordered_map<string, string> attr, string lang, int timeout)
{
    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();

    bool successedt = false;
    int32_t errorCodet = 0;

    bool ret = Login([&prom, &successedt, &errorCodet](long pid, long uid, int errorCode, bool successed){
            successedt = successed;
            errorCodet = errorCode;
            prom.set_value(true);
        }, 
        token, attr, lang, timeout);
    if (!ret)
    {
        std::lock_guard<std::mutex> lock(connMux);
        {
            if (connStatus.load(std::memory_order_relaxed) == ConnStatus::Connected)
            {
                successedt = true;
                return FPNN_EC_OK;
            }

            if (connStatus.load(std::memory_order_relaxed) == ConnStatus::Closed)
            {
                successedt = false;
                return FPNN_EC_CORE_INVALID_CONNECTION;
            }
        }
    }

    f.get();

    std::lock_guard<std::mutex> lock(connMux);
    {
        ok = successedt;
        return errorCodet;
    }
}

void RTMClient::Bye(function<void(int errorCode)> callback)
{
    {
        std::lock_guard<std::mutex> lock(connMux);
        if (rtmAutoReloginInfo.isAutoReloginValid)
            rtmAutoReloginInfo.disableLogin();
    }

    bool ret = rtmClient->sendQuest(FPQWriter::emptyQuest("bye"), [=](FPAnswerPtr answer, int errorCode) {
        close();
        callback(errorCode);
        });
    if (!ret)
        close();
}


void RTMClient::close(bool disableRelogin, bool waitConnectingCannelled)
{
    bool isConnecting = false;

    {
        std::lock_guard<std::mutex> lock(connMux);
        if (connStatus.load(std::memory_order_relaxed) == ConnStatus::Closed)
            return;

        if (disableRelogin && rtmAutoReloginInfo.isAutoReloginValid)
            rtmAutoReloginInfo.disableLogin();
        printRelogin("close", rtmAutoReloginInfo);

        /*********when connection close, need keep connecting status util connected.********/
        if (connStatus.load(std::memory_order_relaxed) == ConnStatus::Connecting)
        {
            isConnecting = true;
        }
        else
        {
            connStatus.store(ConnStatus::Closed, std::memory_order_relaxed);
        }
        printConnStatus("close", connStatus.load(std::memory_order_relaxed));
    }

    /*********fpnn client close********/
    rtmClient->close();

    /*
    1¡¢check network thread, not need wait
    2¡¢keep alive thread, need wait
    */
    if (isConnecting && waitConnectingCannelled)
        WaitForSingleObject(connEvent, INFINITE);
}

void RTMClient::close(bool waitConnectingCannelled)
{
	close(true, waitConnectingCannelled);
}

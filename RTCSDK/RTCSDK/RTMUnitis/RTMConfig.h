#pragma once
#include "xstring"

struct RTMConfig
{
    std::string SDKVersion = "2.7.3";
    std::string InterfaceVersion = "2.7.0";

    int lostConnectionAfterLastPingInSeconds = 60;
    int globalConnectTimeoutSeconds = 30;
    int globalQuestTimeoutSeconds = 30;
    int fileGateClientHoldingSeconds = 150;
    bool triggerCallbackIfAsyncMethodReturnFalse = false;

    int maxPingInterval;
    int globalConnectTimeout;
    int globalQuestTimeout;
    int fileClientHoldingSeconds;
    bool forceTriggerCallbackWhenAsyncMethodReturnFalse;

    RTMConfig()
    {
        maxPingInterval = 60;
        globalConnectTimeout = 30;
        globalQuestTimeout = 30;
        fileClientHoldingSeconds = 150;
        forceTriggerCallbackWhenAsyncMethodReturnFalse = false;
    }

    void Config(RTMConfig config)
    {
        lostConnectionAfterLastPingInSeconds = config.maxPingInterval;
        globalConnectTimeoutSeconds = config.globalConnectTimeout;
        globalQuestTimeoutSeconds = config.globalQuestTimeout;
        fileGateClientHoldingSeconds = config.fileClientHoldingSeconds;
        triggerCallbackIfAsyncMethodReturnFalse = config.forceTriggerCallbackWhenAsyncMethodReturnFalse;
    }
};

struct RTMRegressiveStrategy
{
    int startConnectFailedCount = 3;                //-- 连接失败多少次后，开始退行性处理
    int maxIntervalSeconds = 8;                     //-- 退行性重连最大时间间隔
    int linearRegressiveCount = 4;                  //-- 从第一次退行性连接开始，到最大链接时间，允许尝试几次连接，每次时间间隔都会增大
    int maxRegressvieCount = 10;                     //-- 退行性重连最大次数，超出该次数则不再进行重连
};

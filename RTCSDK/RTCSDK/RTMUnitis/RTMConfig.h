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
    int startConnectFailedCount = 3;                //-- ����ʧ�ܶ��ٴκ󣬿�ʼ�����Դ���
    int maxIntervalSeconds = 8;                     //-- �������������ʱ����
    int linearRegressiveCount = 4;                  //-- �ӵ�һ�����������ӿ�ʼ�����������ʱ�䣬�����Լ������ӣ�ÿ��ʱ������������
    int maxRegressvieCount = 10;                     //-- �����������������������ô������ٽ�������
};

#pragma once
#include <xstring>

struct RTMFile
{
    std::string url;          //-- File url
    int32_t size = 0;        //-- File size

    std::string surl;         //-- Thumb url, only for image type.

    bool isRTMAudio = false;     //-- For RTM audio
    std::string lang;
    std::string codec;   //语音编码 如果是rtm语音消息 会有此值
    int32_t duration = 0;
    int32_t srate = 0;       //语音采样率 如果是rtm语音消息 会有此值
};

enum RTMMsgType
{
    RMT_Withdraw = 1,
    RMT_GEO = 2,
    RMT_MultiLogin = 7,
    RMT_Chat = 30,
    RMT_Cmd = 32,
    RMT_RealAudio = 35,
    RMT_RealVideo = 36,
    RMT_ImageFile = 40,
    RMT_AudioFile = 41,
    RMT_VideoFile = 42,
    RMT_NormalFile = 50
};

struct RTMTranslatedMsg
{
    std::string sourceLanguage;
    std::string targetLanguage;
    std::string sourceText;
    std::string targetText;
};

struct RTMBaseMsg
{
    int8_t msgType;
    int64_t mtime;
    std::string msgText;
    std::string attrs;
    RTMFile fileInfo;
};

struct RTMMsg : RTMBaseMsg
{
    uint64_t fromUid;
    uint64_t toId;                   //-- xid
    uint64_t msgId;
    RTMTranslatedMsg translatedInfo;
};


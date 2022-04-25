#pragma once

enum class RTMFileType
{
    P2P = 0,
    Group,
    Room
};

struct RTMFileInfo
{
    RTMFileType fileType;

    int64_t xid = 0;
    int8_t mtype = 0;
    std::string fileText;
    std::string fileName;
    std::string fileExt;
    std::string userAttrs;

    std::string token;
    std::string ip;
    int32_t port = 0;
    int32_t remainTimeout = 0;
    int64_t lastActionTimestamp = 0;
    std::string rtmAttrs;

    std::function<void(int errorCode, int64_t mtime)> cb;
    bool isValid = false;
    bool isIpv4 = true;
};
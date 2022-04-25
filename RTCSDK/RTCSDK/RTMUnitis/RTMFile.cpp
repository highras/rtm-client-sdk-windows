#include <cstddef>
#include <random>
#include "md5.h"
#include "hex.h"
#include "msec.h"
#include "../RTMClient.h"
#include "../RTMGateQuestProcessor.h"
#include "RTMClientManager.h"
#include "RTMMsg.h"
#include "RTMAudio.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#define RAPIDJSON_HAS_STDSTRING 1

enum empties_t { empties_ok, no_empties };
int32_t _count = 0;
int32_t _randId = 0;
int32_t _randBits = 8;
int32_t _sequenceBits = 6;
int32_t _sequenceMask = 0;
int64_t _lastTime = 0;
std::mutex _mutex;

template <typename Container>
Container& split(
    Container& result,
    const typename Container::value_type& s,
    const typename Container::value_type& delimiters,
    empties_t                      empties = empties_t::empties_ok)
{
    result.clear();
    size_t current;
    size_t next = -1;
    do
    {
        if (empties == empties_t::no_empties)
        {
            next = s.find_first_not_of(delimiters, next + 1);
            if (next == Container::value_type::npos) break;
            next -= 1;
        }
        current = next + 1;
        next = s.find_first_of(delimiters, current);
        result.push_back(s.substr(current, next - current));
    } while (next != Container::value_type::npos);
    return result;
}

int64_t getNextMillis(int64_t lastTime)
{
    int64_t time = slack_real_sec();
    while (time < lastTime)
        time = slack_real_sec();
    return time;
}

int64_t genMid()
{
    std::lock_guard<std::mutex> lck(_mutex);
    int64_t time = slack_real_sec();
    _count = (_count + 1) & _sequenceMask;
    if (_count == 0)
        time = getNextMillis(_lastTime);
    _lastTime = time;
    return (time << (_randBits + _sequenceBits)) | (uint32_t)(_randId << _sequenceBits) | (uint32_t)_count;
}

std::string RTMClient::convertIp4ToIp6(std::string ipv4)
{
    std::vector<std::string> ipv4Fields;
    split(ipv4Fields, ipv4, ".", empties_t::no_empties);
    if (ipv4Fields.size() != 4)
        return std::string();

    char* end;
    for (auto ipField : ipv4Fields)
    {
        int ipFieldt = strtol(ipField.c_str(), &end, 10);
        if (ipFieldt > 255 || ipFieldt < 0)
            return std::string();
    }

    std::stringstream ss8;
    ss8 << std::setfill('0') << std::setw(4)
        << std::hex << strtol(ipv4Fields[2].c_str(), &end, 10) * 0x100 + strtol(ipv4Fields[3].c_str(), &end, 10);
    std::string ipv6Part8 = ss8.str();

    std::stringstream ss7;
    ss7 << std::setfill('0') << std::setw(4)
        << std::hex << strtol(ipv4Fields[0].c_str(), &end, 10) * 0x100 + strtol(ipv4Fields[1].c_str(), &end, 10);
    std::string ipv6Part7 = ss7.str();

    return "64:ff9b::" + ipv6Part7 + ":" + ipv6Part8;
}

void RTMClient::timeout(int32_t& timeout, int64_t& lastActionTimestamp)
{
    int64_t currMsec = slack_real_msec();

    timeout -= (int32_t)((currMsec - lastActionTimestamp) / 1000);

    lastActionTimestamp = currMsec;
}

std::string RTMClient::md5(const string& content)
{
    unsigned char digest[16];
    md5_checksum(digest, content.c_str(), content.size());
    char hexstr[32 + 1];
    Hexlify(hexstr, digest, sizeof(digest));
    return string(hexstr);
}

void RTMClient::connectedForFileClient(const ConnectionInfo& connInfo, bool connected)
{
    if (connected)
    {
        std::string endpoint = rtmFileInfo.ip + ":" + std::to_string(rtmFileInfo.port);
        RTMClientPtr fileClient = RTMClientManager::instance()->getFileClient(endpoint);
        sendFileWithClient(rtmFileInfo, fileClient);
    }
    else if (rtmFileInfo.isValid)
    {
        rtmFileInfo.isIpv4 = false;
        sendFileWithoutClient(rtmFileInfo);
    }
    else
    {
        if (rtmFileInfo.isValid)
            rtmFileInfo.cb(FPNN_EC_CORE_INVALID_CONNECTION, 0);
    }
}

void RTMClient::connectionWillCloseForFileClient(const ConnectionInfo& connInfo, bool closeByError)
{

}

std::string RTMClient::buildJsonForFileAttrs(RTMFileInfo fileInfo)
{
    if (!fileInfo.isValid)
        return "";

    rapidjson::StringBuffer fileBuf;
    rapidjson::Document fileDoc;
    fileDoc.SetObject();

    //rtm field
    {
        rapidjson::Document rtmDoc(&fileDoc.GetAllocator());
        rtmDoc.SetObject();
        if (!fileInfo.rtmAttrs.empty())
        {
            if (rtmDoc.Parse(fileInfo.rtmAttrs.c_str()).HasParseError())
                return "";
            if (rtmDoc.IsObject() == false)
                return "";
        }

        string fileMD5 = md5(fileInfo.fileText);
        std::transform(fileMD5.begin(), fileMD5.end(), fileMD5.begin(), std::tolower);
        string sign = md5(fileMD5 + ":" + fileInfo.token);
        transform(sign.begin(), sign.end(), sign.begin(), ::tolower);
        rapidjson::Value vsign(sign.c_str(), rtmDoc.GetAllocator());
        rtmDoc.AddMember("sign", vsign, rtmDoc.GetAllocator());
        if (fileInfo.fileName.length() > 0)
        {
            rapidjson::Value vname(fileInfo.fileName.c_str(), rtmDoc.GetAllocator());
            rtmDoc.AddMember("filename", vname, rtmDoc.GetAllocator());
            if (fileInfo.fileExt.empty())
            {
                std::size_t found = fileInfo.fileName.find_last_of(".");
                if (found != std::string::npos)
                {
                    std::string ext = fileInfo.fileExt.substr(found, fileInfo.fileExt.length());
                    rapidjson::Value vext(ext.c_str(), rtmDoc.GetAllocator());
                    rtmDoc.AddMember("ext", vext, rtmDoc.GetAllocator());
                }
            }
        }

        fileDoc.AddMember("rtm", rtmDoc, fileDoc.GetAllocator());
    }

    //custom field
    {
        if (!fileInfo.userAttrs.empty())
        {
            rapidjson::Document customDoc(&fileDoc.GetAllocator());
            if (customDoc.Parse(fileInfo.userAttrs.c_str()).HasParseError())
                return "";
            if (customDoc.IsObject() == false)
                return "";
            fileDoc.AddMember("custom", customDoc, fileDoc.GetAllocator());
        }
    }

    rapidjson::Writer<rapidjson::StringBuffer> writer(fileBuf);
    fileDoc.Accept(writer);

    return fileBuf.GetString();
}

FPQuestPtr RTMClient::buildQuestForSendFile(RTMFileInfo fileInfo, int64_t& msgId)
{
    if (!fileInfo.isValid)
        return nullptr;

    shared_ptr<FPQWriter> qw;
    switch (fileInfo.fileType)
    {
    case RTMFileType::P2P:
        qw.reset(new FPQWriter(8, "sendfile"));
        qw->param("to", fileInfo.xid);
        break;

    case RTMFileType::Group:
        qw.reset(new FPQWriter(8, "sendgroupfile"));
        qw->param("gid", fileInfo.xid);
        break;

    case RTMFileType::Room:
        qw.reset(new FPQWriter(8, "sendroomfile"));
        qw->param("rid", fileInfo.xid);
        break;
    }

    msgId = genMid();
    qw->param("pid", pid);
    qw->param("from", uid);
    qw->param("token", fileInfo.token);
    qw->param("mtype", fileInfo.mtype);
    qw->param("mid", msgId);

    qw->param("file", fileInfo.fileText);
    qw->param("attrs", buildJsonForFileAttrs(fileInfo));

    return qw->take();
}

int32_t RTMClient::sendFileWithClient(RTMFileInfo fileInfo, RTMClientPtr fileClient)
{
    if (!fileInfo.isValid || !fileClient)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    timeout(fileInfo.remainTimeout, fileInfo.lastActionTimestamp);
    if (fileInfo.remainTimeout <= 0)
    {
        fileInfo.cb(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return FPNN_EC_CORE_TIMEOUT;
    }

    int64_t msgId = 0;
    FPQuestPtr quest = buildQuestForSendFile(fileInfo, msgId); 
    printQuestJson("SendFile", quest);
    bool success = fileClient->rtmClient->sendQuest(quest, [=](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("SendFile", answer);
        if (errorCode != FPNN_EC_OK)
        {
            if (fileInfo.isValid)
                fileInfo.cb(errorCode, 0);
        }
        else
        {
            FPAReader ar(answer);
            int64_t mtime = ar.getInt("mtime");
            if (fileInfo.isValid)
                fileInfo.cb(errorCode, mtime);

            std::string endpoint = fileInfo.ip + ":" + std::to_string(fileInfo.port);
            RTMClientManager::instance()->pushFileClient(endpoint, shared_from_this());
        }
    }, fileInfo.remainTimeout);

    if (!success)
        fileInfo.cb(FPNN_EC_CORE_INVALID_CONNECTION, 0);

    return FPNN_EC_OK;
}

int32_t RTMClient::sendFileWithoutClient(RTMFileInfo fileInfo)
{
    if (!fileInfo.isValid)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::string ip;
    if (fileInfo.isValid && fileInfo.isIpv4)
        ip = fileInfo.ip;
    else
    {
        std::string ipv6 = convertIp4ToIp6(fileInfo.ip);
        if (ipv6.empty())
            ip = ipv6;
        else
        {
            fileInfo.cb(FPNN_EC_CORE_INVALID_CONNECTION, 0);
            return FPNN_EC_CORE_INVALID_CONNECTION;
        }
    }

    RTMClientPtr fileClient = std::make_shared<RTMClient>(ip, fileInfo.port, pid, uid);
    if (fileClient->rtmClient)
    {
        fileClient->rtmFileInfo = fileInfo;
        if (fileClient->rtmClient->asyncConnect())
        {
            dynamic_cast<RTMGateQuestProcessor*>(fileClient->rtmProcessor.get())->SetClientPtr(fileClient, RTMType::RT_FILE);

            std::string endpoint = fileInfo.ip + ":" + std::to_string(fileInfo.port);
            RTMClientManager::instance()->pushFileClient(endpoint, fileClient);
        }
        else
            return FPNN_EC_CORE_INVALID_CONNECTION;
    }
    else
        return FPNN_EC_CORE_INVALID_CONNECTION;

    return FPNN_EC_OK;
}

int32_t RTMClient::sendFileWithToken(RTMFileInfo fileInfo, string token, string endpoint)
{
    if (!fileInfo.isValid)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::string ip;
    int32_t errorCode = 0;
    int32_t port = 0;
    size_t pos = endpoint.find(":");
    if (pos != -1)
    {
        char* end = nullptr;
        ip = endpoint.substr(0, pos);
        port = strtol(endpoint.substr(pos + 1, endpoint.length()).c_str(), &end, 10);
    }

    fileInfo.token = token;
    fileInfo.ip = ip;
    fileInfo.port = port;

    RTMClientPtr fileClient = RTMClientManager::instance()->getFileClient(endpoint);
    if (fileClient)
        errorCode = sendFileWithClient(fileInfo, fileClient);
    else
        errorCode = sendFileWithoutClient(fileInfo);

    return errorCode;
}

bool RTMClient::sendFile(
    function<void(int errorCode, int64_t mtime)> callback,
    RTMFileType fileType,
    int64_t xid,
    int8_t mtype,
    std::string fileText,
    std::string filename,
    std::string fileExt,
    string userAttrs,
    std::string rtmAttrs,
    int timeout) {
    if (mtype < 40 || mtype > 50)
    {
        return false;
    }

    RTMFileInfo fileInfo;
    fileInfo.isValid = true;
    fileInfo.fileType = fileType,
    fileInfo.xid = xid,
    fileInfo.mtype = mtype,
    fileInfo.fileText = fileText,
    fileInfo.fileName = filename,
    fileInfo.fileExt = fileExt,
    fileInfo.userAttrs = userAttrs,
    fileInfo.remainTimeout = timeout,
    fileInfo.lastActionTimestamp = slack_real_msec(),
    fileInfo.cb = callback;
    fileInfo.rtmAttrs = rtmAttrs;

    bool asyncStarted = FileToken(fileInfo.xid, (int16_t)fileInfo.fileType,
        [=](int errorCode, std::string token, std::string endpoint){
            if (errorCode == 0)
                sendFileWithToken(fileInfo, token, endpoint);
            else
                fileInfo.cb(FPNN_EC_CORE_INVALID_CONNECTION, 0);
    });

    if (!asyncStarted)
        fileInfo.cb(FPNN_EC_CORE_INVALID_CONNECTION, 0);

    return asyncStarted;
}

bool RTMClient::FileToken(int64_t xid, int16_t fileType, function<void(int errorCode, string token, string endpoint)> callback)
{
    FPQWriter qw(2, "filetoken");

    switch ((RTMFileType)fileType)
    {
    case RTMFileType::P2P:
        qw.param("cmd", "sendfile");
        qw.param("to", xid);
        break;

    case RTMFileType::Group:
        qw.param("cmd", "sendgroupfile");
        qw.param("gid", xid);
        break;

    case RTMFileType::Room:
        qw.param("cmd", "sendroomfile");
        qw.param("rid", xid);
        break;
    }
    FPQuestPtr quest = qw.take();
    printQuestJson("FileToken", quest);

    return rtmClient->sendQuest(quest, [callback](FPAnswerPtr answer, int errorCode) {
        printAnswerJson("FileToken", answer);
        string token, endpoint;
        if (errorCode != FPNN_EC_OK)
        {
            callback(errorCode, token, endpoint);
            return;
        }

        FPAReader ar(answer);
        token = ar.getString("token", token);
        endpoint = ar.getString("endpoint", endpoint);
        callback(errorCode, token, endpoint);
        });
}

void RTMClient::SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout)
{
    sendFile(callback, RTMFileType::P2P, pid, mtype, fileText, fileName, fileExt, userAttrs, "", timeout);
}

void RTMClient::SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout)
{
    sendFile(callback, RTMFileType::Group, gid, mtype, fileText, fileName, fileExt, userAttrs, "", timeout);
}

void RTMClient::SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout)
{
    sendFile(callback, RTMFileType::Room, rid, mtype, fileText, fileName, fileExt, userAttrs, "", timeout);
}


std::string buildRTMAudio(RTMAudio rtmAudio)
{
    rapidjson::StringBuffer rtmAttrs;
    rapidjson::Writer<rapidjson::StringBuffer> rtmWriter(rtmAttrs);
    rtmWriter.StartObject();
    rtmWriter.Key("type");
    rtmWriter.String("audiomsg");
    rtmWriter.Key("codec");
    rtmWriter.String(rtmAudio.audioCodec.c_str());
    rtmWriter.Key("srate");
    rtmWriter.Int(rtmAudio.audioSampleRate);
    rtmWriter.Key("lang");
    rtmWriter.String(rtmAudio.audioLang.c_str());
    rtmWriter.Key("duration");
    rtmWriter.Int64(rtmAudio.audioDur);
    rtmWriter.Key("ext");
    rtmWriter.String("amr");
    rtmWriter.EndObject();

    return rtmAttrs.GetString();
}

void RTMClient::SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, RTMAudio rtmAudio, int timeout)
{
    sendFile(callback, RTMFileType::P2P, pid, RTMMsgType::RMT_AudioFile, rtmAudio.audioData, "", "", "", buildRTMAudio(rtmAudio), timeout);
}

void RTMClient::SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, RTMAudio rtmAudio, int timeout)
{
    sendFile(callback, RTMFileType::Group, gid, RTMMsgType::RMT_AudioFile, rtmAudio.audioData, "", "", "", buildRTMAudio(rtmAudio), timeout);
}

void RTMClient::SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, RTMAudio rtmAudio, int timeout )
{
    sendFile(callback, RTMFileType::Room, rid, RTMMsgType::RMT_AudioFile, rtmAudio.audioData, "", "", "", buildRTMAudio(rtmAudio), timeout);
}

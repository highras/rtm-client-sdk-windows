#include "RTCGateQuestProcessor.h"

using namespace std;

RTCGateQuestProcessor::RTCGateQuestProcessor():
    videoCallback(nullptr),
    voiceCallback(nullptr)
{
    registerMethod("pushVoice", &RTCGateQuestProcessor::voice);
    registerMethod("pushVideo", &RTCGateQuestProcessor::video);
    registerMethod("ping", &RTCGateQuestProcessor::ping);

    registerMethod("pushP2PVoice", &RTCGateQuestProcessor::p2pVoice);
    registerMethod("pushP2PVideo", &RTCGateQuestProcessor::p2pVideo);
}
RTCGateQuestProcessor::~RTCGateQuestProcessor() {}

FPAnswerPtr RTCGateQuestProcessor::video(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
    int64_t timestamp = args->wantInt("timestamp");
    int64_t uid = args->wantInt("uid");
    int64_t rid = args->wantInt("rid"); 
    int64_t seq = args->wantInt("seq");
    int64_t flags = args->wantInt("flags");
    int64_t rotation = args->wantInt("rotation");
    int64_t version = args->wantInt("version"); 
    int32_t facing = args->wantInt("facing");  
    int32_t captureLevel = args->wantInt("captureLevel");
    vector<unsigned char> data = args->want("data", vector<unsigned char>()); 
    vector<unsigned char> sps = args->want("sps", vector<unsigned char>()); 
    vector<unsigned char> pps = args->want("pps", vector<unsigned char>());

    if (videoCallback)
        videoCallback(rid, uid, seq, flags, timestamp, rotation, version, facing, captureLevel, data, sps, pps);

    return nullptr;
}

FPAnswerPtr RTCGateQuestProcessor::voice(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
    int64_t timestamp = args->wantInt("timestamp");
    int64_t uid = args->wantInt("uid");
    int64_t rid = args->wantInt("rid");
    int64_t seq = args->wantInt("seq");
    vector<unsigned char> data = args->want("data", vector<unsigned char>());

    if (voiceCallback)
        voiceCallback(uid, rid, seq, timestamp, data);

    return nullptr;
}

FPAnswerPtr RTCGateQuestProcessor::ping(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
    return FPAWriter::emptyAnswer(quest);
}

FPAnswerPtr RTCGateQuestProcessor::p2pVoice(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
    int64_t timestamp = args->wantInt("timestamp");
    int64_t uid = args->wantInt("uid");
    int64_t seq = args->wantInt("seq");
    vector<unsigned char> data = args->want("data", vector<unsigned char>());

    if (p2pVoiceCallback)
        p2pVoiceCallback(uid, seq, timestamp, data);

    return nullptr;
}

FPAnswerPtr RTCGateQuestProcessor::p2pVideo(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci)
{
    int64_t timestamp = args->wantInt("timestamp");
    int64_t uid = args->wantInt("uid");
    int64_t seq = args->wantInt("seq");
    int64_t flags = args->wantInt("flags");
    int64_t rotation = args->wantInt("rotation");
    int64_t version = args->wantInt("version");
    int32_t facing = args->wantInt("facing");
    int32_t captureLevel = args->wantInt("captureLevel");
    vector<unsigned char> data = args->want("data", vector<unsigned char>());
    vector<unsigned char> sps = args->want("sps", vector<unsigned char>());
    vector<unsigned char> pps = args->want("pps", vector<unsigned char>());

    if (p2pVideoCallback)
        p2pVideoCallback(uid, seq, flags, timestamp, rotation, version, facing, captureLevel, data, sps, pps);

    return nullptr;
}

void RTCGateQuestProcessor::SetVideoCallback(VideoCallback callback)
{
    videoCallback = callback;
}

void RTCGateQuestProcessor::SetVoiceCallback(VoiceCallback callback)
{
    voiceCallback = callback;
}

void RTCGateQuestProcessor::SetP2PVideoCallback(P2PVideoCallback callback)
{
    p2pVideoCallback = callback;
}

void RTCGateQuestProcessor::SetP2PVoiceCallback(P2PVoiceCallback callback)
{
    p2pVoiceCallback = callback;
}

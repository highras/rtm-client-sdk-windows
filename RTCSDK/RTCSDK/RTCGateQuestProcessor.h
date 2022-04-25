#pragma once
#include <IQuestProcessor.h>
#include <functional>

using namespace fpnn;
using namespace std;

class RTCGateQuestProcessor : public IQuestProcessor
{
    QuestProcessorClassPrivateFields(RTCGateQuestProcessor)

    typedef function<void(int64_t rid, int64_t uid, int64_t seq,
        int64_t flags, int64_t timestamp, int64_t rotation,
        int64_t version, int32_t facing, int32_t captureLevel,
        vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps)> VideoCallback;
    VideoCallback videoCallback;
    typedef function<void(int64_t uid, int64_t seq,
        int64_t flags, int64_t timestamp, int64_t rotation,
        int64_t version, int32_t facing, int32_t captureLevel,
        vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps)> P2PVideoCallback;
    P2PVideoCallback p2pVideoCallback;
    typedef function<void(int64_t uid, int64_t rid, int64_t seq, int64_t timestamp, vector<unsigned char> data)> VoiceCallback;
    VoiceCallback voiceCallback;
    typedef function<void(int64_t uid, int64_t seq, int64_t timestamp, vector<unsigned char> data)> P2PVoiceCallback;
    P2PVoiceCallback p2pVoiceCallback;
public:
    RTCGateQuestProcessor();
    ~RTCGateQuestProcessor();

    FPAnswerPtr voice(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
    FPAnswerPtr video(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
    FPAnswerPtr ping(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);

    FPAnswerPtr p2pVoice(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);
    FPAnswerPtr p2pVideo(const FPReaderPtr args, const FPQuestPtr quest, const ConnectionInfo& ci);

    void SetVideoCallback(VideoCallback callback);
    void SetVoiceCallback(VoiceCallback callback);

    void SetP2PVideoCallback(P2PVideoCallback callback);
    void SetP2PVoiceCallback(P2PVoiceCallback callback);

    QuestProcessorClassBasicPublicFuncs
};
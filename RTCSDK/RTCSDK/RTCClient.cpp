//#include "Objbase.h"
#include "RTCClient.h"
#include "RTCGateQuestProcessor.h"
using namespace std;

RTCClient::RTCClient(string host, unsigned short port) :
	client(UDPClient::createClient(host, port))
{
	processor = make_shared<RTCGateQuestProcessor>();
	client->setQuestProcessor(processor);

    client->connect();
}

RTCClient::~RTCClient()
{
}

bool RTCClient::EnterRTCRoom(int64_t pid, int64_t rid, int64_t uid, string token)
{
	FPQWriter qw(4, "enterRTCRoom");
	qw.param("pid", pid);
	qw.param("rid", rid);
	qw.param("uid", uid);
	qw.param("token", token);
    FPAnswerPtr answer4 = client->sendQuest(qw.take());
    if (answer4->status() != 0)
    {
        return false;
    }
    else
    {
        FPAReader ar4(answer4);
        ar4.getBool("microphone", false);
        return true;
    }

}

void RTCClient::EnterRTCRoom(int64_t pid, int64_t rid, int64_t uid, string token, function<void(int errorCode, bool microphone)> callback)
{
    FPQWriter qw(4, "enterRTCRoom");
    qw.param("pid", pid);
    qw.param("rid", rid);
    qw.param("uid", uid);
    qw.param("token", token);
    client->sendQuest(qw.take(), [callback](FPAnswerPtr answer, int errorCode) {
        if (errorCode != fpnn::FPNN_EC_OK)
        {
            FPAReader ar(answer);
            bool microphone = ar.wantBool("microphone");
            callback(errorCode, microphone);
        }
        else
        {
            callback(errorCode, false);
        }
        });
}

void RTCClient::SetVideoCallback(function<void(int64_t rid, int64_t uid, int64_t seq, int64_t flags, int64_t timestamp, int64_t rotation, int64_t version, int32_t facing, int32_t captureLevel, vector<unsigned char>data, vector<unsigned char>sps, vector<unsigned char>pps)> callback)
{
    dynamic_cast<RTCGateQuestProcessor*>(processor.get())->SetVideoCallback(callback);
}

void RTCClient::SetAudioCallback(function<void(int64_t uid, int64_t rid, int64_t seq, int64_t timestamp, vector<unsigned char>data)> callback)
{
    dynamic_cast<RTCGateQuestProcessor*>(processor.get())->SetVoiceCallback(callback);
}

void RTCClient::SendAudioData(int64_t rid, int64_t seq, int64_t timestamp, vector<unsigned char> data)
{
    FPQWriter qw(4, "voice", true);
    qw.param("timestamp", timestamp);
    qw.param("data", data);
    qw.param("seq", seq);
    qw.param("rid", rid);
    client->sendQuest(qw.take());
}

void RTCClient::SendVideoData(int64_t rid, int64_t seq, int64_t flags, int64_t timestamp, int64_t rotation, int64_t version, int32_t facing, int32_t captureLevel, vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps)
{
    FPQWriter qw(11, "video", true);
    qw.param("timestamp", timestamp);
    qw.param("seq", seq);
    qw.param("data", data);
    qw.param("rid", rid);
    qw.param("flags", flags);
    qw.param("sps", sps);
    qw.param("pps", pps);
    qw.param("facing", facing);
    qw.param("version", version);
    qw.param("captureLevel", version);
    qw.param("rotation", version);
    client->sendQuest(qw.take());
}

void RTCClient::SetP2PVideoCallback(function<void(int64_t uid, int64_t seq, int64_t flags, int64_t timestamp, int64_t rotation, int64_t version, int32_t facing, int32_t captureLevel, vector<unsigned char>data, vector<unsigned char>sps, vector<unsigned char>pps)> callback)
{
    dynamic_cast<RTCGateQuestProcessor*>(processor.get())->SetP2PVideoCallback(callback);
}

void RTCClient::SetP2PAudioCallback(function<void(int64_t uid, int64_t seq, int64_t timestamp, vector<unsigned char>data)> callback)
{
    dynamic_cast<RTCGateQuestProcessor*>(processor.get())->SetP2PVoiceCallback(callback);
}

void RTCClient::SetP2PRequest(int64_t pid, int64_t uid, int32_t type, int64_t peerUid, int64_t callid, function<void(int errorCode)> callback)
{
    FPQWriter qw(5, "enterRTCRoom");
    qw.param("pid", pid);
    qw.param("uid", uid);
    qw.param("type", type);
    qw.param("peerUid", peerUid);
    qw.param("callid", callid);
    client->sendQuest(qw.take(), [callback](FPAnswerPtr answer, int errorCode) {
        if (errorCode != fpnn::FPNN_EC_OK)
        {
            // error log
            callback(errorCode);
        }
        else
        {
            callback(errorCode);
        }
        });
}

void RTCClient::SendP2PAudioData(int64_t seq, int64_t timestamp, vector<unsigned char> data)
{
	FPQWriter qw(3, "voiceP2P", true);
	qw.param("timestamp", timestamp);
	qw.param("data", data);
	qw.param("seq", seq);
	client->sendQuest(qw.take());
}

void RTCClient::SendP2PVideoData(int64_t seq, int64_t flags, int64_t timestamp, int64_t rotation, int64_t version, int32_t facing, int32_t captureLevel, vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps)
{
	FPQWriter qw(10, "VideoP2P", true);
	qw.param("timestamp", timestamp);
	qw.param("seq", seq);
	qw.param("data", data);
	qw.param("flags", flags);
	qw.param("sps", sps);
	qw.param("pps", pps);
	qw.param("facing", facing);
	qw.param("version", version);
	qw.param("captureLevel", version);
	qw.param("rotation", version);
	client->sendQuest(qw.take());
}

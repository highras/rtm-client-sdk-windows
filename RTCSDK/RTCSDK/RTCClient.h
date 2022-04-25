#pragma once

#include <TCPClient.h>
#include <UDPClient.h>
#include <IQuestProcessor.h>

using namespace fpnn;
using namespace std;

class RTCClient
{
	ClientPtr client;
	IQuestProcessorPtr processor;
public:
    RTCClient(string host, unsigned short port);
	virtual ~RTCClient();

	bool EnterRTCRoom(int64_t pid, int64_t rid, int64_t uid, string token);
	void EnterRTCRoom(int64_t pid, int64_t rid, int64_t uid, string token, function<void(int errorCode, bool microphone)> callback);
	void SetVideoCallback(function<
		void(int64_t rid, int64_t uid, int64_t seq,
			int64_t flags, int64_t timestamp, int64_t rotation,
			int64_t version, int32_t facing, int32_t captureLevel,
			vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps)> callback);
	void SetAudioCallback(function<void(int64_t uid, int64_t rid, int64_t seq, int64_t timestamp, vector<unsigned char> data)> callback);
    void SendAudioData(int64_t rid, int64_t seq, int64_t timestamp, vector<unsigned char> data);
	void SendVideoData(int64_t rid, int64_t seq, int64_t flags, int64_t timestamp, int64_t rotation,
		int64_t version, int32_t facing, int32_t captureLevel, 
		vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps);

	void SetP2PVideoCallback(function<
		void(int64_t uid, int64_t seq,
			int64_t flags, int64_t timestamp, int64_t rotation,
			int64_t version, int32_t facing, int32_t captureLevel,
			vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps)> callback);
	void SetP2PAudioCallback(function<void(int64_t uid, int64_t seq, int64_t timestamp, vector<unsigned char> data)> callback);

	void SetP2PRequest(int64_t pid, int64_t uid, int32_t type, int64_t peerUid, int64_t callid, function<void(int errorCode)> callback);
	void SendP2PAudioData(int64_t seq, int64_t timestamp, vector<unsigned char> data);
	void SendP2PVideoData(int64_t seq, int64_t flags, int64_t timestamp, int64_t rotation,
		int64_t version, int32_t facing, int32_t captureLevel,
		vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps);

};


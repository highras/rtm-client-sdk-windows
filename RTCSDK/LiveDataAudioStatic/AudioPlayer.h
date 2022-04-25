#pragma once
#include "WASAPIRenderer.h"
#include <unordered_map>
#include <list>
#include <vector>


struct OpusDecoder;
typedef struct SRC_STATE_tag SRC_STATE;

using namespace std;

class AudioPlayer
{
	struct JitterBufferData
	{
		list<pair<long long, vector<BYTE>>> uidData;
		long long timestamp;
		bool isReady;
		mutex bufferMutex;
	};
	CWASAPIRenderer* renderer;
	std::thread* hTimer;
	bool running = false;
	unordered_map<long long, JitterBufferData> mJitterBuffer;
	unordered_map<long long, OpusDecoder*> mDecoderHandlers;
	int32_t sampleRate;
	bool needResample = false;

	bool started = false;
	mutex startedMutex;

	SRC_STATE* resampler;

	static inline void MixAudio(float* output, float* input, size_t frameSize)
	{
		for (int i = 0; i != frameSize; i++)
		{
			output[i] += input[i];
			if (output[i] > 1.0f)
			{
				output[i] = 1.0f;
			}
			else if (output[i] < -1.0f)
			{
				output[i] = -1.0f;
			}
		}
	}

	static void WINAPI TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
public:

	AudioPlayer();
	virtual ~AudioPlayer();

	static AudioPlayer* GetInstance();
	void PutAudioData(long long uid, long long timestamp, char* data, size_t length);
	long long GetUserTimestamp(long long uid);
	void Start();
	void Stop();
};


#pragma once
#include "WASAPICapture.h"

#include <vector>
#include <functional>

struct OpusEncoder;
typedef struct SRC_STATE_tag SRC_STATE;

class AudioRecorder
{
	CWASAPICapture* capture;
	function<void(shared_ptr<vector<BYTE>> data)> OnAudioReady;
	std::thread* hTimer;
	bool running = false;
	OpusEncoder* encoder;

	bool started = false;
	mutex startedMutex;

	int32_t sampleRate;
	bool needResample = false;

	SRC_STATE* resampler;

	static void WINAPI TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
public:

	AudioRecorder();
	virtual ~AudioRecorder();
	static AudioRecorder* GetInstance();
	void SetAudioCallback(function<void(shared_ptr<vector<BYTE>> data)> callback);
	void Start();
	void Stop();
};


#pragma once
#include "AudioCapture.h"

#include <vector>
#pragma once
#include <functional>
#include "CWaveFile.h"

struct OpusEncoder;
typedef struct SRC_STATE_tag SRC_STATE;

class WaveRecorder
{
	AudioCapture* capture;
	function<void(double db)> OnAudioReady;
	std::thread* hTimer;
	bool running = false;

	bool started = false;
	mutex startedMutex;

	int32_t sampleRate;
	bool needResample = false;

	CWaveFile file;

	SRC_STATE* resampler;

	int32_t duration = 0;

	static void WINAPI TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
public:
	WaveRecorder();
	virtual ~WaveRecorder();
	static WaveRecorder* GetInstance();
	void SetAudioCallback(function<void(double db)> callback);
	virtual bool Start();
	virtual void Stop(std::string& audioData);
	int32_t GetDuration();
	int32_t GetSampleRate();
};


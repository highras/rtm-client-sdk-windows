#pragma once
#include "WASAPICapture.h"

class AudioRecorder
{
	CWASAPICapture* capture;
	void (*OnAudioReady)(shared_ptr<BYTE> data, size_t size);
	HANDLE hTimer;

	AudioRecorder();
	virtual ~AudioRecorder();

	static void TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
public:
	static AudioRecorder* GetInstance();
	void SetAudioCallback(void (*callback)(shared_ptr<BYTE> data, size_t size));
	void Start();
	void Stop();
};


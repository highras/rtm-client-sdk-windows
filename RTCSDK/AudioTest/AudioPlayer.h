#pragma once
#include "WASAPIRenderer.h"

class AudioPlayer
{
	CWASAPIRenderer* renderer;
	void (*OnAudioReady)(shared_ptr<BYTE> data, size_t count);
	HANDLE hTimer;

	AudioPlayer();
	virtual ~AudioPlayer();

	static void TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
public:
	static AudioPlayer* GetInstance();
	void SetAudioCallback(void(*callback)(shared_ptr<BYTE> data, size_t count));
	void Start();
	void Stop();
};


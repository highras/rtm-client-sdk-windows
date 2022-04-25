#pragma once
#include <functional>
#include <thread>
#include <Windows.h>
#include <mmeapi.h>

using namespace std;


class WavePlayer
{
	HWAVEOUT waveout;
	thread* th;
	HANDLE events[5];// 0 for star 1 for stop 2 for complete 3 for exit 4 for started
	function<void()> completeCallback;
	function<void()> startedCallback;
	function<void()> stopedCallback;

	char* data;
	size_t len;

	static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
public:
	WavePlayer();
	virtual ~WavePlayer();

	static WavePlayer* GetInstance();
	virtual bool Start(void* data, size_t len);
	virtual void SetCompleteCallback(function<void()> callback);
	virtual void SetStartedCallback(function<void()> callback);
	virtual void SetStopedCallback(function<void()> callback);
	virtual bool Stop();
};


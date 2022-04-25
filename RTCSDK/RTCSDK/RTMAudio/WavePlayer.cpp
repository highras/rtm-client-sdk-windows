#include "WavePlayer.h"
#include <thread>

void WavePlayer::waveOutProc(
	HWAVEOUT  hwo,
	UINT      uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
)
{
	WavePlayer* thiz = (WavePlayer*)dwInstance;
	switch (uMsg)
	{
	case WOM_DONE:
		SetEvent(thiz->events[2]);
		break;
	case WOM_OPEN:
		SetEvent(thiz->events[4]);
		break;
	case WOM_CLOSE:
		break;
	default:
		break;
	}
}

WavePlayer::WavePlayer():
	waveout(NULL),
	data(nullptr),
	len(0)
{
	for (auto& ev : events)
	{
		ev = CreateEvent(nullptr, true, false, nullptr);
	}
	th = new thread([this]() 
		{
			while (true)
			{
				DWORD res = WaitForMultipleObjects(5, events, false, 100);
				if (res == WAIT_TIMEOUT)
				{

				}
				else if (res == WAIT_OBJECT_0)
				{
					if (waveout == NULL)
					{
						UINT deviceNum = waveOutGetNumDevs();
						if (deviceNum == 0)
							continue;

						MMRESULT res = waveOutOpen(&waveout, 0, (LPCWAVEFORMATEX)(data + 20), (DWORD_PTR)waveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
						WAVEHDR wavehdr;
						wavehdr.lpData = (LPSTR)(data);
						wavehdr.dwBufferLength = len;
						wavehdr.dwFlags = 0;
						waveOutPrepareHeader(waveout, &wavehdr, sizeof(WAVEHDR));
						res = waveOutWrite(waveout, &wavehdr, sizeof(WAVEHDR));
					}
					ResetEvent(events[0]);
				}
				else if (res == WAIT_OBJECT_0 + 1)
				{
					if (waveout != NULL)
					{
						waveOutReset(waveout);
						waveOutClose(waveout);
						waveout = NULL;
					}
					if (stopedCallback)
						stopedCallback();
					ResetEvent(events[1]);
				}
				else if (res == WAIT_OBJECT_0 + 2)
				{
					if (waveout != NULL)
					{
						waveOutClose(waveout);
						waveout = NULL;
						if (completeCallback)
							completeCallback();
					}
					ResetEvent(events[2]);
				}
				else if (res == WAIT_OBJECT_0 + 3)
				{
					return;
				}
				else if (res == WAIT_OBJECT_0 + 4)
				{
					if (startedCallback)
						startedCallback();
					ResetEvent(events[4]);
				}
			}
		});
}

WavePlayer::~WavePlayer()
{
	SetEvent(events[3]);
	th->join();
	for (auto& ev : events)
	{
		CloseHandle(ev);
	}
}

WavePlayer* WavePlayer::GetInstance()
{
	static WavePlayer* a = new WavePlayer();
	return a;
}

bool WavePlayer::Start(void* data, size_t len)
{
	if (!data || len == 0)
		return false;
	this->data = (char*)data;
	this->len = len;
	return SetEvent(events[0]);
}

void WavePlayer::SetCompleteCallback(function<void()> callback)
{
	completeCallback = callback;
}

void WavePlayer::SetStartedCallback(function<void()> callback)
{
	startedCallback = callback;
}

void WavePlayer::SetStopedCallback(function<void()> callback)
{
	stopedCallback = callback;
}

bool WavePlayer::Stop()
{
	return SetEvent(events[1]);
}
#include "AmrwbPlayer.h"
#include <voiceConverter.h>
#include <vector>

AmrwbPlayer::AmrwbPlayer()
{
	WavePlayer::SetCompleteCallback([this]() {
		if (wavdata)
		{
			free(wavdata);
			wavdata = nullptr;
			wavsize = 0;
		}
		if (completeCallback)
			completeCallback();
		});
	WavePlayer::SetStartedCallback([this]() {
		if (startedCallback)
			startedCallback();
		});
	WavePlayer::SetStopedCallback([this]() {
		if (wavdata)
		{
			free(wavdata);
			wavdata = nullptr;
			wavsize = 0;
		}
		});
}

AmrwbPlayer::~AmrwbPlayer()
{
	if (wavdata)
	{
		free(wavdata);
		wavdata = nullptr;
		wavsize = 0;
	}
}

bool AmrwbPlayer::Start(void* data, size_t len)
{
	int status;
	wavsize = 0;
	wavdata = convert_amrwb_to_wav((char*)data, len, status, wavsize);
    return WavePlayer::Start(wavdata, wavsize);
}

void AmrwbPlayer::SetCompleteCallback(function<void()> callback)
{
	completeCallback = callback;
}

void AmrwbPlayer::SetStartedCallback(function<void()> callback)
{
	startedCallback = callback;
}

#include "AudioPlayer.h"
#include "framework.h"

AudioPlayer::AudioPlayer():
hTimer(NULL),
OnAudioReady(nullptr),
renderer(nullptr)
{
    HRESULT hr;
    IMMDeviceEnumerator* deviceEnumerator = NULL;
    IMMDeviceCollection* deviceCollection = NULL;
    IMMDevice* device = NULL;
    ERole role = eConsole;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
        printf("Unable to instantiate device enumerator: %x\n", hr);
        goto Exit;
    }

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, role, &device);
    if (FAILED(hr))
    {
        printf("Unable to get default device for role %d: %x\n", role, hr);
        goto Exit;
    }

    renderer = new CWASAPIRenderer(device, false, role);
    renderer->Initialize(30);
Exit:
    SAFE_RELEASE(deviceCollection);
    SAFE_RELEASE(deviceEnumerator);
}

AudioPlayer::~AudioPlayer()
{
}

void AudioPlayer::TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	AudioPlayer* thiz = (AudioPlayer*)lpParameter;
	shared_ptr<BYTE> data = shared_ptr<BYTE>(new BYTE[960*2*4]);
	if (thiz->OnAudioReady)
	{
		thiz->OnAudioReady(data, 960 * 2 * 4);
        if (thiz->renderer)
		    thiz->renderer->PutAudioData(data.get(), 960 * 2 * 4);
	}
}

AudioPlayer* AudioPlayer::GetInstance()
{
    static auto a = new AudioPlayer();
	return a;
}

void AudioPlayer::SetAudioCallback(void(*callback)(shared_ptr<BYTE> data, size_t count))
{
	OnAudioReady = callback;
}

void AudioPlayer::Start()
{
    if (!hTimer)
    {
        BOOL res = CreateTimerQueueTimer(&hTimer, NULL, TimerProc, this, 0, 20, WT_EXECUTEDEFAULT);
    }
    if (renderer)
	    renderer->Start();
}

void AudioPlayer::Stop()
{
    if(renderer)
	    renderer->Stop();
    if (hTimer) 
    {
        DeleteTimerQueueTimer(NULL, hTimer, NULL);
        hTimer = NULL;
    }
}

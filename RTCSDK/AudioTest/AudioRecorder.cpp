#include "AudioRecorder.h"

#include "framework.h"

AudioRecorder::AudioRecorder():
hTimer(NULL),
OnAudioReady(nullptr),
capture(nullptr)
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

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, role, &device);
    if (FAILED(hr))
    {
        printf("Unable to get default device for role %d: %x\n", role, hr);
        goto Exit;
    }

    capture = new CWASAPICapture(device, false, role);
    capture->Initialize(30);
Exit:
    SAFE_RELEASE(deviceCollection);
    SAFE_RELEASE(deviceEnumerator);
}

AudioRecorder::~AudioRecorder()
{
}

void AudioRecorder::TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
    AudioRecorder* thiz = (AudioRecorder*)lpParameter;
    shared_ptr<BYTE> data = shared_ptr<BYTE>(new BYTE[960*2*4]);
    size_t readed = 0;
    if (thiz->capture)
       readed = thiz->capture->GetAudioData(data.get(), 960 * 2 * 4);
    if (thiz->OnAudioReady && readed > 0)
    {
        thiz->OnAudioReady(data, 960*2*4);
    }
}

AudioRecorder* AudioRecorder::GetInstance()
{
    static auto a = new AudioRecorder();
    return a;
}

void AudioRecorder::SetAudioCallback(void(*callback)(shared_ptr<BYTE> data, size_t size))
{
    OnAudioReady = callback;
}

void AudioRecorder::Start()
{
    if (!hTimer)
    {
        BOOL res = CreateTimerQueueTimer(&hTimer, NULL, TimerProc, this, 0, 20, WT_EXECUTEDEFAULT);
    }
    if (capture)
        capture->Start();
}

void AudioRecorder::Stop()
{
    if (capture)
        capture->Stop();
    if (hTimer)
    {
        DeleteTimerQueueTimer(NULL, hTimer, NULL);
        hTimer = NULL;
    }
}

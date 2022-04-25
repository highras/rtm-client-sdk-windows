#include "AudioRecorder.h"

#include "framework.h"

#include <opus.h>

AudioRecorder::AudioRecorder():
hTimer(NULL),
OnAudioReady(nullptr),
capture(nullptr),
encoder(nullptr),
resampler(nullptr)
{
    HRESULT hr;
    IMMDeviceEnumerator* deviceEnumerator = NULL;
    IMMDeviceCollection* deviceCollection = NULL;
    IMMDevice* device = NULL;
    ERole role = eCommunications;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to instantiate device enumerator: %x\n", hr);
        goto Exit;
    }

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, role, &device);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to get default device for role %d: %x\n", role, hr);
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
    unique_lock<mutex> lck(startedMutex);
    if (capture) 
    {
        if (started)
            Stop();
        capture->Shutdown();
        capture->Release();
    }
}

void AudioRecorder::TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
    AudioRecorder* thiz = (AudioRecorder*)lpParameter;
    size_t sizeInByte = thiz->capture->SamplesPerSecond() * 20 * thiz->capture->BytesPerSample() * thiz->capture->ChannelCount() / 1000;
    static shared_ptr<BYTE> pcmData = shared_ptr<BYTE>(new BYTE[sizeInByte]);
    static shared_ptr<BYTE> tmpData = shared_ptr<BYTE>(new BYTE[4000]);

    HANDLE tEvent = CreateEvent(nullptr, true, false, nullptr);
    HANDLE hTimer = NULL;
    CreateTimerQueueTimer(&hTimer, NULL, [](PVOID param, BOOLEAN) {
        SetEvent(param);
        }, tEvent, 0, 20, WT_EXECUTEDEFAULT);

    while (thiz->running)
    {
        size_t readed = 0;
        int length = 0;
        if (thiz->capture)
            readed = thiz->capture->GetAudioData(pcmData.get(), sizeInByte);
        // resample
        if (readed != sizeInByte)
            continue;

        if (thiz->needResample)
        {
            SRC_DATA srcData;
            srcData.data_in = (float*)pcmData.get();
            srcData.input_frames = thiz->capture->SamplesPerSecond() * 20 / 1000;
            int32_t resampleSize = sizeInByte * thiz->sampleRate / thiz->capture->SamplesPerSecond();
            static shared_ptr<BYTE> resampleData = shared_ptr<BYTE>(new BYTE[resampleSize]);
            srcData.output_frames = thiz->sampleRate * 20 / 1000;
            srcData.data_out = (float*)resampleData.get();
            srcData.end_of_input = 0;
            srcData.src_ratio = double(thiz->sampleRate) / double(thiz->capture->SamplesPerSecond());
            src_process(thiz->resampler, &srcData);

            if (srcData.output_frames_gen > 0)
                length = opus_encode_float(thiz->encoder, (float*)resampleData.get(), srcData.output_frames_gen, tmpData.get(), 4000);
            else
                continue;
        }
        else
        {
            length = opus_encode_float(thiz->encoder, (float*)pcmData.get(), thiz->sampleRate * 20 / 1000, tmpData.get(), 4000);
        }

        if (length < OPUS_OK)
        {
            fprintf(stderr, "opus encode erro err: %d!\n", length);
            continue;
        }

        shared_ptr<vector<BYTE>> data = make_shared<vector<BYTE>>(length, 0);
        memcpy_s(data->data(), data->size(), tmpData.get(), length);

        if (thiz->OnAudioReady && length > 0)
        {
            thiz->OnAudioReady(data);
        }
        WaitForSingleObject(tEvent, INFINITE);
        ResetEvent(tEvent);
    }
    DeleteTimerQueueTimer(NULL, hTimer, NULL);
    CloseHandle(tEvent);
}

AudioRecorder* AudioRecorder::GetInstance()
{
    static auto a = new AudioRecorder();
    return a;
}

void AudioRecorder::SetAudioCallback(function<void(shared_ptr<vector<BYTE>> data)> callback)
{
    OnAudioReady = callback;
}

void AudioRecorder::Start()
{
    unique_lock<mutex> lck(startedMutex);
    if (!started)
    {
        started = true;
        if (capture) {

            sampleRate = capture->SamplesPerSecond();
            if (sampleRate == 44100)
            {
                sampleRate = 48000;
                needResample = true;
                int err = 0;
                resampler = src_new(SRC_SINC_FASTEST, capture->ChannelCount(), &err);
            }
            else if (sampleRate != 48000 && sampleRate != 8000 && sampleRate != 16000 && sampleRate != 32000)
            {
                fprintf(stderr, "unsupported sample rate %d\n", sampleRate);
                return;
            }

            int err = OPUS_OK;
            encoder = opus_encoder_create(sampleRate, capture->ChannelCount(), OPUS_APPLICATION_VOIP, &err);
            if (err != OPUS_OK)
            {
                fprintf(stderr, "create encoder failed.");
                encoder = nullptr;
                started = false;
                return;
            }

            if (!hTimer)
            {
                running = true;
                hTimer = new thread([this]() {TimerProc(this, false); });
            }

            capture->Start();
        }
    }
}

void AudioRecorder::Stop()
{
    unique_lock<mutex> lck(startedMutex);
    HANDLE hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (started)
    {
        started = false;
        if (hTimer)
        {
            running = false;
            hTimer->join();
            delete hTimer;
            hTimer = nullptr;
        }
        if (capture)
            capture->Stop();

        if (encoder)
        {
            opus_encoder_destroy(encoder);
            encoder = nullptr;
        }

        if (needResample)
        {
            src_delete(resampler);
            resampler = nullptr;
            needResample = false;
        }
    }
}

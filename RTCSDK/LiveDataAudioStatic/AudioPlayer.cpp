#include "AudioPlayer.h"
#include "framework.h"

#include <algorithm>

AudioPlayer::AudioPlayer():
hTimer(NULL),
renderer(nullptr),
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

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, role, &device);
    if (FAILED(hr))
    {
        fprintf(stderr, "Unable to get default device for role %d: %x\n", role, hr);
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
    unique_lock<mutex> lck(startedMutex);
    if (renderer)
    {
        if (started)
            Stop();
        renderer->Shutdown();
        renderer->Release();
    }
}

void AudioPlayer::TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	AudioPlayer* thiz = (AudioPlayer*)lpParameter;
    static const size_t sizeInByte = thiz->sampleRate * 20 * thiz->renderer->BytesPerSample() * thiz->renderer->ChannelCount() / 1000;
    static shared_ptr<BYTE> tmpOutput = shared_ptr<BYTE>(new BYTE[sizeInByte]);
    static shared_ptr<BYTE> outputData = shared_ptr<BYTE>(new BYTE[sizeInByte]);
    
    HANDLE tEvent = CreateEvent(nullptr, true, false, nullptr);
    HANDLE hTimer = NULL;
    CreateTimerQueueTimer(&hTimer, NULL, [](PVOID param, BOOLEAN) {
        SetEvent(param);
        }, tEvent, 0, 20, WT_EXECUTEDEFAULT);

    while (thiz->running)
    {
        memset(tmpOutput.get(), 0, sizeInByte);
        memset(outputData.get(), 0, sizeInByte);
        for (auto& item : thiz->mJitterBuffer)
        {
            unique_lock<mutex> lck(item.second.bufferMutex);
            if (!item.second.isReady)
                continue;
            long long uid = item.first;
            auto& userData = item.second.uidData;
            int err = OPUS_OK;
            auto iter = thiz->mDecoderHandlers.find(uid);
            if (iter == thiz->mDecoderHandlers.end())
            {
                int error = OPUS_OK;
                thiz->mDecoderHandlers.insert(make_pair(uid, opus_decoder_create(thiz->sampleRate, thiz->renderer->ChannelCount(), &error)));
                if (error != OPUS_OK)
                {
                    thiz->mDecoderHandlers.erase(uid);
                    fprintf(stderr, "create decoder error uid: %lld\n", uid);
                    break;
                }
            }

            if (userData.size() > 0)
            {
                int result = opus_decode_float(thiz->mDecoderHandlers[uid], userData.front().second.data(), userData.front().second.size(), (float*)tmpOutput.get(), thiz->sampleRate / 1000 * 20, 0);
                if (result < OPUS_OK)
                {
                    fprintf(stderr, "opus decode erro uid: %lld!\n", uid);
                }
                MixAudio((float*)outputData.get(), (float*)tmpOutput.get(), thiz->sampleRate * 20 * thiz->renderer->ChannelCount() / 1000);
                item.second.timestamp = userData.front().first;
                userData.pop_front();
            }

            if (userData.size() == 0)
            {
                item.second.isReady = false;
            }
        }

        if (thiz->renderer)
        {
            if (thiz->needResample)
            {
                SRC_DATA srcData;
                srcData.data_in = (float*)outputData.get();
                srcData.input_frames = thiz->sampleRate * 20 / 1000;
                int32_t resampleSize = sizeInByte * (thiz->renderer->SamplesPerSecond() + 1000) / thiz->sampleRate;
                static shared_ptr<BYTE> resampleData = shared_ptr<BYTE>(new BYTE[resampleSize]);
                srcData.output_frames = (thiz->renderer->SamplesPerSecond() + 1000) * 20 / 1000;
                srcData.data_out = (float*)resampleData.get();
                srcData.end_of_input = 0;
                srcData.src_ratio = double(thiz->renderer->SamplesPerSecond()) / thiz->sampleRate;
                src_process(thiz->resampler, &srcData);

                thiz->renderer->PutAudioData(resampleData.get(), srcData.output_frames_gen * thiz->renderer->BytesPerSample() * thiz->renderer->ChannelCount());
            }
            else
            {
                thiz->renderer->PutAudioData(outputData.get(), sizeInByte);
            }
        }
        WaitForSingleObject(tEvent, INFINITE);
        ResetEvent(tEvent);
    }

    DeleteTimerQueueTimer(NULL, hTimer, NULL);
    CloseHandle(tEvent);
}

AudioPlayer* AudioPlayer::GetInstance()
{
    static auto a = new AudioPlayer();
	return a;
}

void AudioPlayer::PutAudioData(long long uid, long long timestamp, char* data, size_t length)
{
    if (hTimer)
    {
        unique_lock<mutex> lck(mJitterBuffer[uid].bufferMutex);
        auto iter = upper_bound(mJitterBuffer[uid].uidData.begin(), mJitterBuffer[uid].uidData.end(), timestamp,
            [](const long long a, const pair<long long, vector<BYTE>>& b) {return a < b.first; });

        vector<BYTE> pack(length, 0);
        memcpy_s(pack.data(), pack.size(), data, length);
        mJitterBuffer[uid].uidData.emplace(iter, make_pair(timestamp, pack));

        if (mJitterBuffer[uid].uidData.size() >= 5)
        {
            mJitterBuffer[uid].isReady = true;
        }
    }
}

long long AudioPlayer::GetUserTimestamp(long long uid)
{
    auto iter = mJitterBuffer.find(uid);
    if (iter != mJitterBuffer.end())
    {
        unique_lock<mutex> lck(iter->second.bufferMutex);
        return iter->second.timestamp;
    }
    return 0;
}

void AudioPlayer::Start()
{
    unique_lock<mutex> lck(startedMutex);
    if (!started)
    {
        started = true;
        if (renderer)
        {
            sampleRate = renderer->SamplesPerSecond();
            if (sampleRate == 44100)
            {
                sampleRate = 48000;
                needResample = true;
                int err = 0;
                resampler = src_new(SRC_SINC_FASTEST, renderer->ChannelCount(), &err);
            }
            else if (sampleRate != 48000 && sampleRate != 8000 && sampleRate != 16000 && sampleRate != 32000)
            {
                fprintf(stderr, "unsupported sample rate %d\n", sampleRate);
                return;
            }

            if (!hTimer)
            {
                running = true;

                hTimer = new thread([this]() {TimerProc(this, false); });
            }
            renderer->Start();
        }
    }
}

void AudioPlayer::Stop()
{
    unique_lock<mutex> lck(startedMutex);
    HANDLE hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (started)
    {
        started = false;
        if (renderer)
            renderer->Stop();
        if (hTimer)
        {
            running = false;
            hTimer->join();
            delete hTimer;
            hTimer = nullptr;
        }

        mJitterBuffer.clear();
        for (auto item : mDecoderHandlers)
        {
            if (item.second)
                opus_decoder_destroy(item.second);
        }
        mDecoderHandlers.clear();

        if (needResample)
        {
            src_delete(resampler);
            resampler = nullptr;
            needResample = false;
        }
    }
}

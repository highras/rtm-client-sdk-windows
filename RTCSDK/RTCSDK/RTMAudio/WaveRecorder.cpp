#include "WaveRecorder.h"

#include "framework.h"
#include <samplerate.h>
#include <iostream>

WaveRecorder::WaveRecorder() :
    hTimer(NULL),
    OnAudioReady(nullptr),
    capture(nullptr),
    resampler(nullptr)
{
    IMMDeviceEnumerator* deviceEnumerator = NULL;
    IMMDeviceCollection* deviceCollection = NULL;
    IMMDevice* device = NULL;
    ERole role = eCommunications;

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
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


    capture = new AudioCapture(device, false, role);
    capture->Initialize(30);
Exit:
    SAFE_RELEASE(deviceCollection);
    SAFE_RELEASE(deviceEnumerator);
}

WaveRecorder::~WaveRecorder()
{
    {
        unique_lock<mutex> lck(startedMutex);
        if (capture)
        {
            if (started)
            {
                std::string empty;
                Stop(empty);
            }
            capture->Shutdown();
            capture->Release();
        }
    }

    CoUninitialize();
}

void WaveRecorder::TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
    WaveRecorder* thiz = (WaveRecorder*)lpParameter;
    int a = thiz->capture->SamplesPerSecond();
    int b = thiz->capture->BytesPerSample();
    size_t sizeInByte = thiz->capture->SamplesPerSecond() * 20 * thiz->capture->BytesPerSample() * thiz->capture->ChannelCount() / 1000;
    static shared_ptr<BYTE> pcmData = shared_ptr<BYTE>(new BYTE[sizeInByte]);

    thiz->duration = 0;
    HANDLE tEvent = CreateEvent(nullptr, true, false, nullptr);
    HANDLE hTimer = NULL;
    CreateTimerQueueTimer(&hTimer, NULL, [](PVOID param, BOOLEAN) {
        SetEvent(param);
        }, tEvent, 0, 20, WT_EXECUTEDEFAULT);

    while (thiz->running)
    {
        WaitForSingleObject(tEvent, INFINITE);
        ResetEvent(tEvent);
        size_t readed = 0;
        if (thiz->capture)
            readed = thiz->capture->GetAudioData(pcmData.get(), sizeInByte);
        // resample
        if (readed != sizeInByte)
            continue;

        if (thiz->capture->ChannelCount() == 2)
        {
            for (int i = 0; i != thiz->capture->SamplesPerSecond() * 20 / 1000; i++)
            {
                ((float*)(pcmData.get()))[i] = ((float*)pcmData.get())[i * 2];
            }
        }

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

            for (int i = 0; i != srcData.output_frames_gen; i++)
            {
                ((int16_t*)(pcmData.get()))[i] = INT16_MAX * (((float*)resampleData.get())[i]);
            }

        }
        else
        {
            for (int i = 0; i != thiz->sampleRate / 1000 * 20; i++)
            {
                ((int16_t*)(pcmData.get()))[i] = INT16_MAX * (((float*)pcmData.get())[i]);
            }
        }

        UINT written;
        thiz->file.Write(16 * 20 * 2, pcmData.get(), &written);
        thiz->duration += 20;

        int max = 0;
        if (thiz->OnAudioReady)
        {
            for (int i = 0; i != 16 * 20; i++)
            {
                int x = abs(((int16_t*)(pcmData.get()))[i]);
                if (max < x)
                    max = x;
            }
            double db = 20 * log10(double(max) / INT16_MAX);
            thiz->OnAudioReady(db);
        }
    }
    DeleteTimerQueueTimer(NULL, hTimer, NULL);
    CloseHandle(tEvent);
}

WaveRecorder* WaveRecorder::GetInstance()
{
    static auto a = new WaveRecorder();
    return a;
}

void WaveRecorder::SetAudioCallback(function<void(double)> callback)
{
    OnAudioReady = callback;
}

bool WaveRecorder::Start()
{
    unique_lock<mutex> lck(startedMutex);
    if (!started)
    {
        started = true;
        if (capture)
        {

            sampleRate = capture->SamplesPerSecond();
            if (sampleRate == 44100 || sampleRate == 48000 || sampleRate == 8000 || sampleRate == 32000)
            {
                sampleRate = 16000;
                needResample = true;
                int err = 0;
                resampler = src_new(SRC_SINC_FASTEST, 1, &err);
            }
            else if (sampleRate != 16000)
            {
                fprintf(stderr, "unsupported sample rate %d\n", sampleRate);
                return false;
            }

            WAVEFORMATEX wfxInput;
            ZeroMemory(&wfxInput, sizeof(wfxInput));
            wfxInput.wFormatTag = WAVE_FORMAT_PCM;
            wfxInput.nSamplesPerSec = 16000;
            wfxInput.wBitsPerSample = 16;
            wfxInput.nChannels = 1;
            wfxInput.nBlockAlign = wfxInput.nChannels * (wfxInput.wBitsPerSample / 8);
            wfxInput.nAvgBytesPerSec = wfxInput.nBlockAlign * wfxInput.nSamplesPerSec;
            //LPTSTR str = new WCHAR[32];
            //lstrcpyW(str, L"tmp.wav");
            //file.Open(str, &wfxInput, WAVEFILE_WRITE);
            HRESULT res = file.Open(NULL, &wfxInput, WAVEFILE_WRITE);
            if (FAILED(res))
            {
                DWORD err = GetLastError();
                return false;
            }

            //delete[] str;
            bool s = capture->Start();
            if (!s)
                return false;
            if (!hTimer)
            {
                running = true;
                hTimer = new thread([this]() {TimerProc(this, false); });
            }
        }
        else
        {
            started = false;
            return false;
        }
    }
    return true;
}

void WaveRecorder::Stop(std::string& audioData)
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

        if (needResample)
        {
            src_delete(resampler);
            resampler = nullptr;
            needResample = false;
        }

        {
            file.Close();
            DWORD readed = 0;
            BYTE riff[8];
            file.Read(riff, 8, &readed);
            uint32_t l = 0;
            uint32_t* length = &l;
            *length = *(uint32_t*)(riff + 4) + 8;
            char* data = new char[l];
            file.Read((BYTE*)data, l, &readed);
            file.Release();

            audioData.assign(data, l);
            delete[] data;
        }

        //DeleteFile(L"tmp.wav");
    }
}

int32_t WaveRecorder::GetDuration()
{
    return duration;
}

int32_t WaveRecorder::GetSampleRate()
{
    return sampleRate;
}


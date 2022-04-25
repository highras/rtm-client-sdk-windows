// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "AudioPlayer.h"
#include "AudioRecorder.h"

bool DisableMMCSS = false;
bool Mute = false;
void (*OnRecordDataReady)(char* data, size_t length) = nullptr;

AudioPlayer* g_ap = nullptr;
AudioRecorder* g_ar = nullptr;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


extern "C" void StartEngine()
{
    g_ar->SetAudioCallback([](shared_ptr<vector<BYTE>> data) {
        if (!Mute)
        {
            OnRecordDataReady((char*)data->data(), data->size());
        }
        });
    g_ap->Start();
    g_ar->Start();
}

extern "C" void StopEngine()
{
    g_ap->Stop();
    g_ar->Stop();
}

extern "C" void OpenMicrophone()
{
    Mute = false;
}

extern "C" void MuteMicrophone()
{
    Mute = true;
}


extern "C" void OnAudioReady(long long uid, long long seq, char* data, size_t length)
{
    g_ap->PutAudioData(uid, seq, data, length);
}

extern "C" void Init(void (*callback)(char* data, size_t length))
{
    g_ap = new AudioPlayer();
    g_ar = new AudioRecorder();
    OnRecordDataReady = callback;
}

extern "C" void Uninit()
{
    delete g_ap;
    delete g_ar;
    g_ap = nullptr;
    g_ar = nullptr;
}
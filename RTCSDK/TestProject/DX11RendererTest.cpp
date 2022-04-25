
#include <Windows.h>
#include <cstdio>
#include <thread>

extern "C" void StartEngine();
extern "C" void StopEngine();
extern "C" void Init(void (*callback)(char* data, size_t length));
extern "C" void Uninit();

int wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    AllocConsole();
    FILE* a;
    freopen_s(&a, "CONOUT$", "w", stdout);
    freopen_s(&a, "CONOUT$", "w", stderr);
    Init([](char*, size_t) {});
    StartEngine();
    StopEngine();
    Uninit();

    std::thread *th = new std::thread([]() {});
    th->join();
    th = nullptr;
    th->join();

    return 0;
}
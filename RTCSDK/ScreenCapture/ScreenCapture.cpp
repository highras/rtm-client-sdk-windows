// ScreenCapture.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ScreenCapture.h"
#include <wrl/client.h>
#include <dxgi1_6.h>
#include <d3d11.h>
#include <vector>
#include <libyuv.h>
#include <chrono>
#include <codec_api.h>
#include <list>

using namespace Microsoft::WRL;
using namespace std;


void CaptureScreen()
{
    ComPtr<IDXGIFactory1> pFactory = nullptr;

    CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));

    ComPtr<ID3D11Device> d3d11device;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;

    ISVCEncoder *encoder;
    WelsCreateSVCEncoder(&encoder);
    SEncParamBase param;
    param.fMaxFrameRate = 30;
    param.iPicHeight = 1200;
    param.iPicWidth = 1920;
    param.iTargetBitrate = 4 * 1024 * 1024;
    param.iRCMode = RC_BITRATE_MODE;
    param.iUsageType = SCREEN_CONTENT_REAL_TIME;
    encoder->Initialize(&param);

    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter)));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
                // Feature levels supported
            D3D_FEATURE_LEVEL FeatureLevels[] =
            {
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_1
            };
            UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);

            ComPtr<ID3D11DeviceContext> d3d11Context;
            D3D_FEATURE_LEVEL FeatureLevel;
            UINT flags = 0;
#ifdef _DEBUG
            flags = D3D11_CREATE_DEVICE_DEBUG;
#endif
            if (SUCCEEDED(D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, FeatureLevels, NumFeatureLevels, D3D11_SDK_VERSION, &d3d11device, &FeatureLevel, &d3d11Context)))
            {
                ComPtr<IDXGIOutput> DxgiOutput = nullptr;
                adapter->EnumOutputs(0, &DxgiOutput);
                ComPtr<IDXGIOutput1> DxgiOutput1 = nullptr;
                DxgiOutput->QueryInterface(IID_PPV_ARGS(&DxgiOutput1));
                ComPtr<IDXGIOutputDuplication> dxgioutputduplication;
                HRESULT hr = DxgiOutput1->DuplicateOutput(d3d11device.Get(), &dxgioutputduplication);
                if (FAILED(hr))
                    return;
                ComPtr<IDXGIResource> DesktopResource = nullptr;
                ComPtr<ID3D11Texture2D> buffertexture;
                DXGI_OUTDUPL_FRAME_INFO FrameInfo;
                bool first = true;

                HANDLE hFile = CreateFile(L"1.264", GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

                list<char*> frames;
                auto last = std::chrono::steady_clock::now();

                for (int i = 0; i != 1000; i++)
                {
                    ComPtr<ID3D11Texture2D> d3d11texture;
                    dxgioutputduplication->ReleaseFrame();
                    auto tp1 = std::chrono::steady_clock::now();
                    printf("fps:%f\n", 1000000000.0 / (tp1 - last).count());
                    last = tp1;
                    hr = dxgioutputduplication->AcquireNextFrame(1000/30, &FrameInfo, &DesktopResource);
                    if (FAILED(hr))
                    {
                        printf("timeout\n");
                        continue;
                    }
                    DesktopResource->QueryInterface(IID_PPV_ARGS(&d3d11texture));

                    D3D11_TEXTURE2D_DESC desc;
                    d3d11texture->GetDesc(&desc);

                    DWORD dataSize = ((desc.Width * 32 + 31) / 32) * 4 * desc.Height;
                    char* data = new char[dataSize];

                    if (first)
                    {
                        desc.Usage = D3D11_USAGE_STAGING;
                        desc.BindFlags = 0;
                        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                        desc.MiscFlags = 0;
                        hr = d3d11device->CreateTexture2D(&desc, NULL, &buffertexture);
                        first = false;
                        dxgioutputduplication->ReleaseFrame();
                        Sleep(1000 / 30);
                        continue;
                    }
                    D3D11_MAPPED_SUBRESOURCE outdata;
                    d3d11Context->CopyResource(buffertexture.Get(), d3d11texture.Get());
                    hr = d3d11Context->Map(buffertexture.Get(), 0, D3D11_MAP_READ, 0, &outdata);

                    for (int i = 0; i != desc.Height; i++)
                    {
                        memcpy_s(data + desc.Width * 4 * i, desc.Width * 4, ((char*)outdata.pData) + outdata.RowPitch * i, desc.Width * 4);
                    }

                    d3d11Context->Unmap(buffertexture.Get(), 0);
                    frames.emplace_back(data);
                    //Sleep(1000 / 30);
                }

                int width = 1920;
                int height = 1200;
                for (auto data : frames)
                {
                    static unsigned char* i420y = new unsigned char[(width + 15) / 16 * 16 * height];
                    static unsigned char* i420u = new unsigned char[(width + 15) / 16 * 16 * height / 4];
                    static unsigned char* i420v = new unsigned char[(width + 15) / 16 * 16 * height / 4];
                    libyuv::ARGBToI420((unsigned char*)data, width * 4, i420y, (width + 15) / 16 * 16, i420u, (width + 15) / 16 * 8, i420v, (width + 15) / 16 * 8, width, height);
                    SSourcePicture pic;
                    pic.iPicWidth = width;
                    pic.iPicHeight = height;
                    pic.iColorFormat = videoFormatI420;
                    pic.iStride[0] = (width + 15) / 16 * 16;
                    pic.iStride[1] = (width + 15) / 16 * 8;
                    pic.iStride[2] = (width + 15) / 16 * 8;
                    pic.iStride[3] = 0;
                    pic.uiTimeStamp = std::chrono::steady_clock::now().time_since_epoch().count() / 1000000;
                    pic.pData[0] = (unsigned char*)i420y;
                    pic.pData[1] = (unsigned char*)i420u;
                    pic.pData[2] = (unsigned char*)i420v;
                    pic.pData[3] = nullptr;

                    SFrameBSInfo fbsi;

                    int result = encoder->EncodeFrame(&pic, &fbsi);
                    for (int j = 0; j != fbsi.iLayerNum; j++)
                    {
                        for (int k = 0; k != fbsi.sLayerInfo[j].iNalCount; k++)
                        {
                            DWORD written;
                            int ret = WriteFile(hFile, fbsi.sLayerInfo[j].pBsBuf, fbsi.sLayerInfo[j].pNalLengthInByte[k], &written, nullptr);
                            fbsi.sLayerInfo[j].pBsBuf += fbsi.sLayerInfo[j].pNalLengthInByte[k];
                        }
                    }
                    delete[] data;
                }
                CloseHandle(hFile);
            }
        }
    }
    WelsDestroySVCEncoder(encoder);
}
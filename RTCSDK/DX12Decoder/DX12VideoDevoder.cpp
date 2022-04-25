#include "DXSampleHelper.h"
#include <dxgi.h>
#include <dxgi1_6.h>


void GetHardwareAdapter(
    IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter,
    bool requestHighPerformanceAdapter = false)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
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
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if (adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
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
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

void DX12Decoderinit()
{
	ComPtr<ID3D12VideoDecoder> d3d12decoder;
	D3D12_VIDEO_DECODER_DESC d3d12decoderdesc = {};
	d3d12decoderdesc.Configuration.BitstreamEncryption = D3D12_BITSTREAM_ENCRYPTION_TYPE_NONE;
    d3d12decoderdesc.Configuration.DecodeProfile = D3D12_VIDEO_DECODE_PROFILE_H264;
    d3d12decoderdesc.Configuration.InterlaceType = D3D12_VIDEO_FRAME_CODED_INTERLACE_TYPE_NONE;
	d3d12decoderdesc.NodeMask = 0;
    UINT dxgiFactoryFlags = 0;
    bool m_useWarpDevice = false;
    ID3D12Device* device = nullptr;
    ID3D12VideoDevice* vdevice = nullptr;
#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&device)
        ));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);
        
        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&device)
        ));
    }

    device->QueryInterface(IID_PPV_ARGS(&vdevice));

    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE, IID_PPV_ARGS(&m_commandAllocator)));

    ThrowIfFailed(vdevice->CreateVideoDecoder(&d3d12decoderdesc, IID_PPV_ARGS(&d3d12decoder)));
    ComPtr<ID3D12VideoDecodeCommandList> commandlist;
    ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandlist)));
    D3D12_VIDEO_DECODE_OUTPUT_STREAM_ARGUMENTS outputArgs;
    outputArgs.pOutputTexture2D = nullptr;
    outputArgs.OutputSubresource = 0;
    outputArgs.ConversionArguments.Enable = false;
    D3D12_VIDEO_DECODE_INPUT_STREAM_ARGUMENTS inputArgs;
    inputArgs.CompressedBitstream.pBuffer = nullptr;
    inputArgs.CompressedBitstream.Offset = 0;
    inputArgs.CompressedBitstream.Size = 0;
    inputArgs.pHeap = nullptr;
    inputArgs.FrameArguments[0].pData = 0;
    inputArgs.NumFrameArguments = 1;
    inputArgs.ReferenceFrames;
    commandlist->DecodeFrame(d3d12decoder.Get(), &outputArgs, &inputArgs);

    
}
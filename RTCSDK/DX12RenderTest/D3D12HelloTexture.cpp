//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "D3D12HelloTexture.h"

#include <chrono>
#include <thread>

#include "MFTDecoder.h"
#include "sharders.h"

extern bool running;

D3D12HelloTexture::D3D12HelloTexture(UINT width, UINT height, std::wstring name, HWND hwnd) :
    DXSample(width, height, name, hwnd),
    m_frameIndex(0),
    m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
    m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
    m_rtvDescriptorSize(0),
    m_srvDescriptorSize(0)
{
}

void D3D12HelloTexture::OnInit()
{
    LoadPipeline();
    LoadAssets();
}

// Load the rendering pipeline dependencies.
void D3D12HelloTexture::LoadPipeline()
{
    UINT dxgiFactoryFlags = 0;

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
            IID_PPV_ARGS(&m_device)
            ));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
            ));
    }

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        m_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
        ));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(factory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

        // Describe and create a shader resource view (SRV) heap for the texture.
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 3;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_srvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV for each frame.
        for (UINT n = 0; n < FrameCount; n++)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
            m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }

    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
}

// Load the sample assets.
void D3D12HelloTexture::LoadAssets()
{
    decoder = new MFTDecoder();
    decoder->Init(L"2.mp4");
    TextureWidth = 640;
    TextureHeight = 480;
    // Create the root signature.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1] = {};
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        //ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_ROOT_PARAMETER1 rootParameters[1] = {};
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
        //rootParameters[1].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 16;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif
        ThrowIfFailed(D3DCompile(VertexSharderStr, strlen(VertexSharderStr), "VertexSharder", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompile(PixelSharderStr, strlen(PixelSharderStr), "PixelSharder", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        //ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        //ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
    }

    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

    // Create the vertex buffer.
    {
        Vertex rectVertices[] =
        {
            { { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
            { { 1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
            { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
            { { -1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } }
        };

        const UINT vertexBufferSize = sizeof(rectVertices);

        // Note: using upload heaps to transfer static data like vert buffers is not 
        // recommended. Every time the GPU needs it, the upload heap will be marshalled 
        // over. Please read up on Default Heap usage. An upload heap is used here for 
        // code simplicity and because there are very few verts to actually transfer.
        auto cd3dx12properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto cd3dx12buffer = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
        ThrowIfFailed(m_device->CreateCommittedResource(
            &cd3dx12properties,
            D3D12_HEAP_FLAG_NONE,
            &cd3dx12buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_vertexBuffer)));

        // Copy the triangle data to the vertex buffer.
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, rectVertices, sizeof(rectVertices));
        m_vertexBuffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = vertexBufferSize;

        unsigned short indices[] = {0, 1, 2, 3, 0};

        cd3dx12properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        cd3dx12buffer = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indices));
        ThrowIfFailed(m_device->CreateCommittedResource(
            &cd3dx12properties,
            D3D12_HEAP_FLAG_NONE,
            &cd3dx12buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_indeciesBuffer)));

        ThrowIfFailed(m_indeciesBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, indices, sizeof(indices));
        m_indeciesBuffer->Unmap(0, nullptr);

        m_indeciesBufferView.BufferLocation = m_indeciesBuffer->GetGPUVirtualAddress();
        m_indeciesBufferView.SizeInBytes = sizeof(indices);
        m_indeciesBufferView.Format = DXGI_FORMAT_R16_UINT;
    }

    // Note: ComPtr's are CPU objects but this resource needs to stay in scope until
    // the command list that references it has finished executing on the GPU.
    // We will flush the GPU at the end of this method to ensure the resource is not
    // prematurely destroyed.

    // Create the texture.
    {
        // Describe and create a Texture2D.
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.MipLevels = 1;
        textureDesc.Format = DXGI_FORMAT_R8_UNORM;
        textureDesc.Width = TextureWidth;
        textureDesc.Height = TextureHeight;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        textureDesc.DepthOrArraySize = 1;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        auto cd3dx12properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        ThrowIfFailed(m_device->CreateCommittedResource(
            &cd3dx12properties,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_textureY)));

        textureDesc.Width = TextureWidth/2;
        textureDesc.Height = TextureHeight/2;

        ThrowIfFailed(m_device->CreateCommittedResource(
            &cd3dx12properties,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_textureU)));

        ThrowIfFailed(m_device->CreateCommittedResource(
            &cd3dx12properties,
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_textureV)));

        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_textureY.Get(), 0, 1);

        cd3dx12properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto cd3dx12buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        // Create the GPU upload buffer.
        ThrowIfFailed(m_device->CreateCommittedResource(
            &cd3dx12properties,
            D3D12_HEAP_FLAG_NONE,
            &cd3dx12buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_textureUploadHeapY)));
        cd3dx12buffer = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(m_textureU.Get(), 0, 1));
        // Create the GPU upload buffer.
        ThrowIfFailed(m_device->CreateCommittedResource(
            &cd3dx12properties,
            D3D12_HEAP_FLAG_NONE,
            &cd3dx12buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_textureUploadHeapU)));
        cd3dx12buffer = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(m_textureV.Get(), 0, 1));
        // Create the GPU upload buffer.
        ThrowIfFailed(m_device->CreateCommittedResource(
            &cd3dx12properties,
            D3D12_HEAP_FLAG_NONE,
            &cd3dx12buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_textureUploadHeapV)));

        // Copy data to the intermediate upload heap and then schedule a copy 
        // from the upload heap to the Texture2D.
        std::vector<UINT8> texture = GenerateTextureData();

        D3D12_SUBRESOURCE_DATA textureData[3] = { {},{},{} };
        textureData[0].pData = texture.data();
        textureData[0].RowPitch = TextureWidth;//TextureWidth * TexturePixelSize;
        textureData[0].SlicePitch = textureData[0].RowPitch * TextureHeight;

        textureData[1].pData = texture.data()+ TextureWidth * TextureHeight;
        textureData[1].RowPitch = TextureWidth / 2;//TextureWidth * TexturePixelSize;
        textureData[1].SlicePitch = textureData[1].RowPitch * TextureHeight/2;

        textureData[2].pData = texture.data() + TextureWidth * TextureHeight*5/4;
        textureData[2].RowPitch = TextureWidth / 2;//TextureWidth * TexturePixelSize;
        textureData[2].SlicePitch = textureData[2].RowPitch * TextureHeight / 2;

        UpdateSubresources(m_commandList.Get(), m_textureY.Get(), m_textureUploadHeapY.Get(), 0, 0, 1, &textureData[0]);
        UpdateSubresources(m_commandList.Get(), m_textureU.Get(), m_textureUploadHeapU.Get(), 0, 0, 1, &textureData[1]);
        UpdateSubresources(m_commandList.Get(), m_textureV.Get(), m_textureUploadHeapV.Get(), 0, 0, 1, &textureData[2]);
        auto transmition = CD3DX12_RESOURCE_BARRIER::Transition(m_textureY.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        m_commandList->ResourceBarrier(1, &transmition);
        transmition = CD3DX12_RESOURCE_BARRIER::Transition(m_textureU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        m_commandList->ResourceBarrier(1, &transmition);
        transmition = CD3DX12_RESOURCE_BARRIER::Transition(m_textureV.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        m_commandList->ResourceBarrier(1, &transmition);

        // Describe and create a SRV for the texture. RGB888
        //D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        //srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        //srvDesc.Format = textureDesc.Format;
        //srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        //srvDesc.Texture2D.MipLevels = 1;
        //m_device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srvHeap->GetCPUDescriptorHandleForHeapStart());


        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandler(m_srvHeap->GetCPUDescriptorHandleForHeapStart(), 0, m_srvDescriptorSize);
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_R8_UNORM;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.PlaneSlice = 0;
        m_device->CreateShaderResourceView(m_textureY.Get(), &srvDesc, cpuHandler);
        cpuHandler.Offset(1, m_srvDescriptorSize);
        m_device->CreateShaderResourceView(m_textureU.Get(), &srvDesc, cpuHandler);
        cpuHandler.Offset(1, m_srvDescriptorSize);
        m_device->CreateShaderResourceView(m_textureV.Get(), &srvDesc, cpuHandler);
    }
    
    // Close the command list and execute it to begin the initial GPU setup.
    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue = 1;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForPreviousFrame();
    }
}

// Generate a simple black and white checkerboard texture.
std::vector<UINT8> D3D12HelloTexture::GenerateTextureData()
{
    //decoder = new MFDecoder();
    //const UINT rowPitch = TextureWidth * TexturePixelSize;
    //const UINT cellPitch = rowPitch >> 3;        // The width of a cell in the checkboard texture.
    //const UINT cellHeight = TextureWidth >> 3;    // The height of a cell in the checkerboard texture.
    //const UINT textureSize = rowPitch * TextureHeight;
    //const UINT textureSize = TextureWidth * TextureHeight *3/2;

    //std::vector<UINT8> data(textureSize);
    //UINT8* pData = data.data();

    //UINT n = 0;
    //for (n = 0; n < TextureWidth * TextureHeight; n += 1)
    //{
    //    pData[n] = rand() % 0xFF;// Y
    //}

    //for (; n < textureSize; n += 2)
    //{
    //    pData[n] = 128; //rand() % 0xFF;// U
    //    pData[n + 1] = 128;// rand() % 0xFF;// V
    //}

    std::vector<UINT8> data = decoder->DecodeOpenH264();
    //textureMutex.lock();
    //textures.emplace_back(std::move(data));
    //textureMutex.unlock();

    std::thread a([this]() {
        while (running)
        {
            static auto time1 = std::chrono::high_resolution_clock::now();
            auto time2 = std::chrono::high_resolution_clock::now();
            //printf("a generate fps:%f %lld\n", 1000000000.0f / (time2 - time1).count(), (time2 - time1).count());
            time1 = time2;
            UpdateTextureData();
            Sleep(15);
        }
        });

    a.detach();

    return data;
}

void D3D12HelloTexture::UpdateTextureData()
{
    //const UINT textureSize = TextureWidth * TextureHeight * 3 / 2;

    //std::vector<UINT8> data(textureSize);
    //UINT8* pData = data.data();

    //UINT n = 0;
    //for (n = 0; n < TextureWidth * TextureHeight; n += 1)
    //{
    //    pData[n] = rand() % 0xFF;// Y
    //}

    //for (; n < textureSize; n += 2)
    //{
    //    pData[n] = 128; //rand() % 0xFF;// U
    //    pData[n + 1] = 128;// rand() % 0xFF;// V
    //}

    std::vector<UINT8> data = decoder->DecodeOpenH264();
    if (data.size() == 0)
        return;

    textureMutex.lock();
    textures.emplace_back(data);
    textureMutex.unlock();
}

// Update frame-based values.
void D3D12HelloTexture::OnUpdate()
{
    static auto time1 = std::chrono::high_resolution_clock::now();
    auto time2 = std::chrono::high_resolution_clock::now();
    printf("fps:%f duration:%lldms\n",1000000000.0f/(time2-time1).count(), (time2 - time1).count()/ 1000000);
    time1 = time2;
}

// Render the scene.
void D3D12HelloTexture::OnRender()
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    ThrowIfFailed(m_swapChain->Present(1, 0));

    WaitForPreviousFrame();
}

void D3D12HelloTexture::OnDestroy()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForPreviousFrame();

    m_textureY->Release();
    m_textureU->Release();
    m_textureV->Release();
    m_textureUploadHeapY->Release();
    m_textureUploadHeapU->Release();
    m_textureUploadHeapV->Release();
    m_swapChain->Release();
    for (auto& item : m_renderTargets)
    {
        item->Release();
    }
    m_commandAllocator->Release();
    m_commandQueue->Release();
    m_rootSignature->Release();
    m_rtvHeap->Release();
    m_srvHeap->Release();
    m_pipelineState->Release();
    m_commandList->Release();
    m_vertexBuffer->Release();
    m_indeciesBuffer->Release();
    m_fence->Release();
    m_device->Release();

    CloseHandle(m_fenceEvent);
}

void D3D12HelloTexture::PopulateCommandList()
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    ThrowIfFailed(m_commandAllocator->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

    {
        textureMutex.lock();
        if (textures.size() > 0)
        {
            auto trans = CD3DX12_RESOURCE_BARRIER::Transition(m_textureY.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
            m_commandList->ResourceBarrier(1, &trans);
            trans = CD3DX12_RESOURCE_BARRIER::Transition(m_textureU.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
            m_commandList->ResourceBarrier(1, &trans);
            trans = CD3DX12_RESOURCE_BARRIER::Transition(m_textureV.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
            m_commandList->ResourceBarrier(1, &trans);
            std::vector<UINT8> texture = textures.front();
            textures.pop_front();
            textureMutex.unlock();
            D3D12_SUBRESOURCE_DATA textureData[3] = { {},{},{} };
            textureData[0].pData = texture.data();
            textureData[0].RowPitch = TextureWidth;// TextureWidth* TexturePixelSize;
            textureData[0].SlicePitch = textureData[0].RowPitch * TextureHeight;
            textureData[1].pData = texture.data() + TextureWidth * TextureHeight;
            textureData[1].RowPitch = TextureWidth / 2;//TextureWidth * TexturePixelSize;
            textureData[1].SlicePitch = textureData[1].RowPitch * TextureHeight / 2;
            textureData[2].pData = texture.data() + TextureWidth * TextureHeight*5/4;
            textureData[2].RowPitch = TextureWidth / 2;//TextureWidth * TexturePixelSize;
            textureData[2].SlicePitch = textureData[2].RowPitch * TextureHeight / 2;

            UpdateSubresources(m_commandList.Get(), m_textureY.Get(), m_textureUploadHeapY.Get(), 0, 0, 1, &textureData[0]);
            UpdateSubresources(m_commandList.Get(), m_textureU.Get(), m_textureUploadHeapU.Get(), 0, 0, 1, &textureData[1]);
            UpdateSubresources(m_commandList.Get(), m_textureV.Get(), m_textureUploadHeapV.Get(), 0, 0, 1, &textureData[2]);
            
            trans = CD3DX12_RESOURCE_BARRIER::Transition(m_textureY.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            m_commandList->ResourceBarrier(1, &trans);
            trans = CD3DX12_RESOURCE_BARRIER::Transition(m_textureU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            m_commandList->ResourceBarrier(1, &trans);
            trans = CD3DX12_RESOURCE_BARRIER::Transition(m_textureV.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            m_commandList->ResourceBarrier(1, &trans);
        }
        else
        {
            textureMutex.unlock();
        }
    }

    // Set necessary state.
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* ppHeaps[] = { m_srvHeap.Get()};
    m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(m_srvHeap->GetGPUDescriptorHandleForHeapStart(), 0, m_srvDescriptorSize);
    m_commandList->SetGraphicsRootDescriptorTable(0, srvHandle);
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    // Indicate that the back buffer will be used as a render target.
    auto transmitions = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &transmitions);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->IASetIndexBuffer(&m_indeciesBufferView);
    m_commandList->DrawIndexedInstanced(5, 1, 0, 0, 0);

    // Indicate that the back buffer will now be used to present.
    transmitions = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &transmitions);

    ThrowIfFailed(m_commandList->Close());
}

void D3D12HelloTexture::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

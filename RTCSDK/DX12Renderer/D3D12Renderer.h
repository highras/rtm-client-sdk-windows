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

#pragma once

#include "DXRenderer.h"
#include <mutex>
#include <list>

using namespace DirectX;

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().
using Microsoft::WRL::ComPtr;

class D3D12Renderer : public DXRenderer
{
public:
    D3D12Renderer(UINT width, UINT height, HWND hwnd);

    virtual void OnInit() override;
    virtual void OnDestroy() override;
    void DrawFrame(const std::vector<BYTE>& frameData);

    void ChangeTextureSize(UINT width, UINT height);

private:
    static const UINT FrameCount = 2;
    UINT TextureWidth = 320;
    UINT TextureHeight = 240;
    const std::vector<BYTE>* texture;

    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT2 uv;
    };

    Vertex rectVertices[4] =
    {
        { { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
        { { 1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
        { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
        { { -1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } }
    };

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ComPtr<ID3D12Resource> m_textureUploadHeapY;
    ComPtr<ID3D12Resource> m_textureUploadHeapU;
    ComPtr<ID3D12Resource> m_textureUploadHeapV;
    UINT m_rtvDescriptorSize;
    UINT m_srvDescriptorSize;

    // App resources.
    ComPtr<ID3D12Resource> m_vertexBuffer;
    ComPtr<ID3D12Resource> m_indeciesBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW m_indeciesBufferView;
    ComPtr<ID3D12Resource> m_textureY;
    ComPtr<ID3D12Resource> m_textureU;
    ComPtr<ID3D12Resource> m_textureV;

    // Synchronization objects.
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    void LoadPipeline();
    void LoadAssets();
    void PopulateCommandList();
    void WaitForPreviousFrame();

    void RotateTextureClockwise();
    void RotateTextureAnticlockwise();
    void FlipHorizontal();

    virtual void OnUpdate() override;
    virtual void OnRender() override;
};

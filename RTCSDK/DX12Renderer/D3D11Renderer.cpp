#include "D3D11Renderer.h"
#include "sharders.h"

D3D11Renderer::D3D11Renderer(UINT width, UINT height, HWND hwnd):
    DXRenderer(width, height, hwnd)
{
}

void D3D11Renderer::OnInit()
{
    UINT dxgiFactoryFlags = 0;
    UINT dx11Flags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    // Enable additional debug layers.
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    dx11Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL FeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };
    UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);
    D3D_FEATURE_LEVEL FeatureLevel;

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
        ThrowIfFailed(D3D11CreateDevice(warpAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, dx11Flags, FeatureLevels, NumFeatureLevels, D3D11_SDK_VERSION, &m_device, &FeatureLevel, &m_context));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);
        ThrowIfFailed(D3D11CreateDevice(hardwareAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, dx11Flags, FeatureLevels, NumFeatureLevels, D3D11_SDK_VERSION, &m_device, &FeatureLevel, &m_context));
    }

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
        m_device.Get(),        // Swap chain needs the queue so that it can force a flush on it.
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

    D3D11_TEXTURE2D_DESC textureDesc;
    RtlZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
    textureDesc.Width = TextureWidth;
    textureDesc.Height = TextureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textureDesc.MiscFlags = 0;
    ThrowIfFailed(m_device->CreateTexture2D(&textureDesc, nullptr, &m_textureY));

    textureDesc.Width = TextureWidth / 2;
    textureDesc.Height = TextureHeight / 2;
    ThrowIfFailed(m_device->CreateTexture2D(&textureDesc, nullptr, &m_textureU));
    ThrowIfFailed(m_device->CreateTexture2D(&textureDesc, nullptr, &m_textureV));

    ComPtr<ID3D11Texture2D> BackBuffer = nullptr;
    ThrowIfFailed(m_swapChain->GetBuffer(0,IID_PPV_ARGS(&BackBuffer)));

    // Create a render target view
    ThrowIfFailed(m_device->CreateRenderTargetView(BackBuffer.Get(), nullptr, &m_renderTargetView));

    // Set new render target
    m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

    D3D11_VIEWPORT VP;
    VP.Width = static_cast<FLOAT>(m_width);
    VP.Height = static_cast<FLOAT>(m_height);
    VP.MinDepth = 0.0f;
    VP.MaxDepth = 1.0f;
    VP.TopLeftX = 0;
    VP.TopLeftY = 0;
    m_context->RSSetViewports(1, &VP);

    // Create the sample state
    D3D11_SAMPLER_DESC SampDesc;
    RtlZeroMemory(&SampDesc, sizeof(SampDesc));
    SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SampDesc.MinLOD = 0;
    SampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    ThrowIfFailed(m_device->CreateSamplerState(&SampDesc, &m_samplerLinear));

    // Create the blend state
    D3D11_BLEND_DESC BlendStateDesc;
    BlendStateDesc.AlphaToCoverageEnable = FALSE;
    BlendStateDesc.IndependentBlendEnable = FALSE;
    BlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
    BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    ThrowIfFailed(m_device->CreateBlendState(&BlendStateDesc, &m_blendState));

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

    ThrowIfFailed(m_device->CreateVertexShader(vertexShader->GetBufferPointer(), vertexShader->GetBufferSize(), nullptr, &m_vertexShader));
    ThrowIfFailed(m_device->CreatePixelShader(pixelShader->GetBufferPointer(), pixelShader->GetBufferSize(), nullptr, &m_pixelShader));

    D3D11_INPUT_ELEMENT_DESC Layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    UINT NumElements = ARRAYSIZE(Layout);
    ThrowIfFailed(m_device->CreateInputLayout(Layout, NumElements, vertexShader->GetBufferPointer(), vertexShader->GetBufferSize(), &m_inputLayout));
    m_context->IASetInputLayout(m_inputLayout.Get());
}

void D3D11Renderer::OnDestroy()
{
}

void D3D11Renderer::DrawFrame(const std::vector<BYTE>& frameData)
{
}

void D3D11Renderer::ChangeTextureSize(UINT width, UINT height)
{
}

void D3D11Renderer::OnUpdate()
{
}

void D3D11Renderer::OnRender()
{
}

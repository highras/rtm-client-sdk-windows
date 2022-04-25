#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "DXRenderer.h"

using namespace Microsoft::WRL;
class D3D11Renderer : public DXRenderer
{
public:
    D3D11Renderer(UINT width, UINT height, HWND hwnd);

    virtual void OnInit() override;
    virtual void OnDestroy() override;
    void DrawFrame(const std::vector<BYTE>& frameData);

    void ChangeTextureSize(UINT width, UINT height);
private:
    static const UINT FrameCount = 2;
    UINT TextureWidth = 320;
    UINT TextureHeight = 240;

    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<ID3D11Texture2D> m_textureY;
    ComPtr<ID3D11Texture2D> m_textureU;
    ComPtr<ID3D11Texture2D> m_textureV;

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<ID3D11SamplerState> m_samplerLinear;
    ComPtr<ID3D11BlendState> m_blendState;
    UINT m_frameIndex;

    virtual void OnUpdate() override;
    virtual void OnRender() override;
};


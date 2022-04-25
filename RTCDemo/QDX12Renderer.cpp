#include "QDX12Renderer.h"

QDX12Renderer::QDX12Renderer(QWidget* parent):
    QWidget(parent),
    renderer(nullptr)
{
}

QDX12Renderer::~QDX12Renderer()
{
    if (renderer)
    {
        delete renderer;
        renderer = nullptr;
    }
}

void QDX12Renderer::OnInit()
{
    WCHAR objname[255];
    objectName().toWCharArray(objname);
    renderer = new D3D12Renderer(this->height(), this->width(), (HWND)this->winId());
    renderer->OnInit();
    renderer->ChangeTextureSize(640,480); 
}


void QDX12Renderer::DrawFrame(const std::vector<unsigned char>& frameData)
{
    if (renderer)
        renderer->DrawFrame(frameData);
}

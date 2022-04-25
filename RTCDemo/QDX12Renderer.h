#pragma once
#include <QWidget>
#include <D3D12Renderer.h>
class QDX12Renderer: public QWidget
{
	Q_OBJECT

public:
	QDX12Renderer(QWidget* parent = nullptr);
	~QDX12Renderer();
    void OnInit();
    void DrawFrame(const std::vector<unsigned char>& frameData);
private:
    D3D12Renderer* renderer;
};


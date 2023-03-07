#include "Game.pch.h"
#include "CanvasPS.h"

CanvasPS::CanvasPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/CanvasPS.cso", ios::binary);
	string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading canvas pixel shader failed!");
	psFile.close();

	myPainting.CreateEmptyTexture(DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 2048, 2048);
}

void CanvasPS::SetResource()
{
	myPainting.SetAsResource(2);
}

void CanvasPS::Paint(int aPosX, int aPosY, float aRadius)
{
}

void CanvasPS::Clear()
{
	DX11::Context->ClearRenderTargetView(myPainting.GetRenderTargetView().Get(), myClearColor);
}

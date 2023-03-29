#include "Game.pch.h"
#include "CanvasPS.h"
#include "Canvas.h"


CanvasPS::CanvasPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/CanvasPS.cso", ios::binary);
	string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading canvas pixel shader failed!");
	psFile.close();

	myPaintBuffer.Initialize();
}


void CanvasPS::SetResource()
{
	Canvas::GetInstance()->GetPaintingTex().SetAsResource(0);
	Canvas::GetInstance()->GetScreenTex().SetAsResource(1);
	Canvas::GetInstance()->GetPaperTex().SetAsResource(2);

	const float lerpTime = 0.15f;
	float t = Canvas::GetInstance()->GetTimeSincePaintStart();
	myPaintBufferData.paperTexPercent = Catbox::Clamp(t / lerpTime, 0.f, 1.f);
	myPaintBuffer.SetData(&myPaintBufferData);
	DX11::Context->PSSetConstantBuffers(10, 1, myPaintBuffer.GetAddress());
}
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
}
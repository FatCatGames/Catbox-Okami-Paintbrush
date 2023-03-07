#include "Game.pch.h"
#include "CanvasPS.h"

CanvasPS::CanvasPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/CanvasPS.cso", ios::binary);
	string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading canvas pixel shader failed!");
	myPainting.CreateEmptyTexture(DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
	Clear();
	psFile.close();
}


void CanvasPS::SetResource()
{
	myPainting.SetAsResource(0);
}

void CanvasPS::Paint(int aPosX, int aPosY, int aRadius)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = DX11::Context->Map(myPainting.GetTex().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	BYTE* pData = reinterpret_cast<BYTE*>(mappedResource.pData);
	const UINT rowPitch = mappedResource.RowPitch;

	const int startY = Catbox::Clamp(aPosY - aRadius, 0, 1024);
	const int endY = Catbox::Clamp(aPosY + aRadius, 0, 1024);
	const int startX = Catbox::Clamp(aPosX - aRadius, 0, 1024);
	const int endX = Catbox::Clamp(aPosX + aRadius, 0, 1024);
	const int radiusSquared = aRadius * aRadius;


	for (int y = startY; y < endY; ++y)
	{
		for (int x = startX; x < endX; ++x)
		{
			const int distanceX = x - aPosX;
			const int distanceY = y - aPosY;
			const int distanceSquared = distanceX * distanceX + distanceY * distanceY;

			// Check if the current pixel is inside the circle
			if (distanceSquared <= radiusSquared)
			{
				// Calculate the index of the current pixel in the texture data array
				const UINT pixelIndex = y * rowPitch + x * 4;

				// Set the color of the current pixel to black
				pData[pixelIndex] = 255;   // R
				pData[pixelIndex + 1] = 0; // G
				pData[pixelIndex + 2] = 0; // B
				pData[pixelIndex + 3] = 255; // B
			}
		}
	}


	DX11::Context->Unmap(myPainting.GetTex().Get(), 0);
}

void CanvasPS::Clear()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = DX11::Context->Map(myPainting.GetTex().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	BYTE* pData = reinterpret_cast<BYTE*>(mappedResource.pData);
	const UINT rowPitch = mappedResource.RowPitch;
	for (int y = 0; y < 1024; ++y)
	{
		for (int x = 0; x < 1024; ++x)
		{
			//Make all pixels white again
			const UINT pixelIndex = y * rowPitch + x * 4;
			pData[pixelIndex] = 255;     // R
			pData[pixelIndex + 1] = 255; // G
			pData[pixelIndex + 2] = 255; // B
			pData[pixelIndex + 3] = 255; // A

		}
	}
	DX11::Context->Unmap(myPainting.GetTex().Get(), 0);
}

#include "Game.pch.h"
#include "Canvas.h"
#include "CanvasPS.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Material.h"
#include "OpenCV\GenData.h"
#include "PopupManager.h"
#include "Graphics\Rendering\Buffers\Buffers.h"

Canvas* Canvas::Instance;
Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture;
Texture tempTex;

Canvas::Canvas()
{
	Instance = this;

	myCanvasPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/ScreenCanvasPS.sh");
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/ScreenCanvasPS.cso", ios::binary);
	string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myCanvasPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading canvas pixel shader failed!");

	myPaintingTex.CreateEmptyTexture(DXGI_FORMAT_R8G8B8A8_UNORM, myWidth, myHeight, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
	tempTex.CreateEmptyTexture(DXGI_FORMAT_R8G8B8A8_UNORM, myWidth, myHeight, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

	myScreenTex.CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT);
	myScreenTex.CreateRenderTargetView();
	//myStagingTex.CreateEmptyTexture(DXGI_FORMAT_R8G8B8A8_UNORM, myWidth, myHeight, 1, 0, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_STAGING);

	// Create a staging texture with default usage and copy the painting texture to it
	D3D11_TEXTURE2D_DESC stagingDesc = {};
	stagingDesc.Width = myWidth;
	stagingDesc.Height = myHeight;
	stagingDesc.MipLevels = 1;
	stagingDesc.ArraySize = 1;
	stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	stagingDesc.SampleDesc.Count = 1;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	DX11::Device->CreateTexture2D(&stagingDesc, nullptr, stagingTexture.GetAddressOf());

	Clear();

	myPostRenderListener.action = [this] {Render(); };
	GraphicsEngine::GetInstance()->AddPostRenderListener(myPostRenderListener);
}

Canvas::~Canvas()
{
	GraphicsEngine::GetInstance()->GetRenderingDoneEvent().RemoveListener(myPostRenderListener);
}

void Canvas::Awake()
{
	myPaperTex = AssetRegistry::GetInstance()->GetAsset<Texture>("PaperTex");
	//myShader = dynamic_cast<CanvasPS*>(myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetPixelShader().get());
}


void Canvas::Paint(int anXPos, int anYPos, int aRadius, const Color& aColor)
{
	DX11::Context->CopyResource(stagingTexture.Get(), myPaintingTex.GetTex().Get());

	// Map the staging texture
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = DX11::Context->Map(stagingTexture.Get(), 0, D3D11_MAP_WRITE, 0, &mappedResource);
	BYTE* pData = reinterpret_cast<BYTE*>(mappedResource.pData);
	const UINT rowPitch = mappedResource.RowPitch;

	// Paint on the staging texture
	const int startY = Catbox::Clamp(anYPos - aRadius, 0, myHeight);
	const int endY = Catbox::Clamp(anYPos + aRadius, 0, myHeight);
	const int startX = Catbox::Clamp(anXPos - aRadius, 0, myWidth);
	const int endX = Catbox::Clamp(anXPos + aRadius, 0, myWidth);
	const int radiusSquared = aRadius * aRadius;

	for (int y = startY; y < endY; ++y)
	{
		for (int x = startX; x < endX; ++x)
		{
			// Calculate the distance from the center of the circle
			const int distanceX = x - anXPos;
			const int distanceY = y - anYPos;
			const int distanceSquared = distanceX * distanceX + distanceY * distanceY;

			// Check if the current pixel is inside the circle
			if (distanceSquared <= radiusSquared)
			{
				// Calculate the index of the current pixel in the texture data array
				const UINT pixelIndex = y * rowPitch + x * 4;

				// Set the color of the current pixel to white
				pData[pixelIndex] = aColor.r * 255;   // R
				pData[pixelIndex + 1] = aColor.g * 255; // G
				pData[pixelIndex + 2] = aColor.b * 255; // B
			}
		}
	}

	// Unmap the staging texture and copy it back to the painting texture
	DX11::Context->Unmap(stagingTexture.Get(), 0);
	DX11::Context->CopyResource(myPaintingTex.GetTex().Get(), stagingTexture.Get());
}

void Canvas::Clear()
{
	Paint(myWidth / 2, myHeight / 2, myWidth, Color::White());
}

void Canvas::Save()
{
	std::string symbol = GenData::GetSymbol(stagingTexture.Get(), myWidth, myHeight);
	std::string symbolName = "";
	if (symbol == "o") symbolName = "Sun";
	else if (symbol == "b") symbolName = "Bomb";
	else if (symbol == "c") symbolName = "Moon";
	else if (symbol == "-") symbolName = "Slash";

	if (!symbolName.empty())
	{
		PopupManager::CreatePopup("Symbol", 3, { 400,150 }, symbolName, { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 });
	}
	else
	{
		printmsg("Failed");
	}

	myIsPainting = false;
}

void Canvas::Generate()
{
	GenData::GenerateData();
}

void Canvas::StartPainting()
{
	myIsPainting = true;
	//GraphicsEngine::GetInstance()->RunFullScreenShader(
	//	GraphicsEngine::GetInstance()->GetPreviousScreenTex()->GetShaderResourceView().GetAddressOf(),
	//	myScreenTex.GetRenderTargetView().GetAddressOf(),
	//	GraphicsEngine::GetInstance()->myCopyPS);
}

void Canvas::Render()
{
	if (!myIsPainting) return;

	myPaperTex->SetAsResource(2);

	GraphicsEngine::GetInstance()->RunFullScreenShader(
		GraphicsEngine::GetInstance()->GetPreviousScreenTex()->GetShaderResourceView().GetAddressOf(),
		GraphicsEngine::GetInstance()->GetMainCamera()->GetRenderTexture().GetRenderTargetView().GetAddressOf(),
		myCanvasPS);
}

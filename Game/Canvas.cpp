#include "Game.pch.h"
#include "Canvas.h"
#include "CanvasPS.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Material.h"
#include "OpenCV\GenData.h"
#include "PopupManager.h"
#include "Graphics\Rendering\Buffers\Buffers.h"
#include "GameScene.h"
#include "PaintingScene.h"
#include "../Catbox/Graphics/Rendering/GraphicsEngine.h"
#include <mutex>

Canvas* Canvas::Instance;
Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingDataTexture;
Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingDisplayTexture;
Texture tempTex;
std::mutex paintMutex;

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

	myPaintingDisplayTex.CreateEmptyTexture(DXGI_FORMAT_R8G8B8A8_UNORM, myWidth, myHeight, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

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
	DX11::Device->CreateTexture2D(&stagingDesc, nullptr, stagingDataTexture.GetAddressOf());
	DX11::Device->CreateTexture2D(&stagingDesc, nullptr, stagingDisplayTexture.GetAddressOf());

	Clear();
	myPaperTex = AssetRegistry::GetInstance()->GetAsset<Texture>("PaperTex");
}


void Canvas::Awake()
{
	myShader = dynamic_cast<CanvasPS*>(myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetPixelShader().get());
}


void PaintToTexture(int aWidth, int aHeight, int anXPos, int anYPos, int aRadius, float aRadiusModifier, const Color& aColor, ID3D11Texture2D* aStagingTex, ID3D11Resource* anOutputTex)
{
	const std::scoped_lock<std::mutex> lock(paintMutex);
	//Paint onto the painting data texture

	DX11::Context->CopyResource(aStagingTex, anOutputTex);

	// Map the staging texture
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = DX11::Context->Map(aStagingTex, 0, D3D11_MAP_WRITE, 0, &mappedResource);
	if (!FAILED(hr))
	{
		BYTE* pData = reinterpret_cast<BYTE*>(mappedResource.pData);
		const UINT rowPitch = mappedResource.RowPitch;

		// Paint on the staging texture
		const int realRadius = aRadius * aRadiusModifier;
		const int startY = Catbox::Clamp(anYPos - realRadius, 0, aHeight);
		const int endY = Catbox::Clamp(anYPos + realRadius, 0, aHeight);
		const int startX = Catbox::Clamp(anXPos - realRadius, 0, aWidth);
		const int endX = Catbox::Clamp(anXPos + realRadius, 0, aWidth);
		const int radiusSquared = realRadius * realRadius;

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
		DX11::Context->Unmap(aStagingTex, 0);
		DX11::Context->CopyResource(anOutputTex, aStagingTex);
	}
	else
	{
		printerror("Error mapping texture");
	}
}


void Canvas::Paint(int anXPos, int anYPos, int aRadius, float aRadiusModifier, const Color& aColor)
{
	PaintToTexture(myWidth, myHeight, anXPos, anYPos, aRadius, 1, aColor, stagingDataTexture.Get(), myPaintingTex.GetTex().Get());
	PaintToTexture(myWidth, myHeight, anXPos, anYPos, aRadius, aRadiusModifier, aColor, stagingDisplayTexture.Get(), myPaintingDisplayTex.GetTex().Get());
}

void Canvas::Clear()
{
	Paint(myWidth / 2, myHeight / 2, myWidth, 1, Color::White());
}

Texture& Canvas::GetPaintingTex()
{
	if (Input::GetKeyHeld(KeyCode::MOUSERIGHT)) return myPaintingTex;
	return myPaintingDisplayTex;
}

void Canvas::Save()
{
	myCanPaint = false;

	GenData::GetSymbol(mySymbol, stagingDataTexture.Get(), myWidth, myHeight, [&](bool aSucceeded) {ImageRecognitionCallback(aSucceeded); });
}

void Canvas::ImageRecognitionCallback(bool aSucceeded)
{

	auto camPos = Engine::GetInstance()->GetActiveCamera()->GetTransform()->worldPos();
	PaintingScene::GetInstance()->GetGameObject().SetActive(false);
	GameScene::GetInstance()->GetGameObject().SetActive(true);
	Engine::GetInstance()->SetGamePaused(false);
	
	if (aSucceeded && !mySymbol.name.empty())
	{
		GameScene::GetInstance()->PerformAction(mySymbol);
	}
	myCanPaint = true;
}

void Canvas::Generate()
{
	GenData::GenerateDataSVN();
}

void Canvas::StartPainting()
{
	Clear();


	myCanPaint = true;
	GraphicsEngine::GetInstance()->RunFullScreenShader(
		GraphicsEngine::GetInstance()->GetPreviousScreenTex()->GetShaderResourceView().GetAddressOf(),
		myScreenTex.GetRenderTargetView().GetAddressOf(),
		GraphicsEngine::GetInstance()->myCopyPS);

	GameScene::GetInstance()->GetGameObject().SetActive(false);
	PaintingScene::GetInstance()->GetGameObject().SetActive(true);
	Engine::GetInstance()->SetGamePaused(true);
}


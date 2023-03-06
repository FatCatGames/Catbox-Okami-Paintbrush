#include "stdafx.h"
#include "Texture.h"
#include "Graphics/Rendering/DX11/DX11.h"
#include <DDSImporter\DDSTextureLoader11.h>

Texture::~Texture()
{
	myTexture.Reset();
	mySRV.Reset();
}

void Texture::CreateEmptyTexture(DXGI_FORMAT aFormat, int aWidth, int aHeight, int aMipLevel)
{
	myFormat = aFormat;
	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	texDesc.Width = aWidth / myWidthDivider;
	texDesc.Height = aHeight / myHeightDivider;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.Format = aFormat;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MipLevels = aMipLevel;

	HRESULT result = DX11::Device->CreateTexture2D(&texDesc, nullptr, tex.GetAddressOf());
	if (FAILED(result)) assert(true && "Failed to create new empty texture.");


	result = DX11::Device->CreateShaderResourceView(tex.Get(), nullptr, &mySRV);
	if (FAILED(result)) assert(true && "Failed to create new empty texture.");
}

void Texture::CreateScreenSizeTexture(DXGI_FORMAT aFormat, int aWidthDivider, int aHeightDivider)
{
	myWidthDivider = aWidthDivider;
	myHeightDivider = aHeightDivider;
	CreateEmptyTexture(aFormat, DX11::GetResolution().x, DX11::GetResolution().y, 1);
	//DX11::screenTextures.push_back(this);
	Listener listener;
	listener.action = [this] { Resize(DX11::GetResolution().x, DX11::GetResolution().y); };
	DX11::AddResolutionChangedListener(listener);
}

void Texture::CreateVideoTexture(DXGI_FORMAT aFormat, int aWidth, int aHeight, int aMipLevel, bool aDynamic)
{
	D3D11_TEXTURE2D_DESC texDesc = { 0 };
	texDesc.Width = aWidth;
	texDesc.Height = aHeight;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.Format = aFormat;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.MipLevels = aMipLevel;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	if (aDynamic)
	{
		texDesc.Usage = D3D11_USAGE_DYNAMIC;
		texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	HRESULT result = DX11::Device->CreateTexture2D(&texDesc, nullptr, tex.GetAddressOf());
	if (FAILED(result)) assert(true && "Failed to create new empty texture.");

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof(srv_desc));
	srv_desc.Format = aFormat;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = texDesc.MipLevels;
	result = DX11::Device->CreateShaderResourceView(tex.Get(), &srv_desc, &mySRV);
	if (FAILED(result)) assert(true && "Failed to create new empty texture.");
}


void Texture::CreateRenderTargetView()
{
	DX11::Device->CreateRenderTargetView(tex.Get(), nullptr, &myRTV);
}

void Texture::SetAsResource(unsigned int aSlot)
{
	//Put the texture on the Pixel Shader Texture Registry slot provided in aSlot
	DX11::Context->PSSetShaderResources(
		aSlot,
		1,
		mySRV.GetAddressOf()
	);
}

void Texture::Resize(int newWidth, int newHeight)
{
	CreateEmptyTexture(myFormat, newWidth, newHeight, 1);
	CreateRenderTargetView();
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> Texture::GetRenderTargetView() const
{
	if (!myRTV) assert(true && "Called GetRenderTargetView() on texture that doesn't have one. Remember to call CreateRenderTargetView() first.");
	return myRTV;
}

void Texture::SaveAsset(const char* /*aPath*/)
{
	print("Texture saving not implemented.");
}

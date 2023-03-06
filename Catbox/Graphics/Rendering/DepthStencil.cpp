#include "stdafx.h"
#include "DepthStencil.h"
#include "Graphics/Rendering/DX11/DX11.h"
#include <DDSImporter\DDSTextureLoader11.h>

DepthStencil::~DepthStencil()
{
	myTexture.Reset();
	mySRV.Reset();
}

void DepthStencil::CreateDepthStencil(int aWidth, int aHeight)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	depthBufferDesc = { 0 };
	depthBufferDesc.Width = aWidth;
	depthBufferDesc.Height = aHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.MiscFlags = 0;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;


	HRESULT result = DX11::Device->CreateTexture2D(&depthBufferDesc, nullptr, myTex.GetAddressOf());
	assert(SUCCEEDED(result));

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc = {};
	resourceDesc.Format = DXGI_FORMAT_R32_FLOAT;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceDesc.Texture2D.MipLevels = depthBufferDesc.MipLevels;
	result = DX11::Device->CreateShaderResourceView(myTex.Get(), &resourceDesc, &mySRV);
	assert(SUCCEEDED(result));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	result = DX11::Device->CreateDepthStencilView(myTex.Get(), &depthDesc, myDSV.GetAddressOf());
	assert(SUCCEEDED(result));

	myViewport = D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(aWidth), static_cast<float>(aHeight), 0.0f, 1.0f });
}
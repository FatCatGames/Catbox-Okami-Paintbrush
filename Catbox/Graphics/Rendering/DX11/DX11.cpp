#include "stdafx.h"
#include "Graphics/Rendering/GraphicsEngine.pch.h"
#include "DX11.h"
#include <d3d11.h>

ComPtr<ID3D11Device> DX11::Device;
ComPtr<ID3D11DeviceContext> DX11::Context;
ComPtr<IDXGISwapChain> DX11::SwapChain;


ComPtr<ID3D11RenderTargetView> DX11::BackBuffer;
ComPtr<ID3D11RenderTargetView> DX11::SelectionBuffer;
ComPtr<ID3D11Texture2D> DX11::stagingTexture;
ComPtr<ID3D11Texture2D> DX11::selectBufferTexture;
ComPtr<ID3D11Texture2D> DX11::backBufferTexture;

//ComPtr<ID3D11DepthStencilView> DX11::DepthBuffer;
D3D11_TEXTURE2D_DESC DX11::depthBufferDesc;
D3D11_TEXTURE2D_DESC DX11::selectionBufferDesc;
D3D11_TEXTURE2D_DESC DX11::stagingTextureDesc;

D3D11_VIEWPORT DX11::viewPort = {};
D3D11_VIEWPORT DX11::viewPortHalfSize = {};
D3D11_VIEWPORT DX11::viewPortQuarterSize = {};
Vector2i DX11::resolution;
Event DX11::OnResolutionChanged;

bool DX11::Initialize(HWND aWindowHandle, bool aEnableDeviceDebug, int aWidth, int aHeight)
{
	resolution = Vector2i(aWidth, aHeight);

#pragma region Create device and swapchain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		aEnableDeviceDebug ? D3D11_CREATE_DEVICE_DEBUG : 0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&SwapChain,
		&Device,
		nullptr,
		&Context
	);

	if (FAILED(result)) return false;

	

#pragma endregion

#pragma region Create render target view

	const int width = aWidth;
	const int height = aHeight;


	//Our backbuffer of textures is stored in the swapchain, however, we cannot use the textures as they are directly from the buffer.
	//We have to create a render target view for this instead.

	result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferTexture);
	if (FAILED(result)) return false;

	result = Device->CreateRenderTargetView(backBufferTexture.Get(), nullptr, BackBuffer.GetAddressOf());
	if (FAILED(result)) return false;

	//We no longer need this since we have our render target view
	backBufferTexture.Reset();

#pragma endregion

	RECT clientRect = { 0, 0, 0, 0 };
	GetClientRect(aWindowHandle, &clientRect);

#pragma region Initialize depth buffer
	/*ComPtr<ID3D11Texture2D> depthBufferTexture;
	depthBufferDesc = { 0 };
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.MiscFlags = 0;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;


	result = Device->CreateTexture2D(&depthBufferDesc, nullptr, depthBufferTexture.GetAddressOf());
	if (FAILED(result)) return false;

	result = Device->CreateDepthStencilView(depthBufferTexture.Get(), nullptr, DepthBuffer.GetAddressOf());
	if (FAILED(result)) return false;*/
#pragma endregion
#pragma region Create selection render target 

	selectionBufferDesc.Width = width;
	selectionBufferDesc.Height = height;
	selectionBufferDesc.ArraySize = 1;
	selectionBufferDesc.Format = DXGI_FORMAT_R32_UINT;
	selectionBufferDesc.SampleDesc.Count = 1;
	selectionBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	result = Device->CreateTexture2D(&selectionBufferDesc, nullptr, selectBufferTexture.GetAddressOf());
	if (FAILED(result))
		return false;
	D3D11_RENDER_TARGET_VIEW_DESC selectBufferViewDesc;
	selectBufferViewDesc.Format = selectionBufferDesc.Format;
	selectBufferViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	selectBufferViewDesc.Texture2D.MipSlice = 0;

	result = Device->CreateRenderTargetView(selectBufferTexture.Get(), &selectBufferViewDesc, SelectionBuffer.GetAddressOf());
	if (FAILED(result)) return false;


#pragma endregion
#pragma region Copy selection render texture to cpu 
	stagingTextureDesc.Width = 1;
	stagingTextureDesc.Height = 1;
	stagingTextureDesc.ArraySize = 1;
	stagingTextureDesc.Format = DXGI_FORMAT_R32_UINT;
	stagingTextureDesc.SampleDesc.Count = 1;
	stagingTextureDesc.Usage = D3D11_USAGE_STAGING;
	stagingTextureDesc.BindFlags = 0;
	stagingTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingTextureDesc.MiscFlags = 0;
	result = Device->CreateTexture2D(&stagingTextureDesc, NULL, stagingTexture.GetAddressOf());
	if (FAILED(result))
		return false;
#pragma endregion

	/*ID3D11RenderTargetView* buffers[2];
	buffers[0] = BackBuffer.Get();
	buffers[1] = SelectionBuffer.Get();
	Context->OMSetRenderTargets(2, buffers, DepthBuffer.Get());*/

#pragma region Viewport data

	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.Width = static_cast<float>(width);
	viewPort.Height = static_cast<float>(height);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	viewPortHalfSize.TopLeftX = 0.0f;
	viewPortHalfSize.TopLeftY = 0.0f;
	viewPortHalfSize.Width = width / 2.0f;
	viewPortHalfSize.Height = height / 2.0f;
	viewPortHalfSize.MinDepth = 0.0f;
	viewPortHalfSize.MaxDepth = 1.0f;

	viewPortQuarterSize.TopLeftX = 0.0f;
	viewPortQuarterSize.TopLeftY = 0.0f;
	viewPortQuarterSize.Width = width / 4.0f;
	viewPortQuarterSize.Height = height / 4.0f;
	viewPortQuarterSize.MinDepth = 0.0f;
	viewPortQuarterSize.MaxDepth = 1.0f;

	Context->RSSetViewports(1, &viewPort);
#pragma endregion



	return true;
}

void DX11::BeginFrame(std::array<float, 4> aClearColor)
{
	Context->ClearRenderTargetView(BackBuffer.Get(), &aClearColor[0]);
	//Context->ClearDepthStencilView(DepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	const FLOAT clear[4] = { 0, 0, 0, 0 };
	Context->ClearRenderTargetView(SelectionBuffer.Get(), clear);
	//Context->OMSetRenderTargets(1, BackBuffer.GetAddressOf(), DepthBuffer.Get());
}

void DX11::EndFrame()
{
	SwapChain->Present(1, 0);
}

UINT DX11::GetScreenObjectId(UINT x, UINT y)
{
	if (x >= selectionBufferDesc.Width) return 0;
	if (y >= selectionBufferDesc.Height) return 0;

	D3D11_BOX b = { x, y, 0, x + 1, y + 1, 1 };
	Context->CopySubresourceRegion(stagingTexture.Get(), 0, 0, 0, 0, selectBufferTexture.Get(), 0, &b);

	D3D11_MAPPED_SUBRESOURCE mapped;
	Context->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mapped);
	UINT16* p = (UINT16*)mapped.pData;
	Context->Unmap(stagingTexture.Get(), 0);
	if (p == NULL) return 0;
	return *p;
}

std::vector<UINT> DX11::GetScreenObjectIds(int minX, int minY, int maxX, int maxY)
{
	std::vector<UINT> ids;
	for (int x = minX; x < maxX; x += 10)
	{
		for (int y = minY; y < maxY; y += 10)
		{
			UINT id = GetScreenObjectId(x, y);
			if (std::find(ids.begin(), ids.end(), id) == ids.end()) ids.push_back(id);
		}
	}

	return ids;
}

void DX11::AddResolutionChangedListener(Listener& aListener)
{
	OnResolutionChanged.AddListener(aListener);
}

void DX11::RemoveResolutionChangedListener(Listener& aListener)
{
	OnResolutionChanged.RemoveListener(aListener);
}

void DX11::ResizeBackBuffer(int aWidth, int aHeight)
{
	auto count2 = backBufferTexture.Reset();
	auto count = BackBuffer.Reset();
	HRESULT result = DX11::SwapChain->ResizeBuffers(1, aWidth, aHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferTexture);
	Device->CreateRenderTargetView(backBufferTexture.Get(), nullptr, BackBuffer.GetAddressOf());
}

void DX11::SetResolution(int aWidth, int aHeight)
{
	resolution = Vector2i(aWidth, aHeight);

#pragma region Viewport data

	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.Width = static_cast<float>(aWidth);
	viewPort.Height = static_cast<float>(aHeight);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	viewPortHalfSize.TopLeftX = 0.0f;
	viewPortHalfSize.TopLeftY = 0.0f;
	viewPortHalfSize.Width = aWidth / 2.0f;
	viewPortHalfSize.Height = aHeight / 2.0f;
	viewPortHalfSize.MinDepth = 0.0f;
	viewPortHalfSize.MaxDepth = 1.0f;

	viewPortQuarterSize.TopLeftX = 0.0f;
	viewPortQuarterSize.TopLeftY = 0.0f;
	viewPortQuarterSize.Width = aWidth / 4.0f;
	viewPortQuarterSize.Height = aHeight / 4.0f;
	viewPortQuarterSize.MinDepth = 0.0f;
	viewPortQuarterSize.MaxDepth = 1.0f;

	Context->RSSetViewports(1, &viewPort);
#pragma endregion

	selectionBufferDesc.Width = aWidth;
	selectionBufferDesc.Height = aHeight;

	Device->CreateTexture2D(&selectionBufferDesc, nullptr, selectBufferTexture.GetAddressOf());

	D3D11_RENDER_TARGET_VIEW_DESC selectBufferViewDesc;
	selectBufferViewDesc.Format = selectionBufferDesc.Format;
	selectBufferViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	selectBufferViewDesc.Texture2D.MipSlice = 0;

	Device->CreateRenderTargetView(selectBufferTexture.Get(), &selectBufferViewDesc, SelectionBuffer.GetAddressOf());

	ResizeBackBuffer(aWidth, aHeight);


	OnResolutionChanged.Invoke();
}
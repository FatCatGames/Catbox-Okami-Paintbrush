#include "stdafx.h"
#include "GBuffer.h"
#include "..\DX11\DX11.h"

#include "GameObjects/Transform.h"
#include "Components/3D/Camera.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Material.h"
#include "Assets/MeshData.h"
#include "Assets/Model.h"
#include "Assets/Scene.h"

bool GBuffer::CreateTextures()
{
	DXGI_FORMAT formats[]
	{
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8G8_UNORM,
	};


	for (int i = 0; i < GBuffer::GBufferTexture::Count; ++i)
	{
		myTextures[i].CreateScreenSizeTexture(formats[i]);
		myTextures[i].CreateRenderTargetView();
	}

	return true;
}

void GBuffer::SetAsTarget() const
{
	ID3D11RenderTargetView* rtvs[GBufferTexture::Count+1];
	for (int i = 0; i < myTextures.size(); i++)
	{
		rtvs[i] = myTextures[i].GetRenderTargetView().Get();
	}
	rtvs[GBufferTexture::Count] = DX11::SelectionBuffer.Get();
	auto dsv = Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetDepthStencil().GetDepthStencilView().Get();
	DX11::Context->OMSetRenderTargets(GBufferTexture::Count+1, &rtvs[0], dsv);
}

void GBuffer::ClearTarget() const
{
	ID3D11RenderTargetView* rtvs[GBufferTexture::Count+1];
	for (int i = 0; i < myTextures.size()+1; i++)
	{
		rtvs[i] = nullptr;
	}
	auto dsv = Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetDepthStencil().GetDepthStencilView().Get();
	DX11::Context->OMSetRenderTargets(GBufferTexture::Count+1, &rtvs[0], dsv);
}

void GBuffer::SetAsResource(unsigned int aStartSlot) const
{
	ID3D11ShaderResourceView* srvs[GBufferTexture::Count];
	for (int i = 0; i < myTextures.size(); i++)
	{
		srvs[i] = myTextures[i].GetShaderResourceView().Get();
	}

	DX11::Context->PSSetShaderResources(aStartSlot, GBufferTexture::Count, &srvs[0]);
}

void GBuffer::ClearResource(unsigned int aStartSlot) const
{
	ID3D11ShaderResourceView* srvs[GBufferTexture::Count];
	for (int i = 0; i < myTextures.size(); i++)
	{
		srvs[i] = nullptr;
	}

	DX11::Context->PSSetShaderResources(aStartSlot, GBufferTexture::Count, &srvs[0]);
}

void GBuffer::Clear() const
{
	std::array<float, 4> clear{ 0, 0, 0, 0 };
	for (int i = 0; i < myTextures.size(); ++i)
	{
		DX11::Context->ClearRenderTargetView(myTextures[i].GetRenderTargetView().Get(), &clear[0]);
	}
}

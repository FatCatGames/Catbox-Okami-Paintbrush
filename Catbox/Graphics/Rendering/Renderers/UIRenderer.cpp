#include "stdafx.h"
#include "UIRenderer.h"
#include "Assets/Shader.h"

void UIRenderer::Render(std::vector<UISprite*>& someSpritesToRender, ComPtr<ID3D11RenderTargetView> aTarget)
{
	std::sort(someSpritesToRender.begin(), someSpritesToRender.end(), [](const UIElement* aFirst, const UIElement* aSecond)
		{
			return aFirst->GetLayer() < aSecond->GetLayer();
		});

	ID3D11RenderTargetView* buffers[2];
	buffers[0] = aTarget.Get();
	buffers[1] = DX11::SelectionBuffer.Get();
	DX11::Context->OMSetRenderTargets(2, buffers, nullptr);

	myVertices[0] = { 1, -0.5f, 0, 1 };

	DX11::Context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &myStride, &myOffset);
	DX11::Context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	DX11::Context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(myPrimitiveTopology));


	DX11::Context->VSSetConstantBuffers(11, 1, myUIDataBuffer.GetAddress());
	DX11::Context->PSSetConstantBuffers(11, 1, myUIDataBuffer.GetAddress());

	for (size_t i = 0; i < someSpritesToRender.size(); i++)
	{
		if (!someSpritesToRender[i]->GetTexture()) continue;
		someSpritesToRender[i]->GetTexture()->SetAsResource(0);
		someSpritesToRender[i]->GetPixelShader()->SetResource();
		myUIDataBuffer.SetData(&someSpritesToRender[i]->GetUIRenderData());

		DX11::Context->PSSetShader(someSpritesToRender[i]->GetPixelShader()->pixelShader, nullptr, 0);
		DX11::Context->VSSetShader(someSpritesToRender[i]->GetVertexShader()->vertexShader, nullptr, 0);
		DX11::Context->IASetInputLayout(someSpritesToRender[i]->GetVertexShader()->inputLayout.Get());

		DX11::Context->DrawIndexed(6, 0, 0);
	}

	ID3D11RenderTargetView* clearRtvs[1] = { nullptr };
	ID3D11ShaderResourceView* clearSrvs[1] = { nullptr };
	DX11::Context->PSSetShaderResources(0, 1, clearSrvs);
	DX11::Context->OMSetRenderTargets(1, clearRtvs, nullptr);
}

bool UIRenderer::Initialize()
{
	if (!myUIDataBuffer.Initialize()) return false;

	myVertices[0] = { -0.5f, -0.5f, 0, 1 };
	myVertices[1] = { 0.5f, -0.5f, 0, 1 };
	myVertices[2] = { -0.5f, 0.5f, 0, 1 };
	myVertices[3] = { 0.5f, 0.5f, 0, 1 };

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(4 * sizeof(Vector4f));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = &myVertices[0];

	ID3D11Buffer* vertexBuffer;

	HRESULT result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);
	if (FAILED(result)) return false;

	myStride = sizeof(Vector4f);
	myOffset = 0;
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	myVertexBuffer = vertexBuffer;
	//Create indices
	myIndices[0] = 2;
	myIndices[1] = 3;
	myIndices[2] = 1;
	myIndices[3] = 1;
	myIndices[4] = 0;
	myIndices[5] = 2;

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(6 * sizeof(unsigned int));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	indexSubresourceData.pSysMem = &myIndices[0];

	ID3D11Buffer* indexBuffer;
	result = DX11::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);
	if (FAILED(result)) return false;

	myIndexBuffer = indexBuffer;

	return true;
}

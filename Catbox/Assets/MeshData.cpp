#include "stdafx.h"
#include "MeshData.h"
#include "Graphics/Rendering/DX11/DX11.h"

bool MeshData::SetVertices(const std::vector<AnimatedVertex>& someVertices)
{
	myAnimatedVertices.clear();
	if (myVertexBuffer.Get() != nullptr)
	{
		myVertexBuffer.Get()->Release();
	}
	myIsAnimated = true;
	myNumberOfVertices = static_cast<UINT>(someVertices.size());
	myAnimatedVertices = someVertices;

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(myAnimatedVertices.size()) * static_cast<UINT>(sizeof(AnimatedVertex));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = &myAnimatedVertices[0];

	ID3D11Buffer* vertexBuffer;

	HRESULT result;
	result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);
	if (FAILED(result)) return false;

	myStride = sizeof(AnimatedVertex);
	myOffset = 0;
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	myVertexBuffer = vertexBuffer;
	return true;
}


bool MeshData::SetVertices(const std::vector<Vertex>& someVertices)
{
	myIsAnimated = false;
	if (myVertexBuffer.Get() != nullptr) 
	{
		myVertexBuffer.Get()->Release();
	}
	myNumberOfVertices = static_cast<UINT>(someVertices.size());
	myVertices = someVertices;

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(myVertices.size()) * static_cast<UINT>(sizeof(Vertex));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = &myVertices[0];

	ID3D11Buffer* vertexBuffer;

	HRESULT result;
	result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);
	if (FAILED(result)) return false;

	myStride = sizeof(Vertex);
	myOffset = 0;
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	myVertexBuffer = vertexBuffer;
	return true;
}

bool MeshData::SetIndices(const std::vector<unsigned int>& someIndices)
{
	myNumberOfIndices = static_cast<UINT>(someIndices.size());
	myIndices = someIndices;

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(myIndices.size()) * static_cast<UINT>(sizeof(unsigned int));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	indexSubresourceData.pSysMem = &myIndices[0];

	ID3D11Buffer* indexBuffer;
	HRESULT result;
	result = DX11::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);
	if (FAILED(result)) return false;

	myIndexBuffer = indexBuffer;
	return true;
}

void MeshData::SaveAsset(const char* /*aPath*/)
{
	print("MeshData saving not implemented.");
}

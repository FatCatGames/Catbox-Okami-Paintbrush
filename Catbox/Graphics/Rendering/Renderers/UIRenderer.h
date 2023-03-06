#pragma once
#include "../Buffers/CBuffer.hpp"
#include "Components/UI/UISprite.h"

class UIRenderer
{
public:
	void Render(std::vector<UISprite*>& someSpritesToRender, ComPtr<ID3D11RenderTargetView> aTarget);
	bool Initialize();

private:
	Catbox::CBuffer<UIElement::UIRenderData> myUIDataBuffer;
	Catbox::CBuffer<ResourceDataBuffer> myResourceDataBuffer;

	Vector4f myVertices[4];
	unsigned int myIndices[6];

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

	UINT myPrimitiveTopology = 0;
	UINT myStride = 0;
	UINT myOffset = 0;
};
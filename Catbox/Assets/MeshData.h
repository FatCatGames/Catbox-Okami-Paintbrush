#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Asset.h"

struct Vertex
{
	Vector4f position;
	Vector4f colors[4];
	Vector2f UVs[4];

	Vector3f tangent;
	Vector3f binormal;
	Vector3f normal;
};

struct AnimatedVertex : Vertex
{
	Catbox::Vector4<unsigned int> boneIDs;
	Vector4f boneWeights;
};

class MeshData : public Asset
{
public:
	bool SetVertices(const std::vector<Vertex>& someVertices);
	bool SetVertices(const std::vector<AnimatedVertex>& someVertices);
	bool SetIndices(const std::vector<unsigned int>& someIndices);

	Vector3f myPivot = { 0,0,0 };
	std::vector<Vertex> myVertices;
	std::vector<AnimatedVertex> myAnimatedVertices;
	std::vector<unsigned int> myIndices;
	bool myIsAnimated = false;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

	UINT myPrimitiveTopology = 0;
	UINT myNumberOfVertices = 0;
	UINT myNumberOfIndices = 0;
	UINT myStride = 0; //sizeof(vertex)
	UINT myOffset = 0; //where the first vertex is located in our buffer, useful if you have multiple models per buffer

private:
	void SaveAsset(const char* aPath) override;
};
#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "../Buffers/CBuffer.hpp"
#include "../Buffers/StructuredBuffer.hpp"
#include "../DXDeviceStates.h"
#include "Assets\Model.h"

using namespace Catbox;

class Material;
class Buffers;
class NavMesh;
class DebugRenderer
{
public:
	bool Initialize(std::shared_ptr<Buffers> aBuffers);
	void Render(const std::vector<DebugLine>& aLineList, const std::vector<DebugCube>& aBoxList, ComPtr<ID3D11RenderTargetView> aTarget);
	void Render(const std::vector<NavMesh*>& aNavMeshList, ComPtr<ID3D11RenderTargetView> aTarget);
	void SetBlendState(const BlendState& aBlendState);
	void SetDepthStencilState(const DepthStencilState& aDepthStencilState);

private:
	std::shared_ptr<Material> myDebugMat;
	std::shared_ptr<Buffers> myBuffers;
	Model myCubeModel;
	Model myLineModel;
	MeshData myCubeMesh;
	MeshData myLineMesh;
	std::shared_ptr<PixelShader> myPixelShader;
	std::shared_ptr<VertexShader> myVertexShader;
	Vertex tempVertex;
	Vertex tempOtherVertex;
};
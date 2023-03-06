#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "CommonUtilities/Matrix4x4.hpp"
#include "../Buffers/CBuffer.hpp"
#include "../Buffers/StructuredBuffer.hpp"
#include "Components/3D/Light.h"
#include "../DXDeviceStates.h"
#include "Editor\Windows\SceneWindow.h"
#include "Assets/Material.h"

using namespace Catbox;

class SceneLightData;
struct LightData;
class Buffers;


struct SceneLightsBufferData
{
	int pointStartIndex; //4 bytes
	int IBLStartIndex;//4 bytes
	int lightsCount;//4 bytes
	int padding;//4 bytes
};


class Camera;
class ModelInstance;
class ParticleSystem;
class ForwardRenderer
{
public:
	bool Initialize(std::shared_ptr<Buffers> aBuffers);
	void SetDebugRenderMode(int aRenderMode);
	int GetDebugRenderMode();
	void Render(const std::vector<ModelInstance*>& aModelList, const std::vector<ParticleSystem*>& aParticleSystemList, ComPtr<ID3D11RenderTargetView> aTarget);
	void SetBlendState(const BlendState& aBlendState);
	void SetRastertizerState(const RastertizerState& aBlendState);
	void SetDepthStencilState(const DepthStencilState& aDepthStencilState);
	std::vector<ModelInstance*> OrderBackToFront(std::vector<ModelInstance*> aModelList);
	unsigned int GetDrawCalls() const { return myDrawCalls; };

private:
	void RenderModels(const std::vector<ModelInstance*>& aModelList);
	void RenderParticles(const std::vector<ParticleSystem*>& aParticleSystemList);

	unsigned int myDrawCalls = 0;
	std::shared_ptr<Buffers> myBuffers;
};
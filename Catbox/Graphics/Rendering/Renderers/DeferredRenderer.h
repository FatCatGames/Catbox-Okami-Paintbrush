#pragma once
#include <wrl.h>
#include "CommonUtilities/Matrix4x4.hpp"
#include "Components/3D/Light.h"
#include "../DXDeviceStates.h"
#include "../Buffers/Buffers.h"
#include "../PostProcessingVolume.h"

using namespace Catbox;

class SceneWindow;
class SceneLightData;
struct LightData;
class Camera;
class ModelInstance;
class DeferredRenderer
{
public:
	DeferredRenderer() = default;
	~DeferredRenderer() = default;
	bool Initialize(std::shared_ptr<Buffers> aBuffers);
	void Render(ComPtr<ID3D11RenderTargetView> aTarget);
	void SetBlendState(const BlendState& aBlendState) const;
	void SetRastertizerState(const RastertizerState& aBlendState) const;
	void SetDepthStencilState(const DepthStencilState& aDepthStencilState) const;
	unsigned int GetDrawCalls() { return myDrawCalls; };
	int GetDebugRenderMode() const { myBuffers->myFrameBufferData.DebugMode; }
	void GenerateGBuffer(std::vector<ModelInstance*>& aModelList);
	void OrderFrontToBack(std::vector<ModelInstance*>& aModelList);

private:

	unsigned int myDrawCalls;
	std::shared_ptr<Buffers> myBuffers;
	std::shared_ptr<PixelShader> myGBufferPS;
	std::shared_ptr<PixelShader> myDeferredPS;
	std::shared_ptr<VertexShader> myFullscreenVS;
};
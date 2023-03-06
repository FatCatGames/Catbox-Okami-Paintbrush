#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Components/3D/Light.h"
#include "../DXDeviceStates.h"
#include "../Buffers/Buffers.h"
#include "../Buffers/GBuffer.h"
#include "../PostProcessingVolume.h"

using namespace Catbox;

class GraphicsEngine;
class Camera;
class PixelShader;
class VertexShader;

struct OutlineData
{
	Color color;
};

class PostProcessRenderer
{
public:
	PostProcessRenderer() = default;
	~PostProcessRenderer() = default;
	bool Initialize(std::shared_ptr<Buffers> aBuffers);
	void SetBlendState(const BlendState& aBlendState) const;
	void SetDepthStencilState(const DepthStencilState& aDepthStencilState) const;
	void Clear();
	void RenderBloom(Camera& aCamera, ID3D11ShaderResourceView* const* aSource, ID3D11RenderTargetView* const* aTarget);
	void RenderAmbientOcclusion(Camera& aCamera, ID3D11ShaderResourceView* const* aSource);
	void RenderPostProcessing(Camera& aCamera, Texture* aSource, Texture* aTarget);
	void RenderFog(Camera& aCamera, Texture* aSource, Texture* aTarget);
	Texture* CreateMask(const std::vector<ModelInstance*>& aModelList);
	void RenderOutlines(ID3D11ShaderResourceView** aMask, ID3D11ShaderResourceView** aScreenTex, ID3D11RenderTargetView** aTarget, bool aDefault);
	void ReloadPostProcessShaders();

private:
	void RenderColorAdjustments(std::shared_ptr<PostProcessingVolume> aVolume, Texture* lastUsedTarget, Texture* targetToUse);
	void RenderVignette(std::shared_ptr<PostProcessingVolume> aVolume, Texture* lastUsedTarget, Texture* targetToUse);
	std::filesystem::path LoadFolder(const std::filesystem::path& aDirectory, const std::filesystem::path& aName); //Hjälpfunktion för postprocess reloading
	std::list<std::shared_ptr<PixelShader>> myPostProcessShaders;

	std::shared_ptr<GraphicsEngine> myGraphicsEngine;
	std::shared_ptr<Buffers> myBuffers;
	CBuffer<ColorAdjustmentData> myColorAdjustmentBuffer;
	CBuffer<BloomData> myBloomBuffer;
	CBuffer<VignetteData> myVignetteBuffer;
	CBuffer<OutlineData> myOutlineBuffer;
	CBuffer<SSAOData> mySSAOBuffer;
	CBuffer<FogData> myFogBuffer;
	OutlineData myEditorOutlineData;

	std::shared_ptr<PixelShader> myPostProcessingPS;
	std::shared_ptr<PixelShader> myLuminancePS;
	std::shared_ptr<PixelShader> myGaussianBlurPS;
	std::shared_ptr<PixelShader> myBloomPS;
	std::shared_ptr<PixelShader> myVignettePS;
	std::shared_ptr<PixelShader> mySSAOPS;
	std::shared_ptr<PixelShader> myCopyPS;
	std::shared_ptr<PixelShader> myOutlinePS;
	std::shared_ptr<PixelShader> myOutlineMaskPS;
	std::shared_ptr<PixelShader> myFogPS;

	std::shared_ptr<VertexShader> myFullscreenVS;
	Texture* myTargetToUse;
	Texture* myResourceToUse;
	Texture myIntermediateTargetA;
	Texture myIntermediateTargetB;
	Texture myHalfSizeTarget;
	Texture myQuarterSizeTarget;
	Texture myBlurTargetA;
	Texture myBlurTargetB;
	Texture mySSAOTarget;
	std::shared_ptr<Texture> myNoiseTexture;
};
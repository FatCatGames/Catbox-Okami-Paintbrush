#pragma once
#include "CommonUtilities\Color.h"

struct ColorAdjustmentData
{
	Color Tint; //16 bytes
	float Brightness = 1; //4 bytes
	float Contrast = 1; //4 bytes
	float Saturation = 1; // 4 bytes
	float Padding = 0; //4 bytes
};

struct BloomData
{
	float Strength = 1;
	float Cutoff = 0.5f;
	float Blur = 1;
	float Padding = 0;
};

struct VignetteData
{
	float Strength = -1;
	float Ratio = 1;
	float Radius = 1;
	int Slope = 2;
	Vector2f Center = {0.5f, 0.5f};
	Vector2f Padding;
};

struct SSAOData
{
	float bias = 0.35f;
	float strength = 0.05f;
	float radius = 0.02f;
	float padding;
};

struct FogData
{
	Color color;
	float density = 1;
	float cutoff = 0.2f;

	float yDepthDensity = 1;
	float yDepthCutoff = -2;
	float yDepthSmoothing = 5;
	Vector3f Padding;
	Color yDepthColor;
};

enum PostProcessEffect
{
	ColorAdjustment,
	Bloom,
	Vignette,
	AmbientOcclusion,
	Fog,
	Count
};

class PostProcessingProfile : public Asset
{
public:
	void SaveAsset(const char* aPath) override;
	void LoadFromPath(const char* aPath) override;
	void RenderInProperties() override;
	inline ColorAdjustmentData& GetSettings() { return mySettings; }
	inline BloomData& GetBloomSettings() { return myBloomSettings; }
	inline VignetteData& GetVignetteSettings() { return myVignetteSettings; }
	inline SSAOData& GetSSAOSettings() { return mySSAOSettings; }
	inline FogData& GetFogSettings() { return myFogSettings; }
	inline bool IsEffectEnabled(PostProcessEffect anEffect) { return myEnabledEffects[anEffect]; }
	inline bool GetShaderReload() { return myShouldReloadShaders; }
	inline void SetShaderReload(bool aValue) { myShouldReloadShaders = aValue; }

private:
	friend class PostProcessingVolume;
	friend class PostProcessingWindow;

	ColorAdjustmentData mySettings;
	bool myEnabledEffects[PostProcessEffect::Count];
	bool myShouldReloadShaders;
	BloomData myBloomSettings;
	SSAOData mySSAOSettings;
	VignetteData myVignetteSettings;
	FogData myFogSettings;
};
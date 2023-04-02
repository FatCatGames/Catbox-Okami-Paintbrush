#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "ParticleProperty.hpp"
#include "CommonUtilities/Color.h"
#include "Graphics\ColorGradient.h"

#include "Graphics/Rendering/DXDeviceStates.h"
#include "Shaders\FlipbookShader.h"
#include "Shaders\ParticleAtlasWalkShader.h"

class GeometryShader;
struct ParticleBufferData
{
	Vector4f position = { 0,0,0,1 };
	Vector4f color = { 1,1,1,1 };
	Vector3f scale = { 1,1,1 };
	float lifetime;
	int index = -1;
	float rotation = 0;
};

class ParticleEmitterSettings : public Asset
{
public:

	ParticleEmitterSettings();
	~ParticleEmitterSettings() = default;
	void RenderInProperties() override;
	inline const BlendState& GetBlendState() { return myBlendState; }

	void LoadFromPath(const char* aPath) override;

	enum class EmissionShape
	{
		Cone,
		Sphere,
		Edge,
		Cube
	};

private:

	void SaveAsset(const char* aPath) override;

	void RenderEmissionSettings();
	void RenderShapeSettings();
	void RenderOverLifetimeSettings();
	void RenderNoiseSettings();
	void RenderGraphicsSettings();

	void RenderDataPropertyOptions(ParticleProperty<float>* aProperty, const float& aMin = 0, const float& aMax = 100);
	void RenderDataPropertyOptions(ParticleProperty<int>* aProperty);

	friend class AssetLoader;
	friend class ParticleEmitter;

	//DX11 rendering stuff
	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	UINT myStride;
	UINT myDXOffset;
	std::shared_ptr<VertexShader> myVertexShader;
	std::shared_ptr<GeometryShader> myGeometryShader;
	std::shared_ptr<PixelShader> myPixelShader;
	UINT myPrimitiveTopology;
	D3D11_BUFFER_DESC myVertexBufferDesc;
	D3D11_SUBRESOURCE_DATA mySubresourceData;

	//Emission data
	bool myIsLooping = true;
	float myDuration = 5;
	float myStartDelay = 0;
	ParticleProperty<float> myLifetime;
	ParticleProperty<float> mySpawnDelay;
	ParticleProperty<float> myTimeBetweenEmissions;
	ParticleProperty<int> myParticlesPerEmission;
	ParticleProperty<float> mySize;
	ParticleProperty<float> mySpeed;
	ParticleProperty<float> myRotation;
	int myMaxParticles = 100;
	Color myStartColor;
	bool myUseGravity = false;
	float myGravityModifier = 1;

	struct ShapeData
	{
		EmissionShape shape = EmissionShape::Cone;
		Vector3f offset;
		Vector3f size;
	} myShapeData;

	struct NoiseSettings
	{
		bool enabled = false;
		bool enableX, enableY, enableZ = true;
		float strength = 0.1f;
		float frequency = 1.f;
	} myNoiseSettings;

	//Over lifetime
	ColorGradient myColorOverLifetime;
	Curve mySizeOverLifetime;
	Curve mySpeedOverLifetime;
	Curve myRotationOverLifetime;
	Curve myVelocityOverLifetimeX;
	Curve myVelocityOverLifetimeY;
	Curve myVelocityOverLifetimeZ;
	bool myColorOverLifetimeEnabled = false;
	bool mySizeOverLifetimeEnabled = false;
	bool myRotationOverLifetimeEnabled = false;
	bool mySpeedOverLifetimeEnabled = false;
	bool myVelocityOverLifetimeEnabled = false;
	float myVelocityOverTimeInfluence = 10;
	float myEndSize = 0;
	float myEndSpeed = 0;
	bool myShouldUpdate = false;
	bool myNeedsRestart = false;
	bool myHasUnsavedChanges = false;
	bool myUseWorldSpace = true;

	//Rendering

	enum class RenderType
	{
		Default,
		Flipbook,
		AtlasWalk,
		Material
	};

	RenderType myType = RenderType::Default;
	BlendState myBlendState = BlendState::BS_AlphaBlend;
	std::shared_ptr<Texture> myTexture;
	std::shared_ptr<Material> myMaterial;
	std::shared_ptr<Material> myDefaultMaterial;
	std::shared_ptr<PixelShader> myFlipbookShader;
	FlipbookShader::FlipbookBufferData* myFlipbookData;
	std::shared_ptr<PixelShader> myAtlasWalkShader;
	ParticleAtlasWalkShader::ParticleAtlasWalkBufferData* myAtlasWalkData;
};
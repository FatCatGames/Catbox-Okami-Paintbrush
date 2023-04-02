#pragma once
#include "ParticleEmitterSettings.h"
#include "Assets\Material.h"


class ParticleEmitter
{
public:

	struct ParticleData
	{
		Vector4f velocity = { 0, 0, 0, 0 };
		Vector4f position = { 0, 0, 0, 1 };
		Vector3f startRotation = { 0,0,0 };
		Vector3f currentRotation = { 0,0,0 };
		Color startColor;
		Color endColor;
		float startSpeed = 1;
		float currentSpeed = 0;
		float endSpeed = 0;
		float spawnTimer = 0;
		float startLifetime = 1;
		float remainingLifetime = 1;
		float startSize = 1;
		float currentSize = 1;
		float noiseOffset = 0;
		int index;
	};

	struct EmitterData
	{
		float timeUntilEmission = 0;
		float timePassed = 0;
	};

	struct ParticleInstanceData
	{
		Catbox::Matrix4x4<float> transform;
	};

	ParticleEmitter(Transform* aParent) { myParent = aParent; }
	~ParticleEmitter() = default;
	const inline std::shared_ptr<ParticleEmitterSettings> GetSharedData() { return mySharedData; }
	void SetSharedData(std::shared_ptr<ParticleEmitterSettings> someSharedData, bool reloadParticles = true);
	void Init();

	void RenderInProperties();
	void Update();
	void Replay();
	void SetAsResource(Catbox::CBuffer<Material::MaterialBufferData>& aMaterialBuffer);
	void Render();
	void UpdateMaxParticles();
	void SetPaused(bool aIsPaused) { myIsPaused = aIsPaused; }
	void ResetTimeUntilEmission();

private:
	void CreateInitialShape(std::vector<ParticleData*>& someParticles, ParticleEmitterSettings::ShapeData& aShapeData);
	void UpdateParticleSystem();
	void ResetParticle(ParticleData& aParticle);
	void SpawnParticles();

	std::vector<ParticleInstanceData> myInstances;
	ComPtr<ID3D11Buffer> myInstanceBuffer;

	EmitterData myInstanceData;
	std::shared_ptr<ParticleEmitterSettings> mySharedData;
	std::vector<ParticleData> myParticles;
	std::vector<ParticleBufferData> myParticleBufferData;
	Transform* myParent;

	float myDelayTimer = 0;
	int myParticlesToRender = 0;
	int myParticleIndex = 0;
	bool myIsPaused = false;
	bool myShouldUpdate = false;
};
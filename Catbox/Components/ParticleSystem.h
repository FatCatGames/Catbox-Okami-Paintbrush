#pragma once
#include "Graphics/Particles/ParticleEmitter.h"

class ParticleSystem : public Component
{
public:
	
	~ParticleSystem() = default;
	void Update() override;
	void RunInEditor() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void Render();
	std::vector<ParticleEmitter>& GetEmitters();
	void Play();
	void Replay();
	void AddEmitter(std::shared_ptr<ParticleEmitterSettings> aSettings);

private:
	std::vector<ParticleEmitter> myEmitters;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	void OnObjectFinishedLoading() override;
};


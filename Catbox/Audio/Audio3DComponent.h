#pragma once
#include <vector>
#include "AudioManager.h"

class Audio3DComponent : public Component
{
public:
	Audio3DComponent() = default;
	~Audio3DComponent();
	void Awake() override;
	void Update() override;
	void AddSoundEffect(AudioManager::SoundAsset* a3DSoundEffect);
	void PlaySoundEffect(int aSoundEffect);
	void StopSoundEffect(int aSoundEffect, bool anInstantFlag = true);
	void StopAllSoundEffect(bool anInstantFlag = true);
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void RunInEditor() override;
	void OnDisable() override;
	void OnDestroy() override;

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	std::vector<SoundEventInstanceHandle> mySoundEffects;
	std::vector<const AudioManager::SoundAsset*> mySoundEffectTypes;
	int myAmountOfSounds = 0;
	bool myAlwaysActive = false;
	bool doOnce = true;
};
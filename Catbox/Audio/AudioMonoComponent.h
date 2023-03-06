#pragma once
#include <vector>
#include "AudioManager.h"

class AudioMonoComponent : public Component
{
public:
	AudioMonoComponent() = default;
	~AudioMonoComponent();
	void Awake() override;
	void Update() override;
	void AddSoundEffect(AudioManager::SoundAsset* aMonoSoundEffect);
	void PlaySoundEffect(int aSoundEffect);
	void PlaySoundEffectAsOneShot(int aSoundEffect);
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
	int mySoundToPreview = 0;
	bool myAlwaysActive = false;
	bool doOnce = true;
};
#pragma once
#include "Assets/PostProcessingProfile.h"

class PostProcessingVolume
{
public:
	PostProcessingVolume();
	void RenderInProperties();
	//this is temporary until its possible to select Assets
	void AddBlendProfile(std::shared_ptr<PostProcessingProfile> aProfile, float aPosition);
	inline bool IsEnabled() const { return myIsEnabled; }
	inline void SetEnabled(bool aValue) { myIsEnabled = aValue; }
	void SetProfile(std::shared_ptr<PostProcessingProfile> aProfile);
	std::shared_ptr<PostProcessingProfile> GetProfile(){ return myProfile;}
	const ColorAdjustmentData* GetPostProcess();
	const BloomData* GetBloomSettings();
	const VignetteData* GetVignetteSettings();
	void Save(rapidjson::Value& aVolumeData);
	void Load(rapidjson::Value& aVolumeData);

private:
	struct BlendVolumePoint
	{
		std::shared_ptr<PostProcessingProfile> settings;
		float position = 0;
	};
	void CalculateBlendResult();

	bool myIsEnabled = false;
	bool myBlendingEnabled = false;
	std::shared_ptr<PostProcessingProfile> myProfile;
	std::shared_ptr<PostProcessingProfile> myBlendedProfile;
	float myBlendValue = 0.5f;
	std::vector<BlendVolumePoint> myBlendSettings;
};
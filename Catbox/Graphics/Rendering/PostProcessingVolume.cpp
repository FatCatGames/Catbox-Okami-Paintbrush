#include "stdafx.h"
#include "PostProcessingVolume.h"
#include "UtilityFunctions.hpp"

PostProcessingVolume::PostProcessingVolume()
{
	auto profileList = AssetRegistry::GetInstance()->GetAssets<PostProcessingProfile>();
	if (!profileList.empty())
	{
		myProfile = std::dynamic_pointer_cast<PostProcessingProfile>(profileList[0]);
	}
}

void PostProcessingVolume::RenderInProperties()
{
	Catbox::Checkbox("Enable", &myIsEnabled);

	int profileIndex = -1;
	std::vector<const char*> profileNames;
	auto profileList = AssetRegistry::GetInstance()->GetAssets<PostProcessingProfile>();
	int i = 0;
	for (auto& p : profileList)
	{
		if (myProfile == p) profileIndex = i;
		profileNames.push_back(p->GetName().c_str());
		++i;
	}

	if (Catbox::Checkbox("Enable Blending", &myBlendingEnabled))
	{
		if (myBlendingEnabled)
		{
			myBlendedProfile = std::make_shared<PostProcessingProfile>();
			if (myBlendSettings.empty())
			{
				myBlendSettings.resize(2);
				myBlendSettings[1].position = 1;

				if (!profileList.empty())
				{
					myBlendSettings[0].settings = std::dynamic_pointer_cast<PostProcessingProfile>(profileList[0]);
					if (profileList.size() > 1)
					{
						myBlendSettings[1].settings = std::dynamic_pointer_cast<PostProcessingProfile>(profileList[1]);
					}
					else myBlendSettings[1].settings = myBlendSettings[0].settings;
				}

			}
		}
		else myBlendedProfile = nullptr;
	}

	if (!myBlendingEnabled)
	{
		ImGui::SetNextItemWidth(100);
		if (ImGui::Combo("Profile", &profileIndex, profileNames.data(), static_cast<int>(profileNames.size())))
		{
			myProfile = dynamic_pointer_cast<PostProcessingProfile>(profileList[profileIndex]);
		}
	}
	else
	{
		bool calculateResult = false;

		for (int i = 0; i < myBlendSettings.size(); i++)
		{
			int index = -1;
			for (int p = 0; p < profileList.size(); p++)
			{
				if (myBlendSettings[i].settings == profileList[p])
				{
					index = p;
					break;
				}
			}

			ImGui::SetNextItemWidth(100);
			if (ImGui::Combo(("Profile##" + std::to_string(i)).c_str(), &index, profileNames.data(), static_cast<int>(profileNames.size())))
			{
				myBlendSettings[i].settings = dynamic_pointer_cast<PostProcessingProfile>(profileList[index]);
				calculateResult = true;
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(50);
			calculateResult |= Catbox::SliderFloat(("Position##" + std::to_string(i)).c_str(), &myBlendSettings[i].position, 0, 1);
		}

		calculateResult |= Catbox::SliderFloat("Blend value", &myBlendValue, 0, 1);


		if (ImGui::Button("Add"))
		{
			calculateResult = true;
			AddBlendProfile(myBlendSettings[0].settings, 0.5f);
		}

		if (ImGui::Button("Remove"))
		{
			calculateResult = true;
			if (myBlendSettings.size() > 2)
			{
				myBlendSettings.erase(myBlendSettings.end() - 1);
			}
		}

		if (calculateResult) CalculateBlendResult();
	}
}

void PostProcessingVolume::AddBlendProfile(std::shared_ptr<PostProcessingProfile> aProfile, float aPosition)
{
	BlendVolumePoint newBlendPoint;
	newBlendPoint.settings = aProfile;
	newBlendPoint.position = aPosition;

	for (size_t i = 0; i < myBlendSettings.size(); i++)
	{
		if (aPosition >= myBlendSettings[i].position)
		{
			continue;
		}

		myBlendSettings.insert(myBlendSettings.begin() + i, newBlendPoint);
		return;
	}
	myBlendSettings.push_back(newBlendPoint);
}

void PostProcessingVolume::CalculateBlendResult()
{
	auto& blend = myBlendedProfile->mySettings;

	for (size_t i = 0; i < myBlendSettings.size(); i++)
	{
		if (myBlendValue < myBlendSettings[i].position)
		{
			if (!myBlendSettings[i].settings) return;
			if (i == 0)
			{
				blend = myBlendSettings.front().settings->GetSettings();
				return;
			}
			if (!myBlendSettings[i - 1].settings) return;

			auto& blend1 = myBlendSettings[i - 1].settings->GetSettings();
			auto& blend2 = myBlendSettings[i].settings->GetSettings();
			float percent = (myBlendValue - myBlendSettings[i - 1].position) / (myBlendSettings[i].position - myBlendSettings[i - 1].position);
			blend.Tint = Catbox::Lerp(blend1.Tint, blend2.Tint, percent);
			blend.Contrast = Catbox::Lerp(blend1.Contrast, blend2.Contrast, percent);
			blend.Brightness = Catbox::Lerp(blend1.Brightness, blend2.Brightness, percent);
			blend.Saturation = Catbox::Lerp(blend1.Saturation, blend2.Saturation, percent);
			return;
		}
	}

	if (!myBlendSettings.back().settings) return;
	blend = myBlendSettings.back().settings->GetSettings();
}

void PostProcessingVolume::SetProfile(std::shared_ptr<PostProcessingProfile> aProfile)
{
	myProfile = aProfile;
	std::vector<const char*> profileNames;
	auto profileList = AssetRegistry::GetInstance()->GetAssets<PostProcessingProfile>();
	for (auto& p : profileList)
	{
		profileNames.push_back(p->GetName().c_str());
	}
}

const ColorAdjustmentData* PostProcessingVolume::GetPostProcess()
{
	if (!myBlendingEnabled)
	{
		if (!myProfile) return nullptr;
		return &myProfile->GetSettings();
	}
	if (!myBlendedProfile) return nullptr;
	return &myBlendedProfile->GetSettings();
}

const BloomData* PostProcessingVolume::GetBloomSettings()
{
	if (!myProfile) return nullptr;
	return &myProfile->GetBloomSettings();
}

const VignetteData* PostProcessingVolume::GetVignetteSettings()
{
	if (!myProfile) return nullptr;
	return &myProfile->GetVignetteSettings();
}

void PostProcessingVolume::Save(rapidjson::Value& aVolumeData)
{
	auto parser = RapidJsonWrapper::GetInstance();
	auto& alloc = parser->GetDocument().GetAllocator();

	parser->SetTarget(aVolumeData);
	parser->SaveValue<DataType::Float>("Blend", myBlendValue);
	parser->SaveValue<DataType::Bool>("Enabled", myIsEnabled);
	parser->SaveValue<DataType::Bool>("BlendingEnabled", myBlendingEnabled);

	if (myBlendingEnabled)
	{
		auto blendPoints = parser->CreateArray();
		blendPoints.SetArray();
		for (size_t i = 0; i < myBlendSettings.size(); i++)
		{
			auto blendPoint = parser->CreateObject();
			parser->SetTarget(blendPoint);
			const char* profileName = myBlendSettings[i].settings->GetName().c_str();
			auto ref = rapidjson::GenericStringRef(profileName);
			parser->SaveString("Profile", ref);
			parser->SaveValue<DataType::Float>("Pos", myBlendSettings[i].position);
			blendPoints.PushBack(blendPoint, alloc);
		}

		parser->SetTarget(aVolumeData);
		parser->SaveObject("BlendPoints", blendPoints);
	}

	parser->SaveString("Profile", myProfile->myName.c_str());
}

void PostProcessingVolume::Load(rapidjson::Value& aVolumeData)
{
	myBlendValue = aVolumeData["Blend"].GetFloat();
	myIsEnabled = aVolumeData["Enabled"].GetBool();
	myBlendingEnabled = aVolumeData["BlendingEnabled"].GetBool();
	if (myBlendingEnabled)
	{
		myBlendedProfile = std::make_shared<PostProcessingProfile>();
		auto blendPoints = aVolumeData["BlendPoints"].GetArray();
		for (auto& blendPoint : blendPoints)
		{
			auto profile = AssetRegistry::GetInstance()->GetAsset<PostProcessingProfile>(blendPoint["Profile"].GetString());
			if (!profile) continue;
			AddBlendProfile(profile, blendPoint["Pos"].GetFloat());
		}
		CalculateBlendResult();
	}

	if (aVolumeData.HasMember("Profile")) 
	{
		myProfile = AssetRegistry::GetInstance()->GetAsset<PostProcessingProfile>(aVolumeData["Profile"].GetString());
	}
}

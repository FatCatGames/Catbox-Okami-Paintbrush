#include "stdafx.h"
#include "Audio3DComponent.h"

Audio3DComponent::~Audio3DComponent()
{
	for (size_t i = 0; i < myAmountOfSounds; i++)
	{
		Engine::GetInstance()->GetAudioManager()->StopEvent(mySoundEffects[i]);
		Engine::GetInstance()->GetAudioManager()->ReleaseEvent(mySoundEffects[i]);
	}
}

void Audio3DComponent::Awake()
{
	for (size_t i = 0; i < myAmountOfSounds; i++)
	{
		if (!mySoundEffectTypes[i]) continue;
		mySoundEffects[i] = Engine::GetInstance()->GetAudioManager()->CreateEventInstanceBy3DID(mySoundEffectTypes[i]->myId);
		if (myAlwaysActive)
		{
			Engine::GetInstance()->GetAudioManager()->PlayEvent(mySoundEffects[i]);
		}
	}
	doOnce = false;
}

void Audio3DComponent::Update()
{
	if (doOnce)
	{
		Awake();
	}

	for (auto& effects : mySoundEffects)
	{
		Engine::GetInstance()->GetAudioManager()->Set3DSoundPosition(effects, myTransform->worldPos());
	}
}

void Audio3DComponent::AddSoundEffect(AudioManager::SoundAsset* a3DSoundEffect)
{
	myAmountOfSounds++;
	mySoundEffectTypes.push_back(a3DSoundEffect);
	SoundEventInstanceHandle temp = Engine::GetInstance()->GetAudioManager()->CreateEventInstanceBy3DID(a3DSoundEffect->myId);
	mySoundEffects.push_back(temp);
}

void Audio3DComponent::PlaySoundEffect(int aSoundEffect)
{
	if (aSoundEffect >= 0 && aSoundEffect < mySoundEffects.size())
		Engine::GetInstance()->GetAudioManager()->PlayEvent(mySoundEffects[aSoundEffect]);
}

void Audio3DComponent::StopSoundEffect(int aSoundEffect, bool anInstantFlag)
{
	if (aSoundEffect >= 0 && aSoundEffect < mySoundEffects.size())
		Engine::GetInstance()->GetAudioManager()->StopEvent(mySoundEffects[aSoundEffect], anInstantFlag);
}

void Audio3DComponent::StopAllSoundEffect(bool anInstantFlag)
{
	for (size_t i = 0; i < mySoundEffects.size(); i++)
	{
		StopSoundEffect(i, anInstantFlag);
	}
}

void Audio3DComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	if (ImGui::InputInt("AmountOfSounds", &myAmountOfSounds))
	{
		if (myAmountOfSounds < 0) myAmountOfSounds = 0;
		mySoundEffects.resize(myAmountOfSounds);
		mySoundEffectTypes.resize(myAmountOfSounds);
	}
	auto& list3d = Engine::GetInstance()->GetAudioManager()->Get3DList();
	std::vector<const char*> soundStrings;
	//std::vector<AudioManager::eSoundEffects> soundEnums;
	soundStrings.push_back("Nothing");
	//soundEnums.push_back(AudioManager::eSoundEffects::Nothing);
	for (auto& soundAsset : list3d)
	{
		//soundEnums.push_back(soundAsset.first);
		soundStrings.push_back(soundAsset.myShortName.c_str());
	}

	if (myAmountOfSounds == 0)
	{
		ImGui::TextWrapped("This component is for 3D sounds only. For non-spatial sound effects use the Audio Mono Component");
	}

	for (size_t i = 0; i < mySoundEffects.size(); i++)
	{
		int selected = 0;
		//int selected = -1;
		if (mySoundEffectTypes[i])
		{
			selected = mySoundEffectTypes[i]->myId + 1;
		}

		if (ImGui::Combo(("Sound effect " + std::to_string(i)).c_str(), &selected, soundStrings.data(), static_cast<int>(soundStrings.size())))
		{
			if (selected == 0)
			{
				mySoundEffectTypes[i] = nullptr;
			}
			else
			{
				mySoundEffectTypes[i] = &list3d[selected - 1];
			}
		}
	}
	ImGui::Checkbox("AlwaysActive##3d", &myAlwaysActive);
}

void Audio3DComponent::RunInEditor()
{

}

void Audio3DComponent::OnDisable()
{
	for (auto& effects : mySoundEffects)
	{
		Engine::GetInstance()->GetAudioManager()->StopEvent(effects, false);
	}
}

void Audio3DComponent::OnDestroy()
{
	for (auto& effects : mySoundEffects)
	{
		Engine::GetInstance()->GetAudioManager()->StopEvent(effects);
		Engine::GetInstance()->GetAudioManager()->ReleaseEvent(effects);
	}
}

void Audio3DComponent::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();

	wrapper.SaveValue<DataType::Bool>("Active", myAlwaysActive);
	wrapper.SaveValue<DataType::Int>("AmountOfSounds", myAmountOfSounds);


	auto tempArray = wrapper.CreateArray();
	for (auto& soundEffect : mySoundEffectTypes)
	{
		if (soundEffect)
		{
		rapidjson::Value strVal;
		strVal.SetString(soundEffect->myEventPath.c_str(), alloc);
		tempArray.PushBack(strVal, alloc);
		}
		else
		{
			rapidjson::Value strVal;
			strVal.SetString("Nothing", alloc);
			tempArray.PushBack(strVal, alloc);
		}
	}
	wrapper.SaveObject("Effects", tempArray);

}

void Audio3DComponent::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Active"))
	{
		myAlwaysActive = aComponentData["Active"].GetBool();
	}
	if (aComponentData.HasMember("AmountOfSounds"))
	{
		myAmountOfSounds = aComponentData["AmountOfSounds"].GetInt();
		mySoundEffects.resize(myAmountOfSounds);
		mySoundEffectTypes.resize(myAmountOfSounds);
	}
	if (aComponentData.HasMember("Effects"))
	{
		if (aComponentData["Effects"].Size() >= mySoundEffectTypes.size())
		{
			for (size_t i = 0; i < mySoundEffectTypes.size(); i++)
			{
				mySoundEffectTypes[i] = Engine::GetInstance()->GetAudioManager()->Get3DSoundAssetByPath(aComponentData["Effects"][i].GetString());
			}
		}
	}
	doOnce = true;
}

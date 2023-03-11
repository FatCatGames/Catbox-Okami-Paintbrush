#include "stdafx.h"
#include "AudioMonoComponent.h"

AudioMonoComponent::~AudioMonoComponent()
{
	for (size_t i = 0; i < myAmountOfSounds; i++)
	{
		Engine::GetInstance()->GetAudioManager()->StopEvent(mySoundEffects[i]);
		Engine::GetInstance()->GetAudioManager()->ReleaseEvent(mySoundEffects[i]);
	}
}

void AudioMonoComponent::Awake()
{
	for (size_t i = 0; i < myAmountOfSounds; i++)
	{
		if (!mySoundEffectTypes[i]) continue;
		mySoundEffects[i] = Engine::GetInstance()->GetAudioManager()->CreateEventInstanceByNormalID(mySoundEffectTypes[i]->myId);
		if (myAlwaysActive)
		{
			Engine::GetInstance()->GetAudioManager()->PlayEvent(mySoundEffects[i]);
		}
	}
	doOnce = false;
}

void AudioMonoComponent::Update()
{
	//If it breaks for some dumb reason, uncomment this.

	if (doOnce)
	{
		Awake();
	}
}

void AudioMonoComponent::AddSoundEffect(AudioManager::SoundAsset* aMonoSoundEffect)
{
	myAmountOfSounds++;
	mySoundEffectTypes.push_back(aMonoSoundEffect);
	SoundEventInstanceHandle temp = Engine::GetInstance()->GetAudioManager()->CreateEventInstanceByNormalID(aMonoSoundEffect->myId);
	mySoundEffects.push_back(temp);
}

void AudioMonoComponent::PlaySoundEffect(int aSoundEffect)
{
	if (aSoundEffect >= 0 && aSoundEffect < mySoundEffects.size())
		Engine::GetInstance()->GetAudioManager()->PlayEvent(mySoundEffects[aSoundEffect]);
}

void AudioMonoComponent::PlaySoundEffectAsOneShot(int aSoundEffect)
{
	if (aSoundEffect >= 0 && aSoundEffect < mySoundEffects.size())
		Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByID(mySoundEffectTypes[aSoundEffect]->myId);
}

void AudioMonoComponent::StopSoundEffect(int aSoundEffect, bool anInstantFlag)
{
	if (aSoundEffect >= 0 && aSoundEffect < mySoundEffects.size())
		Engine::GetInstance()->GetAudioManager()->StopEvent(mySoundEffects[aSoundEffect], anInstantFlag);
}

void AudioMonoComponent::StopAllSoundEffect(bool anInstantFlag)
{
	for (size_t i = 0; i < mySoundEffects.size(); i++)
	{
		StopSoundEffect(i, anInstantFlag);
	}
}

void AudioMonoComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	if (ImGui::InputInt("AmountOfSounds", &myAmountOfSounds))
	{
		if (myAmountOfSounds < 0) myAmountOfSounds = 0;
		mySoundEffects.resize(myAmountOfSounds);
		mySoundEffectTypes.resize(myAmountOfSounds);
	}
	auto& list = Engine::GetInstance()->GetAudioManager()->GetMonoList();
	std::vector<const char*> soundStrings;
	//std::vector<AudioManager::eSoundEffects> soundEnums;
	soundStrings.push_back("Nothing");
	//soundEnums.push_back(AudioManager::eSoundEffects::Nothing);
	for (auto& soundAsset : list)
	{
		//soundEnums.push_back(soundAsset.first);
		soundStrings.push_back(soundAsset.myShortName.c_str());
	}

	if (myAmountOfSounds == 0)
	{
		ImGui::TextWrapped("This component is for sounds that are non-spatial. For 3D sound effects use the Audio 3D component");
	}

	for (size_t i = 0; i < mySoundEffects.size(); i++)
	{
		int selected = 0;

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
				mySoundEffectTypes[i] = &list[selected - 1];
			}
		}
	}

	ImGui::Checkbox("AlwaysActive##3d", &myAlwaysActive);
	ImGui::Text("Testing Sounds");

	if(ImGui::Combo((" "), &mySoundToPreview, soundStrings.data(), static_cast<int>(soundStrings.size())))
	{

	}

	if(ImGui::Button("Play"))
	{
		Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByID(mySoundToPreview - 1);
	}

	ImGui::SameLine();
	if(ImGui::Button("Stop"))
	{
		Engine::GetInstance()->GetAudioManager()->StopAllSound();
		Engine::GetInstance()->GetAudioManager()->StopMusic();
	}

	//if (myAmountOfSounds > 0)
	//{
	// //This is just to test parameters. Leave it for now in case I need to test again.
	//	if (ImGui::Button("Press here"))
	//	{
	//		printmsg(std::to_string(Engine::GetInstance()->GetAudioManager()->GetAmountOfParameters(mySoundEffects[0])));
	//		printmsg(Engine::GetInstance()->GetAudioManager()->GetParameterNameByIndex(mySoundEffects[0], -1));
	//		printmsg(Engine::GetInstance()->GetAudioManager()->GetParameterNameByIndex(mySoundEffects[0], 0));
	//		printmsg(Engine::GetInstance()->GetAudioManager()->GetParameterNameByIndex(mySoundEffects[0], 1));
	//		printmsg(Engine::GetInstance()->GetAudioManager()->GetParameterNameByIndex(mySoundEffects[0], 2));

	//		//Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(mySoundEffects[0], 0, 100000);
	//		//Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(mySoundEffects[0], 0, 1);
	//		//Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(mySoundEffects[0], 4, 1);
	//		Engine::GetInstance()->GetAudioManager()->SetParameterByName(mySoundEffects[0], "Player HP", 10);
	//		Engine::GetInstance()->GetAudioManager()->SetParameterByIndex(mySoundEffects[0], 1, 10);
	//		Engine::GetInstance()->GetAudioManager()->SetGlobalParameter(Engine::GetInstance()->GetAudioManager()->GetParameterNameByIndex(mySoundEffects[0], 1), 10);

	//	}
	//}
}

void AudioMonoComponent::RunInEditor()
{

}

void AudioMonoComponent::OnDisable()
{
	for (auto& effects : mySoundEffects)
	{
		Engine::GetInstance()->GetAudioManager()->StopEvent(effects, false);
	}
}

void AudioMonoComponent::OnDestroy()
{
	for (auto& effects : mySoundEffects)
	{
		Engine::GetInstance()->GetAudioManager()->StopEvent(effects);
		Engine::GetInstance()->GetAudioManager()->ReleaseEvent(effects);
	}
}

void AudioMonoComponent::Save(rapidjson::Value& aComponentData)
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

void AudioMonoComponent::Load(rapidjson::Value& aComponentData)
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
				mySoundEffectTypes[i] = Engine::GetInstance()->GetAudioManager()->GetMonoSoundAssetByPath(aComponentData["Effects"][i].GetString());
			}
		}
	}
	doOnce = true;
}

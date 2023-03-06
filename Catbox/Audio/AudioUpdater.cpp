#include "stdafx.h"
#include "AudioUpdater.h"
//
//void AudioUpdater::Init()
//{
//	myAudio = &GameManager::GetInstance()->GetAudio();
//	myMasterVolume = myAudio->GetMasterVolume();
//	myMusicVolume = myAudio->GetMusicVolume();
//}
//
//void AudioUpdater::Awake()
//{
//	if (myAudio)
//	{
//		myAudio->PlayMusic(AudioManager::eMusic::Nothing);
//		myAudio->PlayMusic(myCurrentSong);
//		GameManager::GetInstance()->GetAudio().PauseSoundEffects(false); //safety to make sure sound is always playing in a new scene.
//	}
//}
//
//void AudioUpdater::Update()
//{
//	if (myAudio)
//	{
//		myAudio->Update();
//	}
//}
//
//void AudioUpdater::RenderInProperties()
//{
//	const char* items[] = { "Nothing", "ForestSounds", "VillageSounds", "MountainSounds", "BossSounds", "ForestAmbience" };
//	if (ImGui::Combo("Level theme", &mySelectedSound, items, IM_ARRAYSIZE(items)))
//	{
//		if (mySelectedSound == 0)
//			myCurrentSong = AudioManager::eMusic::Nothing;
//		if (mySelectedSound == 1)
//			myCurrentSong = AudioManager::eMusic::Forest;
//		if (mySelectedSound == 2)
//			myCurrentSong = AudioManager::eMusic::Village;
//		if (mySelectedSound == 3)
//			myCurrentSong = AudioManager::eMusic::Mountain;
//		if (mySelectedSound == 4)
//			myCurrentSong = AudioManager::eMusic::Boss;
//		if (mySelectedSound == 5)
//			myCurrentSong = AudioManager::eMusic::MainMenu;
//	}
//
//	if (ImGui::Button("Restart music"))
//	{
//		myAudio->PlayMusic(AudioManager::eMusic::Nothing);
//		myAudio->PlayMusic(myCurrentSong);
//	}
//
//	if (mySelectedSound != 0)
//	{
//		if (mySelectedSound < 4)
//		{
//			ImGui::SameLine();
//
//			if (ImGui::Button("Normal mode"))
//			{
//				myAudio->SetFightMusic(false);
//			}
//
//			ImGui::SameLine();
//
//			if (ImGui::Button("Battle mode"))
//			{
//				myAudio->SetFightMusic(true);
//			}
//		}
//
//		if (mySelectedSound == 4)
//		{
//			if (ImGui::Button("Phase 1"))
//			{
//				myAudio->SetBossMusic(1);
//			}
//
//			ImGui::SameLine();
//
//			if (ImGui::Button("Phase 2"))
//			{
//				myAudio->SetBossMusic(2);
//			}
//
//			ImGui::SameLine();
//
//			if (ImGui::Button("Phase 3"))
//			{
//				myAudio->SetBossMusic(3);
//			}
//		}
//	}
//
//	static int aSoundEffect = 0;
//
//	ImGui::TextWrapped("3D effects played through the sound test\nwill be at 0,0,0. Only works in play mode.");
//
//	ImGui::DragInt("Effect 0-32ish", &aSoundEffect, 0.1f, 0, 32);
//
//	if (ImGui::Button("PlayEffect"))
//	{
//		myAudio->PlaySoundEffect(static_cast<AudioManager::eSoundEffects>(aSoundEffect));
//	}
//
//	ImGui::SameLine();
//
//	if (ImGui::Button("Stop all sounds"))
//	{
//		myAudio->StopAllSound();
//	}
//
//	ImGui::DragInt("MasterVolume", &myMasterVolume, 0.1f, 0, 9);
//
//	if (ImGui::Button("Set Master"))
//	{
//		myAudio->SetMasterVolume(myMasterVolume);
//	}
//
//	ImGui::DragInt("Music Volume", &myMusicVolume, 0.1f, 0, 9);
//
//	if (ImGui::Button("Set Music"))
//	{
//		myAudio->SetMusicVolume(myMusicVolume);
//	}
//
//	ImGui::DragInt("HP Test", &myTestHP, 0.1f, 0, 100);
//
//	if (ImGui::Button("Set Health"))
//	{
//		myAudio->SetHealthParam(myTestHP);
//	}
//
//}
//
//void AudioUpdater::RunInEditor()
//{
//	GameManager::GetInstance()->GetAudio().StopAllSound();
//	myAudio->Update();
//}
//
//void AudioUpdater::OnDisable()
//{
//
//}
//
//void AudioUpdater::Save(rapidjson::Value& aComponentData)
//{
//	auto& wrapper = *RapidJsonWrapper::GetInstance();
//	auto& alloc = wrapper.GetDocument().GetAllocator();
//
//	wrapper.SaveValue<DataType::Int>("Theme", mySelectedSound);
//}
//
//void AudioUpdater::Load(rapidjson::Value& aComponentData)
//{
//	if (aComponentData.HasMember("Theme"))
//	{
//		mySelectedSound = aComponentData["Theme"].GetInt();
//		myCurrentSong = static_cast<AudioManager::eMusic>(mySelectedSound);
//	}
//}

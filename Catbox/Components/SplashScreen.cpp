#include "stdafx.h"
#include "SplashScreen.h"
#include "Components/UI/VideoPlayer.h"
#include "ComponentTools/SceneManager.h"
#include "Assets/AssetLoader.h"
#include "ComponentTools/UIEventHandler.h"

void SplashScreen::Init()
{
}

void SplashScreen::Awake()
{
}

void SplashScreen::Update()
{
	if (myGameObject->GetComponent<VideoPlayer>()->GetVideoState() == VideoPlayer::VideoState::Stopped && AssetLoader::GetIsThreadActive() == false)
	{
		//Engine::GetInstance()->GetSceneManager()->LoadScene(AssetRegistry::GetInstance()->GetAsset<Scene>(mySceneToLoad));
		UIEventHandler::SetNextLevel(mySceneToLoad);
	}
}

void SplashScreen::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputText("Scene To Load", &mySceneToLoad);
}

void SplashScreen::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveString("SceneToLoad", mySceneToLoad.c_str());
}

void SplashScreen::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("SceneToLoad")) 
	{
		mySceneToLoad = aComponentData["SceneToLoad"].GetString();
	}
}

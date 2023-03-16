#include "Game.pch.h"
#include "Project.h"
#include "Components\Physics\Collisions\CollisionManager.h"
#include <Components/3D/ModelInstance.h>
#include "Assets/AssetLoader.h"
#include "Assets/Material.h"
#include <Components/Animator.h>
#include "Assets\Model.h"
#include "Components\3D\Light.h"
#include "Components/ParticleSystem.h"
#include <CommonUtilities/InputHandler.h>
#include "Components\ComponentHandler.h"
#include "ComponentTools\SceneManager.h"
#include "Managers/GameManager.h"
#include "Managers\NetMessageHandler.h"

#include "Player\PlayerCamera.h"
#include "Player\PlayerController.h"
#include "LookAtCamera.h"
#include "Paintbrush.h"
#include "CanvasPS.h"
#include "Canvas.h"
#include "GameScene.h"
#include "PaintingScene.h"
#include "Bomb.h"
#include "Tree.h"

//Not too fond of settings not loading correctly
#include "ProjectSettings.h"

std::shared_ptr<GameManager> Project::gameManager;

void Project::Setup()
{
#pragma region Components setup

	ComponentHandler::RegisterComponent<PlayerCamera>();
	ComponentHandler::RegisterComponent<PlayerController>();
	ComponentHandler::RegisterComponent<LookAtCamera>();
	ComponentHandler::RegisterComponent<Paintbrush>();
	ComponentHandler::RegisterComponent<Canvas>();
	ComponentHandler::RegisterComponent<GameScene>();
	ComponentHandler::RegisterComponent<PaintingScene>();
	ComponentHandler::RegisterComponent<Bomb>();
	ComponentHandler::RegisterComponent<Tree>();

#pragma endregion

	shared_ptr<CanvasPS> brushPS = std::make_shared<CanvasPS>();
	CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/CanvasPS.sh", brushPS);

	gameManager = std::make_shared<GameManager>();

	Engine::GetInstance()->GetCollisionManager()->AddLayer("Player");
	Engine::GetInstance()->GetCollisionManager()->AddLayer("Interactable");
	Engine::GetInstance()->GetCollisionManager()->AddLayer("Tree");
	ProjectSettings::LoadProjectSettings();


	if (!EDITORMODE) 
	{
		AssetLoader::LoadAsset("Assets/Textures/UIProggArt/LoadingText.dds");
		AssetLoader::LoadAsset("Assets/Scenes/SplashScreen.scene");
		AssetLoader::LoadAllAssets();

		auto& worldInstancing = AssetRegistry::GetInstance()->GetAsset<Scene>("MainMenu_lvl");
		Engine::GetInstance()->GetSceneManager()->LoadScene(worldInstancing);
	}
}


void Project::Update()
{
	
}

void Project::Start()
{
}

#include "stdafx.h"
#include "Engine.h"
#include "ComponentTools/SceneManager.h"
#include "../GameObjects/GameObjectFactory.h"
#include <iostream>
#include "../CommonUtilities/InputHandler.h"
#include "CommonUtilities\RapidJsonWrapper.h"
#include "ComponentTools\UIEventHandler.h"
#include "Physics/Collisions/CollisionManager.h"
#include "Components/ComponentHandler.h"
#include "Editor\Windows\SceneWindow.h"
#include "Audio/AudioManager.h"
#include "ComponentTools\ThreadPool.h"
#include "ProjectSettings.h"
#include "../Game/Project.h"
#include "CameraController.h"
#include "Physics/PhysicsEngine.h"
#include "TimerManager.h"
#include "Networking/NetworkManager.h"

Engine* Engine::myInstance = nullptr;

Engine::Engine()
{
	if (!myInstance)
	{
		myInstance = this;
		myGraphicsEngine = std::make_shared<GraphicsEngine>();
		myAssetRegistry = std::make_shared<AssetRegistry>();
		myGameObjectFactory = std::make_shared <GameObjectFactory>();
		mySceneManager = std::make_shared<SceneManager>();
		myRapidJsonWrapper = std::make_shared<RapidJsonWrapper>();
		myCollisionManager = std::make_shared<CollisionManager>();
		myAudioManager = std::make_shared<AudioManager>();
		myThreadPool = std::make_shared<ThreadPool>();
		myThreadPool->Start();
		myCameraController = std::make_shared<CameraController>();
		myPhysicsEngine = std::make_shared<PhysicsEngine>();
		ComponentHandler::ManuallyRegisterComponents();
		UIEventHandler::Init();
		ProjectSettings::LoadProjectSettings();
		ProjectSettings::LoadKeyBinds();
	}
}

Engine::~Engine()
{
	if (myNetworkManger) {
		myNetworkManger->Disconnect();
		myNetworkManger.reset();
	}
}

void Engine::Init(bool aIsEditorMode)
{
	myAssetRegistry->LoadAssets();
	myGameObjectFactory->Init(mySceneManager.get());
	myPhysicsEngine->Init();
	myIsEditorMode = aIsEditorMode;
}
std::shared_ptr<CatNet::NetworkManager> Engine::GetNetworkManager()
{
	if (!myNetworkManger)
	{
		myNetworkManger = std::make_shared<CatNet::NetworkManager>();
	}
	
	return myNetworkManger;
}
GameObject* Engine::GetGameObject(int id)
{
	return myInstance->myGameObjectFactory->GetObjectById(id);
}

void Engine::RemoveGameObject(int id)
{
	myInstance->myGameObjectFactory->RemoveObjectById(id);
}

void Engine::SetMainCamera(Camera* aCamera)
{
	myMainCamera = aCamera;
	SetActiveCamera(aCamera);
}

void Engine::SetActiveCamera(Camera* aCamera)
{
	myActiveCamera = aCamera;
	myGraphicsEngine->SetCamera(myActiveCamera);
}

bool Engine::IsGameRunning()
{
	return myIsGameRunning && !Engine::GetInstance()->GetSceneManager()->GetIsLoading();
}

void Engine::SetGamePaused(bool aIsPaused)
{
	myTimer.SetPaused(aIsPaused);
}

bool Engine::IsGamePaused()
{
	return myTimer.IsPaused();
}

bool Engine::IsSceneLoaded()
{
	if (mySceneManager->GetCurrentScene())
	{
		return !mySceneManager->GetIsLoading();
	}
	return false;
}

void Engine::StartPlayMode()
{
	//RapidJsonWrapper::GetInstance()->ClearDocument();
	myIsGameRunning = true;

	if (EDITORMODE)
	{
		print("Play mode!");
		mySceneManager->GetCurrentScene()->TempSave();
	}
	Project::Start();
	myCameraController->Clear();
	mySceneManager->StartGame();
	myGraphicsEngine->SetCamera(myActiveCamera);
}

void Engine::StopPlayMode()
{
	print("Stop play mode!");
	myGameObjectFactory->ClearAllObjects();
	myPhysicsEngine->ResetScene();
	myGraphicsEngine->EmptyVectors();
	myAudioManager->StopAllSound();
	myIsGameRunning = false;
	myTimer.SetPaused(false);
	myGraphicsEngine->SetCamera(Editor::GetInstance()->GetEditorCamera().GetCamera());
	myCameraController->Clear();
	mySceneManager->GetCurrentScene()->Unload();
	mySceneManager->GetCurrentScene()->TempLoad();
	Editor::GetInstance()->ClearAllSelectedObjects();
	Editor::GetInstance()->ResetHierarchy();
	myCollisionManager->ClearColliders();
	myMainCamera = nullptr;
	myActiveCamera = nullptr;
}

void Engine::ShutDown()
{
	myGameObjectFactory->ClearAllObjects();
}


void Engine::Update()
{
	Input::Update();
	myTimer.Update();
	TimerManager::Update(deltaTime);
	myAudioManager->Update();
	mySceneManager->Update();
	if (myNetworkManger)
	{
		myNetworkManger->Update();
	}
	if (EDITORMODE)
	{
		//myPhysicsEngine->DrawLines();
	}
	if (PLAYMODE)
	{
		myPhysicsEngine->Update();
		mySceneManager->FixedUpdate();	
		UIEventHandler::Update();
		myCollisionManager->UpdateCollisions();
		myCameraController->Update();
	}
}


Vector2i Engine::ViewportToScreenPos(int anX, int anY)
{
	if (anX < 0) anX = 0;
	if (anY < 0) anY = 0;

	if (EDITORMODE)
	{
		std::vector<SceneWindow*> sceneWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<SceneWindow>(WindowType::Scene);
		if (sceneWindows.size() == 0) return { anX, anY };
		SceneWindow* viewPort = sceneWindows[0];

		//I want to die :)
		Vector2f viewPortPos = viewPort->GetPosition();
		if (viewPortPos.x < 0)
		{
			viewPortPos.x += DX11::GetResolution().x;
		}
		else if (viewPortPos.x > DX11::GetResolution().x)
		{
			viewPortPos.x -= DX11::GetResolution().x;
		}
		/*
		if (viewPortPos.y < 0)
		{
			viewPortPos.y += DX11::GetResolution().y;
		}
		else if (viewPortPos.y > DX11::GetResolution().y)
		{
			viewPortPos.y -= DX11::GetResolution().y;
		}*/
		anX -= viewPortPos.x;
		anY -= viewPortPos.y;

		const int width = DX11::GetResolution().x;
		const int height = DX11::GetResolution().y;
		anX *= width / viewPort->GetSize().x;
		anY *= height / viewPort->GetSize().y;
		//hardcoded values to account for borders
		anX -= DX11::GetResolution().x * 0.005f;
		anY -= DX11::GetResolution().y * 0.046f;
	}

	return { anX, anY };
}

Vector2i Engine::ViewportToScreenPos(Vector2i aScreenPos)
{
	return ViewportToScreenPos(aScreenPos.x, aScreenPos.y);
}

Vector2i Engine::ScreenToViewportPos(Vector2i aScreenPos)
{
	if (aScreenPos.x < 0) aScreenPos.x = 0;
	if (aScreenPos.y < 0) aScreenPos.y = 0;

	if (EDITORMODE)
	{
		std::vector<SceneWindow*> sceneWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<SceneWindow>(WindowType::Scene);
		if (sceneWindows.size() == 0) return { aScreenPos.x, aScreenPos.y };
		SceneWindow* viewPort = sceneWindows[0];

		Vector2f viewPortPos = viewPort->GetPosition();
		if (viewPortPos.x < 0)
		{
			viewPortPos.x += DX11::GetResolution().x;
		}
		else if (viewPortPos.x > DX11::GetResolution().x)
		{
			viewPortPos.x -= DX11::GetResolution().x;
		}

		aScreenPos.x -= viewPortPos.x;
		aScreenPos.y -= viewPortPos.y;

		const int width = DX11::GetResolution().x;
		const int height = DX11::GetResolution().y;

		// undo viewport-to-screen scaling
		aScreenPos.x = static_cast<int>(aScreenPos.x * viewPort->GetSize().x / static_cast<float>(width));
		aScreenPos.y = static_cast<int>(aScreenPos.y * viewPort->GetSize().y / static_cast<float>(height));

		// add viewport position
		aScreenPos.x += static_cast<int>(viewPortPos.x);
		aScreenPos.y += static_cast<int>(viewPortPos.y);

		// adjust for negative or out-of-bounds viewport position
		if (viewPortPos.x < 0) {
			aScreenPos.x -= width;
		}
		else if (viewPortPos.x > width) {
			aScreenPos.x += width;
		}

		if (viewPortPos.y < 0) {
			aScreenPos.y -= height;
		}
		else if (viewPortPos.y > height) {
			aScreenPos.y += height;
		}

		// adjust for hardcoded values
		aScreenPos.x -= static_cast<int>(width * 0.005f);
		aScreenPos.y -= static_cast<int>(height * 0.046f);
	}

	return { aScreenPos.x, aScreenPos.y };
}


const float Engine::GetDeltaTime()
{
	return myTimer.GetDeltaTime();
}

const double Engine::GetTotalTime()
{
	return myTimer.GetTotalTime();
}

bool Engine::IsViewportHovered()
{
	if (!EDITORMODE) return true;
	auto viewport = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneWindow>(WindowType::Scene);
	if (viewport && !viewport->IsHovered())
	{
		return false;
	}
	return true;
}
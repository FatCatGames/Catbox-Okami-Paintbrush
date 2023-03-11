#pragma once
#include "CommonUtilities/Timer.h"
#include "GameObjects/GameObjectFactory.h"
#include "Assets\Prefab.h"

class Camera;
class Transform;
class SceneManager;
class GraphicsEngine;
class JsonParser;
class GameObject;
class CollisionManager;
class AudioManager;
class GraphicsEngine;
class AssetRegistry;
class RapidJsonWrapper;
class ThreadPool;
class CameraController;
class PhysicsEngine;
namespace CatNet 
{
	class NetworkManager;
}


class Engine
{
public:
	static Engine* GetInstance() { return myInstance; }
	Engine();
	~Engine();
	void Init(bool aIsEditorMode);

	std::shared_ptr<SceneManager> GetSceneManager() const { return mySceneManager; }
	std::shared_ptr<GameObjectFactory> GetGameObjectFactory() const { return myGameObjectFactory; }
	std::shared_ptr<GraphicsEngine> GetGraphicsEngine() const { return myGraphicsEngine; }
	std::shared_ptr<CollisionManager> GetCollisionManager() const { return myCollisionManager; }
	std::shared_ptr<AudioManager> GetAudioManager() const { return myAudioManager; }
	std::shared_ptr<ThreadPool> GetThreadPool() const { return myThreadPool; }
	std::shared_ptr<CameraController> GetCameraController() const { return myCameraController; }
	std::shared_ptr<PhysicsEngine> GetPhysicsEngine() const { return myPhysicsEngine; }
	std::shared_ptr<CatNet::NetworkManager> GetNetworkManager();
	static GameObject* GetGameObject(int id);
	static void RemoveGameObject(int id);
	void SetMainCamera(Camera* aCamera);
	inline Camera* GetMainCamera() { return myMainCamera; }
	void SetActiveCamera(Camera* aCamera);
	inline Camera* GetActiveCamera() { return myActiveCamera; }
	inline bool IsEditorMode() { return myIsEditorMode; }
	bool IsGameRunning();
	void SetGamePaused(bool aIsPaused);
	bool IsGamePaused();
	bool IsSceneLoaded();
	void StartPlayMode();
	void StopPlayMode();
	void ShutDown();
	
	//This is for when we have a console window
	//static void printmsg(std::string aMsg);
	//static void PrintError(std::string aMsg);

	void SetWindowSize(const int& aWidth, const int& aHeight) { myWindowSize = { aWidth, aHeight }; }
	Vector2i GetWindowSize() { return myWindowSize; }
	Vector2i ViewportToScreenPos(int anX, int anY);
	Vector2i ViewportToScreenPos(Vector2i aScreenPos);
	Vector2i ScreenToViewportPos(Vector2i aScreenPos);
	bool IsViewportHovered();
	float GetWindowRatio() { return myWindowSize.x / (float)myWindowSize.y; }

	const float GetDeltaTime();
	const double GetTotalTime();
	void Update();

private:
	static Engine* myInstance;
	std::shared_ptr<SceneManager> mySceneManager;
	std::shared_ptr<GameObjectFactory> myGameObjectFactory;
	std::shared_ptr<GraphicsEngine> myGraphicsEngine;
	std::shared_ptr<CollisionManager> myCollisionManager;
	std::shared_ptr<AudioManager> myAudioManager;
	std::shared_ptr<AssetRegistry> myAssetRegistry;
	std::shared_ptr<RapidJsonWrapper> myRapidJsonWrapper;
	std::shared_ptr<ThreadPool> myThreadPool;
	std::shared_ptr<CameraController> myCameraController;
	std::shared_ptr<PhysicsEngine> myPhysicsEngine;
	std::shared_ptr<CatNet::NetworkManager> myNetworkManger;
	Camera* myMainCamera = nullptr;
	Camera* myActiveCamera = nullptr;
	Catbox::Timer myTimer;
	Vector2i myWindowSize;
	bool myIsEditorMode = false;
	bool myIsGameRunning = false;
};


static GameObject* Instantiate()
{
	return Engine::GetInstance()->GetGameObjectFactory()->CreateGameObject();
}

static GameObject* InstantiatePrefab(const std::string& aName)
{
	return Engine::GetInstance()->GetGameObjectFactory()->CreateGameObjectWithPrefab(aName);
}

static GameObject* InstantiatePrefab(std::shared_ptr<Prefab> aPrefab)
{
	return Engine::GetInstance()->GetGameObjectFactory()->CreateGameObjectWithPrefab(aPrefab->GetName());
}

static GameObject* Instantiate(Transform* aParent)
{
	return Engine::GetInstance()->GetGameObjectFactory()->CreateGameObject(aParent);
}
#pragma once
#include "Assets/Scene.h"

class GraphicsEngine;
class GameObjectFactory;
class SceneManager
{
public:
	SceneManager() = default;
	~SceneManager();

	void LoadScene(std::shared_ptr<Scene> aScene);
	bool GetIsLoading();
	Scene* GetCurrentScene();
	inline std::vector<std::shared_ptr<Scene>> GetAllScenes() { return myScenes; }
	int GetActiveSceneIndex();
	void Update();
	void FixedUpdate();
	void SetPaused(bool aPaused);
	void StartGame(); //for playtesting in editor only

	bool GameIsPaused() const;
	bool SceneIsLoading() { return myIsLoadingScene; }

	void AddOnSceneLoadedListener(Listener& aListener);

private:

	void ThreadLoadScene();

	Event myOnSceneLoadedEvent;
	std::shared_ptr<Scene> myCurrentScene = nullptr;
	std::vector<std::shared_ptr<Scene>> myScenes;
	std::shared_ptr<Scene> mySceneToLoad = nullptr;
	float myFixedUpdateTimer = 0;
	bool myThreadActive = false;
	bool myIsLoading = false;
	bool myPaused = false;
	int myIndexToLoad = 0;
	int myInstantiatingIndex = 0;
	bool myIsLoadingScene = false;
};
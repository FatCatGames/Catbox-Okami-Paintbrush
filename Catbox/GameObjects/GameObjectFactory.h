#pragma once
#include "GameObject.h"
#include <map>

class SceneManager;
class Scene;
class GameObjectFactory
{
public:
	GameObjectFactory() = default;
	~GameObjectFactory() = default;
	void Init(SceneManager* aSceneManager);
	GameObject* CreateGameObject();
	GameObject* CreateGameObject(Transform* aParent);
	GameObject* CreateGameObjectWithId(int anId);
	GameObject* CreateGameObjectWithPrefab(const std::string& aName);
	GameObject* GetObjectById(int id);
	void ClearAllObjects();
	void RemoveObjectById(int id);
	void SetSceneToInstantiateTo(Scene* aScene);

private:
	std::map<int, GameObject*> myObjects;
	//int myEntitiesCount;
	int myObjectIdIndex;
	SceneManager* mySceneManager;
	Scene* mySceneToInstantiateTo = nullptr;
};
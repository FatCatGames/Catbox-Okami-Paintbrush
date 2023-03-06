#include "stdafx.h"
#include "GameObjectFactory.h"
#include "../ComponentTools/SceneManager.h"
#include "Assets\Prefab.h"

void GameObjectFactory::Init(SceneManager* aSceneManager)
{
	mySceneManager = aSceneManager;
	myObjects = std::map<int, GameObject*>();
}

GameObject* GameObjectFactory::CreateGameObject()
{
	GameObject* newGameObject = new GameObject();
	newGameObject->SetObjectInstanceID(++myObjectIdIndex);
	myObjects.insert({ myObjectIdIndex, newGameObject });
	mySceneToInstantiateTo->AddRootObject(newGameObject);
	return newGameObject;
}

GameObject* GameObjectFactory::CreateGameObject(Transform* aParent)
{
	GameObject* newGameObject = new GameObject();
	newGameObject->SetObjectInstanceID(++myObjectIdIndex);
	myObjects.insert({ myObjectIdIndex, newGameObject });
	newGameObject->GetTransform()->SetWorldPos(aParent->worldPos());
	newGameObject->GetTransform()->SetParent(aParent);
	return newGameObject;
}
GameObject* GameObjectFactory::CreateGameObjectWithId(int anId)
{
	GameObject* newGameObject = new GameObject();
	newGameObject->SetObjectInstanceID(anId);
	myObjects[anId] = newGameObject;
	mySceneToInstantiateTo->AddRootObject(newGameObject);
	return newGameObject;
}

GameObject* GameObjectFactory::CreateGameObjectWithPrefab(const std::string& aName)
{
	auto prefab = AssetRegistry::GetInstance()->GetAsset<Prefab>(aName);
	if (!prefab)
	{
		printerror("Tried to instantiate prefab " + aName + " but it does not exist");
		return nullptr;
	}

	GameObject* newObj = Instantiate();
	newObj->LoadFromPath(prefab->GetPath().string().c_str());
	newObj->SetPrefab(prefab);
	return newObj;
}

GameObject* GameObjectFactory::GetObjectById(int id) {
	return myObjects[id];
}

void GameObjectFactory::ClearAllObjects()
{
	for (auto& [id, object] : myObjects)
	{
		delete object;
	}
	myObjects.clear();
}

void GameObjectFactory::RemoveObjectById(int id)
{
	myObjects.erase(id);
}

void GameObjectFactory::SetSceneToInstantiateTo(Scene* aScene)
{
	myObjects.clear();
	mySceneToInstantiateTo = aScene;
}

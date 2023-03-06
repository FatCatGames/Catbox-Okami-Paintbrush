#pragma once
#include "Asset.h"
#include <functional>
#include <vector>
#include <unordered_map>
#include "Graphics/Rendering/SceneLightData.h"

struct LightData;
class Camera;
class GameObjectFactory;
class GameObject;
class Light;

class Scene : public Asset
{
public:
	Scene() = default;
	~Scene();
	void Update();
	void FixedUpdate();
	void LoadScene();
	void ImportScene();
	void LoadFromPath(const char* aPath) override;
	void SaveAsset(const char* aPath) override;
	void Unload();
	void AddRootObject(GameObject* anObject);
	void RemoveRootObject(GameObject* anObject);
	inline std::vector<GameObject*>& GetRootObjects() { return myRootObjects; }
	void AddLight(Light* aLight);
	void RemoveLight(Light* aLight);
	void ChangeLightType(Light* aLight, int aNewType);
	void StartGameObjects();
	void TempSave();
	void TempLoad(); 
	void DoubleClickAsset() override;
	GameObject* FindGameObject(const std::string& aName);

protected:
	friend class SceneHierarchyWindow;
	friend class GameObjectFactory;
	std::vector<GameObject*> myRootObjects;
	std::function<void()> OnSceneLoaded;
	SceneLightData myLightData;
	std::vector<Light*> myLights;
	bool myShouldTempSave = false;
	Light* myEnvLight;
};
#pragma once
#include <typeinfo>
#include "Transform.h"
#include <string>
#include "Assets/Asset.h"


struct SkeletonInstance;
class Prefab;
class Component;
class Collider;
class GameObjectFactory;
class GameObject : public Asset
{
public:
	GameObject();
	~GameObject();
	void FixedUpdate();
	void UpdateTransform();
	void Destroy();
	void SetObjectInstanceID(int anID);
	const int& GetObjectInstanceID();
	Transform* GetTransform();
	//Legacy Collisions
	void OnCollisionEnter(Collider* someColission);
	void OnTriggerEnter(Collider* someColission);
	void OnCollisionStay(Collider* someColission);
	void OnCollisionExit(Collider* someColission);
	void OnOverlapBegin(Collider* someColission);
	void OnOverlap(Collider* someColission);
	void OnOverlapEnd(Collider* someColission);

	bool operator==(GameObject& aRhs);
	bool IsActive();
	void SetActive(bool aIsActiveFlag);
	void SetTag(const char* aTag);
	void SetName(const char* aName);
	const std::string& GetTag();
	const std::string& GetName();
	float GetAliveTime() const { return myAliveTime; }
	void SetAliveTime(float aTime) { myAliveTime = aTime; }

	template<class T, class... Args>
	T* AddComponent(Args&&... args);
	void RemoveComponent(Component* aComponent);

	template<class T>
	T* GetComponent();
	inline std::vector<Component*>& GetComponents() { return myComponents; }

	GameObject* Duplicate();
	std::string GetObjectData();
	void LoadObjectData(std::string anObject);
	void LoadObjectData(rapidjson::Value& anObject);
	void LoadComponents(rapidjson::Value& anObject);
	void RenderInProperties(std::vector<Asset*>& anAssetList) override;
	inline bool MarkedForDestroy() { return myShouldDestroy; }

	void SaveAsset(const char* aPath) override;
	void LoadFromPath(const char* aPath) override;
	bool myIsOpenedInHierarchy = false;

	void SetSkeleton(std::shared_ptr<SkeletonInstance> aSkeleton);
	Transform* GetBone(const std::string& aBoneName);

	inline std::shared_ptr<Prefab> GetPrefab() { return myPrefab; }
	inline bool GetPrefabMissing() { return myPrefabMissing; }
	void SetPrefab(std::shared_ptr<Prefab> aPrefab);

	void SetRenderOutline(bool aFlag) { myShouldRenderOutline = aFlag; }
	bool GetRenderOutline() { return myShouldRenderOutline;	}

	void SetOutlineColor(Color aFlag) { myOutlineColor = aFlag; }
	const Color& GetOutlineColor() { return myOutlineColor; }

protected:
	friend class Scene;
	friend class GameObjectFactory;
	friend class Transform;
	void SetComponentsEnabled(bool aIsEnabled);
	std::string SaveObjectData();
	void Update();
	void Start();
	Transform myTransform;
	void DeleteObject();

	std::shared_ptr<Prefab> myPrefab;
	bool myPrefabMissing = false;
	std::string myPrefabName = "";
	std::shared_ptr<SkeletonInstance> mySkeleton;
	std::vector<Transform*> myChildrenToBones;

	int myInstanceID = -1;
	//int myIndex = -1;
	std::string myTag;
	std::string myNamePlusId; //prevents string from going out of scope while serializing
	bool myIsActive = true;
	std::vector<Component*> myComponents;
	bool myShouldDestroy = false;
	bool myHasStarted = false;
	std::string mySearchComponent = "";
	float myAliveTime;
	bool myHasFinishedLoading = false;
	bool myShouldCull = true;

	bool myShouldRenderOutline = false; //Lasse hijack
	Color myOutlineColor = { 1,1,1,1 };

};


template<class T, typename... Args>
inline T* GameObject::AddComponent(Args&&... args)
{
	auto newComponent = static_cast<Component*>(new T(std::forward<Args>(args)...));
	myComponents.push_back(newComponent);
	newComponent->SetGameObject(this);
	if (myInstanceID != -1) 
	{
		newComponent->Init();
	}
	if (Engine::GetInstance()->IsGameRunning() && myInstanceID != -1 && myHasFinishedLoading)
	{
		newComponent->Awake();
		newComponent->myHasStarted = true;
	}
	
	return static_cast<T*>(newComponent);
}

template<class T>
inline T* GameObject::GetComponent()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		T* comp = dynamic_cast<T*>(myComponents[i]);
		if (comp != nullptr)
		{
			return comp;
		}
	}
	return nullptr;
}

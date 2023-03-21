#pragma once

class GameObject;
class Collider;
class Transform;
class Component
{
public:
	Component() = default;
	virtual ~Component() {}
	virtual void FixedUpdate(){};
	virtual void Init() {};
	virtual void Awake(){};
	virtual void OnTransformChanged(){};
	virtual void RenderInProperties(std::vector<Component*>& aComponentList);
	virtual void RegisterComponentNodes(){};
	virtual void OnEnable() {};
	virtual void OnDisable() {};
	virtual void OnDestroy() {};
	void Destroy();
	Transform* GetTransform();
	GameObject& GetGameObject();
	void SetEnabled(bool anIsEnabled);
	bool IsEnabled() { return myIsEnabled; }
	const std::string& GetName();
	std::string GetComponentData();

protected:
	friend class ComponentHandler;
	friend class GameObject;

	virtual void Update() {};
	virtual void RunInEditor(){};
	virtual void Save(rapidjson::Value& aComponentData);
	virtual void Load(rapidjson::Value& aComponentData);
	virtual void OnObjectFinishedLoading() {};

	virtual void OnCollisionEnter(Collider* aCollider) { aCollider; };
	virtual void OnCollisionStay(Collider* aCollider) { aCollider; };
	virtual void OnCollisionExit(Collider* aCollider) { aCollider; };
	virtual void OnTriggerEnter(Collider* aCollider) { aCollider; };
	virtual void OnOverlapBegin(Collider* aCollider) { aCollider; };
	virtual void OnOverlap(Collider* aCollider) { aCollider; };
	virtual void OnOverlapEnd(Collider* aCollider) { aCollider; };

	GameObject* myGameObject;
	Transform* myTransform;
	bool myIsEnabled = true;
	bool myHasStarted = false;
	bool myShouldCull = false;
	bool myShouldUpdateWhilePaused = false;
	static unsigned int globalComponentCount;
	unsigned int myComponentId;

private:
	void SetGameObject(GameObject* aGameObject);
};

template<typename T>
std::vector<T*> ComponentVectorCast(std::vector<Component*>& aComponentList)
{
	std::vector<T*> castedComponentList;
	for (Component* component : aComponentList)
	{
		T* castedComponent = dynamic_cast<T*>(component);
		castedComponentList.push_back(castedComponent);
	}
	return castedComponentList;
}
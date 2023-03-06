#pragma once
#include "Physics/Collisions/Collider.h"
#include "Physics/PhysicsEngine.h"
class MeshCollider : public Collider
{
public:
	~MeshCollider();
	void Awake() override;
	void DebugDraw() override {};
	bool IsInside(const Vector3f& aPos) const override { return false; };

private:
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Init();
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	physx::PxRigidStatic* myStatic = NULL;
	bool myDebugMode = false;
	bool myIsStatic = false;
	std::shared_ptr<Model> myModel = nullptr;
	std::string myModelName = "Undefined";
};


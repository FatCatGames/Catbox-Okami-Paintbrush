#pragma once
#include "Collider.h"
#include "PxRigidStatic.h"

class ConvexCollider : public Collider
{
public:
	ConvexCollider();
	~ConvexCollider();

	void Awake();
	bool IsInside(const Vector3f& aPoint) const override;
	void SetOffset(const Vector3f& anOffset);
	const Vector3f GetPosition() const;
	void DebugDraw() override;
	void Update() override;
	void Init() override;
private:
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void RunInEditor() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	Vector3f myOffset;
	std::shared_ptr <Model> myModel = nullptr;
	physx::PxRigidStatic* myStatic = nullptr;
};
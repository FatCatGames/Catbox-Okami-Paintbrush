#pragma once
#include "Collider.h"
#include "PxRigidStatic.h"

class SphereCollider : public Collider
{
public:
	SphereCollider();
	~SphereCollider();

	void Awake();
	bool IsInside(const Vector3f& aPoint) const override;
	void SetOffset(const Vector3f& anOffset);
	void SetRadius(const float& aRadius);
	float GetRadius() const;
	const Vector3f GetPosition() const;
	void DebugDraw() override;
	void Update() override;

private:
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void RunInEditor() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	float myRadius;
	Vector3f myOffset;
	physx::PxRigidStatic* myStatic = nullptr;
};
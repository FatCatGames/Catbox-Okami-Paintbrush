#pragma once
#include "Collider.h"
#include <physX/PxRigidStatic.h>

class BoxCollider : public Collider
{
public:
	BoxCollider();
	~BoxCollider();
	void Init();
	void Awake();
	void SetSize(const Vector3f& aSize);
	void SetOffset(const Vector3f& anOffset);
	bool IsInside(const Vector3f& aPoint) const override;

	const Vector3f GetMinCorner() const;
	const Vector3f GetMaxCorner() const;
	const Vector3f GetSize() const { return mySize; }
	
	void RunInEditor() override;
	void Update() override;
	void DebugDraw() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;

	void OnCollisionEnter(Collider* aCollider) override;
	void OnCollisionExit(Collider* aCollider) override;
	void OnTriggerEnter(Collider* aCollider) override;


private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	Vector3f mySize;
	Vector3f myOffset;
	physx::PxRigidStatic* myStatic = nullptr;
};
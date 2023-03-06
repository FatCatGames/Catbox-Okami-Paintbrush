#pragma once
#include <stdafx.h>
#include "Debugging\DebugDrawer.h"

class RigidBody;
class Collider : public Component
{
public:
	enum class ColliderType
	{
		undefined,
		Box,
		Sphere,
		Circle2D,
		Box2D
	};

	Collider() = default;
	void Awake() override;
	~Collider();
	void RegisterCollision(Collider* anotherCollider);
	void UnregisterCollision(Collider* anotherCollider);
	inline std::vector<Collider*>& GetCollisions() { return myCurrentCollisions; }
	virtual void DebugDraw() = 0;
	inline const ColliderType& GetColliderType() const { return myColliderType; }
	inline const int GetCollisionLayer() const { return myLayer; }
	const std::string GetCollisionLayerName() const;
	inline void SetIsTrigger(bool aValue) { myIsTrigger = aValue; }
	inline bool GetIsTrigger() { return myIsTrigger; }
	void OnDisable() override;
	void SetLayer(int aLayer);
	void SetLayer(const std::string& aLayer);
	virtual bool IsInside(const Vector3f& aPoint) const = 0;
	bool GetDebugMode() const { return myDebugMode; }

protected:
	bool myHasLoaded = false;
	int myLayer = 0;
	RigidBody* myRb;
	std::vector<Collider*> myCurrentCollisions;
	ColliderType myColliderType;
	bool myIsTrigger = false;
	bool hasRunAwake = false;
	bool myDebugMode = false;
};
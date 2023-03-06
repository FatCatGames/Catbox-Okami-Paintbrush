#pragma once
#include "Physics/Ray.h"

class SphereCollider;
class BoxCollider;
class Collider;

class CollisionManager 
{
public:
	CollisionManager();
	~CollisionManager() = default;
	void UpdateCollisions();
	Collider* RayIntersect(Ray& aRay, const float& aMaxLength, std::vector<UINT> aLayermask, Vector3f& anOutIntersectionPoint, bool aIgnoreStartCollider = true);
	Collider* OverlapSphere(const Vector3f& anOrigin, const float aRadius, std::vector<UINT> aLayermask, Vector3f& anOutIntersectionPoint);
	void AddCollider(Collider* aCollider);
	void RemoveCollider(Collider* aCollider);
	void AddLayer(const std::string& aLayerName);
	void DrawCollisionMatrix();
	inline const std::vector<std::string>& GetCollisionLayers() const { return myCollisionLayerNames; } 
	inline std::vector<std::vector<bool>>& GetCollisionLayerMatrix() { return myCollisionMatrix; } 
	inline std::vector<std::vector<bool>>& GetPhysicsCollisionLayers() { return myPhysicsCollisionLayers; }
	int GetLayerByName(const std::string& aName);
	void ClearColliders();

private:
	friend class PhysicsWindow;
	std::vector<std::vector<Collider*>> myColliders;
	std::vector<std::vector<bool>> myCollisionMatrix;
	std::vector<std::string> myCollisionLayerNames;
	std::vector<std::vector<bool>> myPhysicsCollisionLayers;
	int myCollisionCheckRange = 50;
	Ray myRay;
};
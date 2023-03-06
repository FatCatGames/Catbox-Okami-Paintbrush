#pragma once
#include "PhysicsEngine.h"

struct HitReport 
{
	bool Hit = false;
	GameObject* GameObjectHit = nullptr;
	Vector3f PositionHit = { 0,0,0 };
	Vector3f NormalHit = { 0,0,0 };
};

class PhysXUtilities
{
public:
	//Send in a Bitshifted Int with the Layer IDs you want to check, to check layer 2, 3 & 5 you'd pass this in as an int: 
	//1 << 2 | 1 << 3 | 1 << 5
	static HitReport RayCast(const Vector3f& aOrigin, const Vector3f& aDirection, float aMaxDistance, float aAliveTime = 0, const Color& aColor = {1,1,1,1}, int layersToCheck = 0);
	
	static bool SweepSingle(const Vector3f& aOrigin, const float aRadius, const Vector3f& aDirection, float aMaxDistance, int layersToCheck = 0);
	static std::vector<GameObject*> SweepMultiple(const Vector3f& aOrigin, const float aRadius, const Vector3f& aDirection, float aMaxDistance, int layersToCheck = 0);
	
	static bool OverlapAny(const Vector3f& aOrigin, const float aRadius, int layersToCheck = 0);
	static std::vector<GameObject*> OverlapMultiple(const Vector3f& aOrigin, const float aRadius, int layersToCheck = 0);
private:

};
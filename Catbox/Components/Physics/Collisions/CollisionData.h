#pragma once
#include "Collider.h"

struct CollisionData
{
	Collider* otherCollider{ nullptr };
	Vector3f intersectionPoint { 0,0,0 };
	Vector3f overlap;
};
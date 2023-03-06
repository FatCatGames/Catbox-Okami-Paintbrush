#include "stdafx.h"
#include "CollisionManager.h"
#include "../CommonUtilities/UtilityFunctions.hpp"

#include "SphereCollider.h"
#include "BoxCollider.h"
#include "BoxCollider2D.h"
#include "CircleCollider2D.h"
#include "Collider.h"
#include "../Physics/Intersection.h"
#include <cassert>
#include "../Components/Physics/RigidBody.h"
#include "../ComponentTools/SceneManager.h"

CollisionManager::CollisionManager()
{
	AddLayer("Default");
}

void CollisionManager::ClearColliders()
{
	for (auto& layer : myColliders)
	{
		layer.clear();
	}
}

void CollisionManager::UpdateCollisions()
{
	Vector3f out; //not used

	for (size_t lay1 = 0; lay1 < myCollisionMatrix.size(); lay1++)
	{
		for (size_t lay2 = 0; lay2 < myCollisionMatrix[lay1].size(); lay2++)
		{
			if (!myCollisionMatrix[lay1][lay2]) continue; //these layers do not collide
			for (size_t i = 0; i < myColliders[lay1].size(); i++)
			{

				Collider* col = myColliders[lay1][i];
				if (!col->GetGameObject().IsActive() || !col->IsEnabled()) continue;

				//test this collider against all other colliders
				for (size_t j = 0; j < myColliders[lay2 + lay1].size(); j++)
				{
					if (lay2 == 0 && j < i)
					{
						j = i;
						continue;
					}
					Collider* col2 = myColliders[lay1 + lay2][j];
					if (!col2->GetGameObject().IsActive() || !col2->IsEnabled()) continue;
					if (col == col2) continue;


					if (Intersection::IsCollidedWith(*col, *col2, out))
					{
						col->RegisterCollision(col2);
						col2->RegisterCollision(col);
						//HandleCollision(col, col2);
						//HandleCollision(col2, col);
					}
					else
					{
						col->UnregisterCollision(col2);
						col2->UnregisterCollision(col);
					}
				}
			}
		}
	}
}


Collider* CollisionManager::RayIntersect(Ray& aRay, const float& aMaxLength, std::vector<UINT> aLayermask, Vector3f& anOutIntersectionPoint, bool aIgnoreStartCollider)
{
	const Vector3f& origin = aRay.GetOrigin();
	float closestDist = aMaxLength * aMaxLength;
	Collider* closestCol = nullptr;

	for (int n = 0; n < aLayermask.size(); n++)
	{
		for (size_t i = 0; i < myColliders[aLayermask[n]].size(); i++)
		{
			Collider* col = myColliders[aLayermask[n]][i];
			if (!col->GetGameObject().IsActive()) continue;
			if (!col->IsEnabled()) continue;
			Vector3f out;

			if (Intersection::RayIntersect(*col, aRay, aMaxLength, out))
			{
				float dist = (out - origin).LengthSqr();
				if (dist <= closestDist)
				{
					if (aIgnoreStartCollider && col->IsInside(origin)) continue;
					anOutIntersectionPoint = out;
					closestDist = dist;
					closestCol = col;
				}
			}
		}
	}

	return closestCol;
}

Collider* CollisionManager::OverlapSphere(const Vector3f& anOrigin, const float aRadius, std::vector<UINT> aLayermask, Vector3f& anOutIntersectionPoint)
{
	Catbox::Sphere sphere(anOrigin, aRadius);

	float closestDist = INT_MAX;
	Collider* closestCol = nullptr;

	for (int n = 0; n < aLayermask.size(); n++)
	{
		for (size_t i = 0; i < myColliders[aLayermask[n]].size(); i++)
		{
			Collider* col = myColliders[aLayermask[n]][i];
			if (!col->GetGameObject().IsActive()) continue;
			if (!col->IsEnabled()) continue;
			Vector3f out;

			if (Intersection::SphereIntersect(*col, sphere, out))
			{
				float dist = (out - sphere.position).LengthSqr();
				if (dist <= closestDist)
				{
					anOutIntersectionPoint = out;
					closestDist = dist;
					closestCol = col;
				}
			}
		}
	}

	return closestCol;
}


void CollisionManager::AddCollider(Collider* aCollider)
{
	myColliders[aCollider->GetCollisionLayer()].push_back(aCollider);
}

void CollisionManager::RemoveCollider(Collider* aCollider)
{
	if (myColliders.size() > aCollider->GetCollisionLayer()) 
	{
		auto& colliders = myColliders[aCollider->GetCollisionLayer()];
		for (size_t i = 0; i < colliders.size(); i++)
		{
			if (colliders[i] == aCollider)
			{
				colliders.erase(colliders.begin() + i);
				return;
			}
		}
	}
}


void CollisionManager::AddLayer(const std::string& aLayerName)
{
	myCollisionLayerNames.push_back(aLayerName);
	myCollisionMatrix.emplace_back();
	myColliders.emplace_back();
	myPhysicsCollisionLayers.emplace_back();

	for (size_t i = 0; i < myCollisionMatrix.size(); i++)
	{
		myCollisionMatrix[i].push_back(true);
	}

	for (size_t i = 0; i < myPhysicsCollisionLayers.size(); i++)
	{
		while (myPhysicsCollisionLayers[i].size() < myPhysicsCollisionLayers.size())
		{
			myPhysicsCollisionLayers[i].push_back(false);
		}
	}
}


void CollisionManager::DrawCollisionMatrix()
{
	ImGui::Text("Collision matrix");
}

int CollisionManager::GetLayerByName(const std::string& aName)
{
	for (int i = 0; i < myCollisionLayerNames.size(); i++)
	{
		if (myCollisionLayerNames[i] == aName)
		{
			return i;
		}
	}

	const char* err = ("Tried to get collision layer " + aName + " but it did not exist").c_str();
	assert(true && err);
	return -1;
}

#include "stdafx.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "CollisionData.h"
#include "../Components/Physics/RigidBody.h"
#include "../Physics/Intersection.h"

void Collider::Awake()
{
	Engine::GetInstance()->GetCollisionManager()->AddCollider(this);
	myRb = myGameObject->GetComponent<RigidBody>();
	myShouldCull = true;
}

Collider::~Collider()
{
	Engine::GetInstance()->GetCollisionManager()->RemoveCollider(this);
	for (auto& col : myCurrentCollisions)
	{
		col->UnregisterCollision(this);
	}
}

void Collider::RegisterCollision(Collider* anotherCollider)
{
	for (size_t i = 0; i < myCurrentCollisions.size(); i++)
	{
		if (myCurrentCollisions[i] == anotherCollider)
		{
			if (myCurrentCollisions[i]->myIsTrigger || myIsTrigger)
			{
				myGameObject->OnOverlap(anotherCollider);
			}
			myGameObject->OnCollisionStay(anotherCollider);
			return;
		}
	}

	if (anotherCollider->myIsTrigger || myIsTrigger)
	{
		myGameObject->OnOverlapBegin(anotherCollider);
	}
	else myGameObject->OnCollisionEnter(anotherCollider);
	myCurrentCollisions.push_back(anotherCollider);
}

void Collider::UnregisterCollision(Collider* anotherCollider)
{
	for (size_t i = 0; i < myCurrentCollisions.size(); i++)
	{
		if (myCurrentCollisions[i] == anotherCollider)
		{
			if (myCurrentCollisions[i]->myIsTrigger || myIsTrigger)
			{
				myGameObject->OnOverlapEnd(anotherCollider);
			}
			else myGameObject->OnCollisionExit(anotherCollider);
			myCurrentCollisions.erase(myCurrentCollisions.begin() + i);
			return;
		}
	}
}

const std::string Collider::GetCollisionLayerName() const
{
	return Engine::GetInstance()->GetCollisionManager()->GetCollisionLayers()[myLayer];
}

void Collider::OnDisable()
{
	for (size_t i = 0; i < myCurrentCollisions.size(); i++)
	{
		myCurrentCollisions[i]->UnregisterCollision(this);
	}
	myCurrentCollisions.clear();
}

void Collider::SetLayer(int aLayer)
{
	if (aLayer == myLayer) return;

	for (size_t i = 0; i < myCurrentCollisions.size(); i++)
	{
		myCurrentCollisions[i]->UnregisterCollision(this);
	}
	myCurrentCollisions.clear();

	Engine::GetInstance()->GetCollisionManager()->RemoveCollider(this);
	myLayer = aLayer;
	Engine::GetInstance()->GetCollisionManager()->AddCollider(this);
}

void Collider::SetLayer(const std::string& aLayer)
{
	myLayer = Engine::GetInstance()->GetCollisionManager()->GetLayerByName(aLayer);
}


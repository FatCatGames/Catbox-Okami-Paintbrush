#include "Game.pch.h"
#include "BreakableRock.h"
#include "Components\Physics\Collisions\Collider.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Model.h"

void BreakableRock::OnTriggerEnter(Collider* aCollider)
{
	if (aCollider->GetCollisionLayer() == static_cast<int>(CollisionLayer::Bomb))
	{
		myGameObject->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>("brokenrock"));
	}
}

void BreakableRock::OnOverlapBegin(Collider* aCollider)
{
	if (aCollider->GetCollisionLayer() == static_cast<int>(CollisionLayer::Bomb))
	{
		myGameObject->GetComponent<ModelInstance>()->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>("brokenrock"));
	}
}

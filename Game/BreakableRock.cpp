#include "Game.pch.h"
#include "BreakableRock.h"
#include "Components\Physics\Collisions\Collider.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Model.h"

void BreakableRock::OnTriggerEnter(Collider* aCollider)
{
	if (aCollider->GetCollisionLayer() == static_cast<int>(CollisionLayer::Bomb))
	{
		myModel = myGameObject->GetComponent<ModelInstance>();
		myModel->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>("brokenrock"));
		myIsLerping = true;
	}
}

void BreakableRock::OnOverlapBegin(Collider* aCollider)
{
	if (aCollider->GetCollisionLayer() == static_cast<int>(CollisionLayer::Bomb))
	{
		myModel = myGameObject->GetComponent<ModelInstance>();
		myModel->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>("brokenrock"));
		myIsLerping = true;
	}
}


void BreakableRock::Update()
{
	if (myIsLerping)
	{
		if (myDeadTime > 1)
		{
			myLerpTimer += deltaTime;
			if (myLerpTimer < myLerpTarget)
			{
				float percent = myLerpTimer / myLerpTarget;
				float t = percent * percent;
				myModel->GetMaterial(0)->SetColor(Color(1, 1, 1, 1 - t));
			}
			else
			{
				myGameObject->Destroy();
			}
		}
		else myDeadTime += deltaTime;
	}
}
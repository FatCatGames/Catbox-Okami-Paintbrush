#include "Game.pch.h"
#include "Tree.h"
#include "Components\Physics\RigidBody.h"

void Tree::Slash(const Vector3f& anIntersectionPos, const Vector3f& anIntersectionDir)
{
	for (auto& child : myTransform->GetChildren())
	{
		if (child->GetGameObject()->GetName() == "Top")
		{
			auto rb = child->GetGameObject()->GetComponent<RigidBody>();
			rb->SetRotationAxisLock(false, false, false);
			rb->SetTranslationAxisLock(false, false, false);
			rb->AddForceAtPos(1, anIntersectionPos, physx::PxForceMode::eIMPULSE);
			rb->SetUseGravity(true);
		}
	}
}

#include "Game.pch.h"
#include "Tree.h"
#include "Components\Physics\RigidBody.h"
#include "Physics\PhysicsEngine.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Material.h"
#include "Components\ParticleSystem.h"

void Tree::Slash(const Vector3f& anIntersectionPos, const Vector3f& anIntersectionDir)
{
	myIsLerping = true;
	mySlashDir = anIntersectionDir;

	for (auto& child : myTransform->GetChildren())
	{
		if (child->GetGameObject()->GetName() == "Top")
		{
			myTreeTop = child;

			myTargetAngleZ = -Catbox::Rad2Deg(atan2(anIntersectionDir.x, anIntersectionDir.z));
			if (myTargetAngleZ < -90) myTargetAngleZ = -180 - myTargetAngleZ;
			else if (myTargetAngleZ > 90) myTargetAngleZ = 180 - myTargetAngleZ;

			myTargetAngleX = Catbox::Rad2Deg(atan2(anIntersectionDir.z, anIntersectionDir.x));
			if (myTargetAngleX < -90) myTargetAngleX = -180 - myTargetAngleX;
			else if (myTargetAngleX > 90) myTargetAngleX = 180 - myTargetAngleX;

			myTreeTopMat = child->GetGameObject()->GetComponent<ModelInstance>()->GetMaterial(0).get();
		}
		else if(child->GetGameObject()->GetName() == "Trunk")
		{
			myTreeTrunkMat = child->GetGameObject()->GetComponent<ModelInstance>()->GetMaterial(0).get();
		}
		else if (child->GetGameObject()->GetName() == "Sakura")
		{
			child->GetGameObject()->GetComponent<ParticleSystem>()->Pause();
		}
	}
}

void Tree::Update()
{
	if (myIsLerping)
	{
		myLerpTimer += deltaTime;
		if (myLerpTimer < myLerpTarget)
		{
			float percent = myLerpTimer / myLerpTarget;
			float t = percent * percent;
			myTreeTop->SetWorldRot(Catbox::Lerp(myStartRot.x, myTargetAngleX, t), myStartRot.y, Catbox::Lerp(myStartRot.z, myTargetAngleZ, t));
			myTreeTopMat->SetColor(Color(1, 1, 1, 1 - t));
		}
		else if (myLerpTimer - myLerpTarget > 1)
		{
			myDeadTime += deltaTime;
			myTreeTrunkMat->SetColor(Color(1, 1, 1, 1 - myDeadTime));

			if (myDeadTime > 1)
			{
				myGameObject->Destroy();
			}
		}
	}
}

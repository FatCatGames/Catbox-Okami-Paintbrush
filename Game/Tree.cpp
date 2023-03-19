#include "Game.pch.h"
#include "Tree.h"
#include "Components\Physics\RigidBody.h"
#include "Physics\PhysicsEngine.h"

void Tree::Slash(const Vector3f& anIntersectionPos, const Vector3f& anIntersectionDir)
{
	myIsLerping = true;
	mySlashDir = anIntersectionDir;

	for (auto& child : myTransform->GetChildren())
	{
		if (child->GetGameObject()->GetName() == "Top")
		{
			myTreeTop = child;
			/*auto rb = child->GetGameObject()->GetComponent<RigidBody>();
			physx::PxTransform tempTransfrom;
			tempTransfrom.p.x = 0;
			tempTransfrom.p.y = 3;
			tempTransfrom.p.z = 0;
			rb->GetActor()->setCMassLocalPose(tempTransfrom);
			rb->SetRotationAxisLock(false, false, false);
			rb->SetTranslationAxisLock(false, false, false);
			rb->AddForceAtPos(5, anIntersectionPos, physx::PxForceMode::eIMPULSE);
			rb->SetUseGravity(true);*/
			myStartRot = child->worldRot();

			myTargetAngleX = Catbox::Rad2Deg(atan2(1, sqrt(pow(mySlashDir.x, 2) + pow(mySlashDir.z, 2))));
			myTargetAngleZ = Catbox::Rad2Deg(atan2(mySlashDir.x, mySlashDir.z));
			printmsg("Angle: " + std::to_string(myTargetAngleZ));
			return;
		}
	}
}

void Tree::Update()
{
	if (myIsLerping && myLerpTimer < myLerpTarget)
	{
		myLerpTimer += deltaTime;
		float t = myLerpTimer / myLerpTarget;
		myTreeTop->SetWorldRot(Catbox::Lerp(myStartRot.x, myTargetAngleX, t), myStartRot.y, Catbox::Lerp(myStartRot.z, myTargetAngleZ, t));
	}
}

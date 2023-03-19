#include "Game.pch.h"
#include "Bomb.h"
#include "Components\Physics\RigidBody.h"
#include "Components\CameraShake.h"
#include "Components\Physics\Collisions\Collider.h"

void Bomb::Awake()
{
	myRb = myGameObject->GetComponent<RigidBody>();
}

void Bomb::OnTransformChanged()
{
	if (!myHasSavedPos || !PLAYMODE)
	{
		mySpawnPos = myTransform->worldPos();
		myHasSavedPos = true;
	}
}

void Bomb::Update()
{
	if (!myHasLoadedPos && myHasSavedPos)
	{
		myRb->SetActorPosition(mySpawnPos);
		myHasLoadedPos = true;
	}

	myAliveTime += deltaTime;

	if (myPushForce.LengthSqr() > 0.1f)
	{
		myRb->AddForce(myPushForce, 5);
		myPushForce = Vector3f::zero();
		mySpeed = 5;
	}
	if (mySpeed > 0)
	{
		mySpeed -= deltaTime;
	}
	
	if (myAliveTime > 3)
	{
		Engine::GetInstance()->GetActiveCamera()->GetGameObject().GetComponent<CameraShake>()->Start();
		auto explosion = InstantiatePrefab("BombExplosion");
		explosion->GetTransform()->SetWorldPos(myTransform->worldPos());
		myGameObject->Destroy();
	}
}

void Bomb::FixedUpdate()
{
	//myRb->GetActor()->setAngularVelocity(myRb->GetActor()->getAngularVelocity().getNormalized(), 0);
}

void Bomb::OnCollisionStay(Collider* aCollider)
{
	if (aCollider->GetCollisionLayer() == 1)
	{
		myPushForce = (myTransform->worldPos() - aCollider->GetTransform()->worldPos()).GetNormalized();
	}
}

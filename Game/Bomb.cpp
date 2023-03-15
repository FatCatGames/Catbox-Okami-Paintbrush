#include "Game.pch.h"
#include "Bomb.h"
#include "Components\Physics\RigidBody.h"

void Bomb::OnTransformChanged()
{
	if (!myHasSavedPos)
	{
		mySpawnPos = myTransform->worldPos();
		myHasSavedPos = true;
	}
}

void Bomb::Update()
{
	if (!myHasLoadedPos)
	{
		myGameObject->GetComponent<RigidBody>()->SetActorPosition(mySpawnPos);
		myHasLoadedPos = true;
	}

	myAliveTime += deltaTime;
	if (myAliveTime > 2)
	{
		auto explosion = InstantiatePrefab("BombExplosion");
		explosion->GetTransform()->SetWorldPos(myTransform->worldPos());
		myGameObject->Destroy();
	}
}

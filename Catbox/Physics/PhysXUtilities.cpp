#include "stdafx.h"
#include "PhysXUtilities.h"
#include "GameObjects/GameObjectFactory.h"
//#include "PhysicsEngine.h"

HitReport PhysXUtilities::RayCast(const Vector3f& aOrigin, const Vector3f& aDirection, float aMaxDistance, float aAliveTime, const Color& aColor, int layersToCheck)
{
	physx::PxRaycastBuffer aBuffer;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;

	if (aAliveTime > 0) 
	{
		DebugDrawer::DrawLine(aOrigin, aOrigin + aDirection * aMaxDistance, aColor, aAliveTime);
	}

	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->raycast(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z), physx::PxVec3(aDirection.x, aDirection.y, aDirection.z).getNormalized(), physx::PxReal(aMaxDistance), aBuffer, physx::PxHitFlag::eDEFAULT, aFilterData);

	HitReport returnHitReport;
	returnHitReport.Hit = aBuffer.hasAnyHits();
	if(returnHitReport.Hit)
	{
		returnHitReport.GameObjectHit = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.getAnyHit(0).actor->userData));
		returnHitReport.NormalHit = Vector3f(aBuffer.getAnyHit(0).normal.x, aBuffer.getAnyHit(0).normal.y, aBuffer.getAnyHit(0).normal.z);
		returnHitReport.PositionHit = Vector3f(aBuffer.getAnyHit(0).position.x, aBuffer.getAnyHit(0).position.y, aBuffer.getAnyHit(0).position.z);
	}
	return returnHitReport;
}

bool PhysXUtilities::SweepSingle(const Vector3f& aOrigin, const float aRadius, const Vector3f& aDirection, float aMaxDistance, int layersToCheck)
{
	physx::PxSweepHit aHitBuffer[256];
	physx::PxSweepBuffer aBuffer(aHitBuffer, 256);
	physx::PxSphereGeometry aSweepShape = physx::PxSphereGeometry(aRadius);
	physx::PxTransform aTransform(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z));
	std::vector<GameObject*> objectsHit;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;

	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->sweep(aSweepShape, aTransform, physx::PxVec3(aDirection.x, aDirection.y, aDirection.z), aMaxDistance, aBuffer, physx::PxHitFlag::eDEFAULT, aFilterData);

	if (aBuffer.nbTouches > 0)
	{
		return true;
	}

	return false;
}

std::vector<GameObject*> PhysXUtilities::SweepMultiple(const Vector3f& aOrigin, const float aRadius, const Vector3f& aDirection, float aMaxDistance, int layersToCheck)
{
	physx::PxSweepHit aHitBuffer[256];
	physx::PxSweepBuffer aBuffer(aHitBuffer, 256);
	physx::PxSphereGeometry aSweepShape = physx::PxSphereGeometry(aRadius);
	physx::PxTransform aTransform(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z));
	std::vector<GameObject*> objectsHit;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;

	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->sweep(aSweepShape, aTransform, physx::PxVec3(aDirection.x, aDirection.y, aDirection.z), aMaxDistance, aBuffer, physx::PxHitFlag::eDEFAULT, aFilterData);

	for (physx::PxU32 i = 0; i < aBuffer.nbTouches; i++)
	{
		objectsHit.push_back(Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.touches[i].actor->userData)));
	}

	return std::vector<GameObject*>();
}

bool PhysXUtilities::OverlapAny(const Vector3f& aOrigin, const float aRadius, int layersToCheck)
{
	physx::PxOverlapHit aHitBuffer[256];
	physx::PxOverlapBuffer aBuffer(aHitBuffer, 256);
	physx::PxSphereGeometry anOverlapShape = physx::PxSphereGeometry(aRadius);
	physx::PxTransform aTransform(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z));
	std::vector<GameObject*> objectsHit;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;

	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->overlap(anOverlapShape, aTransform, aBuffer, aFilterData);

	if (aBuffer.nbTouches > 0)
	{
		return true;
	}

	return false;
}

std::vector<GameObject*> PhysXUtilities::OverlapMultiple(const Vector3f& aOrigin, const float aRadius, int layersToCheck)
{
	physx::PxOverlapHit aHitBuffer[256];
	physx::PxOverlapBuffer aBuffer(aHitBuffer, 256);
	physx::PxSphereGeometry anOverlapShape = physx::PxSphereGeometry(aRadius);
	physx::PxTransform aTransform(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z));
	std::vector<GameObject*> objectsHit;
	physx::PxQueryFilterData aFilterData = physx::PxQueryFilterData();
	aFilterData.data.word0 = layersToCheck;
	
	Engine::GetInstance()->GetPhysicsEngine()->GetScene()->overlap(anOverlapShape, aTransform, aBuffer, aFilterData);

	for (physx::PxU32 i = 0; i < aBuffer.nbTouches; i++)
	{
		//Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.touches[i].actor->userData))->SetActive(false);
		//Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.touches[i].actor->userData))->Destroy(); //Teehee
		objectsHit.push_back(Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)(aBuffer.touches[i].actor->userData)));
	}

	return objectsHit;
}

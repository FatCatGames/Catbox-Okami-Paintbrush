#pragma once
#include <physX\PxPhysicsAPI.h>
enum Shape
{
	PxS_Box,
	PxS_Circle,
	PxS_Capsule,
	PxS_Convex,
	PxS_Mesh,
	PxS_Count
};

enum ParticleType
{
	PxPS_Fluid,
	PxPS_Cloth
};

struct FilterGroup 
{
	enum Filters
	{
		PxF_Default = (1 << 0),
		PxF_Enemy = (1 << 1),
		PxF_WhateverIDC = (1 << 2)
	};
};

class DefaultSimulationCallback : public physx::PxSimulationEventCallback
{
public:
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;

	void onWake(physx::PxActor** actors, physx::PxU32 count) override;

	void onSleep(physx::PxActor** actors, physx::PxU32 count) override;

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;

	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

	void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;
}; 

class DefaultCharacterControllerCallback : public physx::PxUserControllerHitReport
{
public:
	void onShapeHit(const physx::PxControllerShapeHit& hit);
	void onControllerHit(const physx::PxControllersHit& hit);
	void onObstacleHit(const physx::PxControllerObstacleHit& hit);
};


class PhysicsEngine
{
public:
	friend class DefaultSimulationCallback;
	friend class DefaultCharacterControllerCallback;
	void Init();
	void Update();
	void DrawLines();
	void AddActor(physx::PxRigidActor* aActor, const int& aId, const int aLayerID = 0);
	void AddParticleSystem(physx::PxParticleSystem* aParticleSystem, const int& aId);
	void ResetScene();
	physx::PxScene* GetScene() { return myPxScene; }

	physx::PxRigidActor* CreateDynamicActor(Shape aShape = Shape::PxS_Box, Vector3f aScale = { 1,1,1 }, Model* aMeshdata = nullptr, std::string aMaterialName = "Default", float aMass = 1, bool aDebugMode = false, Vector3f aTransformSize = { 1,1,1 }, bool aIsTrigger = false);
	physx::PxRigidStatic* CreateStaticActor(Shape aShape = Shape::PxS_Box, Vector3f aScale = { 1,1,1 }, Model* aMeshdata = nullptr, std::string aMaterialName = "Default", bool aDebugMode = false, Vector3f aTransformSize = { 1,1,1 }, Vector3f aLocalRotation = {0,0,0}, bool aIsTrigger = false);
	physx::PxPBDParticleSystem* CreateParticleSystem(ParticleType = ParticleType::PxPS_Fluid);
	physx::PxController* CreateCharacterController(Shape aShape = Shape::PxS_Box, Vector3f aSize = {1,1,1}, float aDensity = 1.0f, std::string aMaterialName = "Default", int aId = -1);
	physx::PxMaterial* CreateMaterial(std::string aName, Vector3f aMaterial); 
	physx::PxMaterial* GetMaterial(std::string aName);
	physx::PxMaterial* EditMaterial(std::string aName, Vector3f aMaterial);

protected:
	std::vector<std::function<void(Collider*)>> myCallbacks;
	std::vector<std::function<void()>> myCCTCallbacks;
	std::vector<Collider*> myCallbacksParameters;
private:
	void InitScene();

	physx::PxConvexMesh* CookConvexMesh(MeshData& aMeshData, Vector3f aSize = {1,1,1});
	physx::PxConvexMesh* CookConvexMesh(std::vector<Vertex>& aMeshData, Vector3f aSize = { 1,1,1 });
	physx::PxTriangleMesh* CookTriangleMesh(const MeshData& aMeshData, Vector3f aSize = { 1,1,1 });

	void SendCallbacks();

	bool myResetingScene = false;

	physx::PxScene* myPxScene = NULL;
	physx::PxDefaultAllocator myPxDefaultAllocatorCallback;
	physx::PxDefaultErrorCallback myPxDefaultErrorCallback;
	physx::PxFoundation* myPxfoundation = NULL;
	physx::PxCooking* myCooker = NULL;
	physx::PxSimulationEventCallback* mySimCallback = NULL;
	physx::PxUserControllerHitReport* myCCTCallback = NULL;
	physx::PxMaterial* myPxDefaultMaterial = NULL;
	physx::PxParticleMaterial* myDefaultParticleMaterial = NULL;
	physx::PxTolerancesScale myPxToleranceScale;
	physx::PxPvd* myPxPvd = NULL;
	physx::PxPhysics* myPxPhysics = NULL;
	physx::PxDefaultCpuDispatcher* myPxDispatcher = NULL;
	physx::PxControllerManager* myCharacterController = NULL;
	physx::PxCudaContextManager* myCudaManager = NULL;

	std::vector<physx::PxShape*> myShapes;
	std::vector<physx::PxActor*> myAddActorsInRuntime;


	float myDebugRenderTimer = 0;
	std::vector<physx::PxActor*> myActors;
	std::unordered_map<std::string, physx::PxMaterial*> myPhysxMaterialMap;
};

inline physx::PxFilterFlags DefaultPxFilterFlags(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags |= physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	if (physx::PxFilterObjectIsKinematic(attributes0) || physx::PxFilterObjectIsKinematic(attributes1))
	{
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			pairFlags |= physx::PxPairFlag::eCONTACT_DEFAULT;
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		}
		return physx::PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
	

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)) 
	{
		pairFlags |= physx::PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}
	return physx::PxFilterFlag::eDEFAULT;
}

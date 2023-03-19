#include "stdafx.h"
#include "PhysicsEngine.h"
#include "Assets/Model.h"
#include "physX/cudamanager/PxCudaContext.h"
#include "Components/Physics/Collisions/BoxCollider.h"
#include "Components/Physics/Collisions/ConvexCollider.h"
#include "Components/Physics/Collisions/SphereCollider.h"
#include "Components/Physics/Collisions/MeshCollider.h"
#include "Components/Physics/Collisions/CollisionManager.h"

void PhysicsEngine::Init()
{
	mySimCallback = new DefaultSimulationCallback();
	myCCTCallback = new DefaultCharacterControllerCallback();
	myCCFCallback = new DefaultCCFCallback();

	myPxfoundation = PxCreateFoundation(PX_PHYSICS_VERSION, myPxDefaultAllocatorCallback, myPxDefaultErrorCallback);
	assert(myPxfoundation != nullptr && "COULD NOT CREATE PHYSX FOUNDATION");
	myPxPvd = physx::PxCreatePvd(*myPxfoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	myPxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	myPxToleranceScale = physx::PxTolerancesScale();
	myPxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *myPxfoundation, myPxToleranceScale, true, myPxPvd);
	physx::PxCookingParams aParams = physx::PxCookingParams(myPxToleranceScale);
	myCooker = PxCreateCooking(PX_PHYSICS_VERSION, *myPxfoundation, aParams);

	//physx::PxCudaContextManagerDesc cudaContextManager;
	//myCudaManager = PxCreateCudaContextManager(*myPxfoundation, cudaContextManager, PxGetProfilerCallback());
	//myDefaultParticleMaterial = myPxPhysics->createPBDMaterial(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	myPxDefaultMaterial = myPxPhysics->createMaterial(1.0f, 1.0f, 0.0f);
	myPhysxMaterialMap.insert({ "Default", myPxDefaultMaterial });
	InitScene();

}

void PhysicsEngine::Update()
 {
	if (deltaTime > 0 && !myResetingScene)
	{
		if (myPxScene) 
		{
			float delta = 1.0f / 60.0f;
			if (deltaTime > (1.0f / 60.0f))
			{
				delta = deltaTime;
			}
			myPxScene->simulate(delta);
			if (myPxScene->checkResults(true))
			{
				SendCallbacks();
			}
			if (myPxScene->fetchResults(true))
			{
				//SendCallbacks();
				DrawLines();

			}
			myPxScene->fetchResultsParticleSystem();
		}
	}
}

void PhysicsEngine::DrawLines()
{
	if (myPxScene)	
	{
		const physx::PxRenderBuffer& aBuffer = myPxScene->getRenderBuffer();
		for (physx::PxU32 i = 0; i < aBuffer.getNbPoints(); i++)
		{
			const physx::PxDebugPoint& debugPoint = aBuffer.getPoints()[i];
			Vector3f aPoint = { debugPoint.pos.x, debugPoint.pos.y, debugPoint.pos.z };
			DebugDrawer::DrawSphere(aPoint, 0.5f);
		}
		for (physx::PxU32 i = 0; i < aBuffer.getNbLines(); i++)
		{
			const physx::PxDebugLine& debugLine = aBuffer.getLines()[i];
			Vector3f aVector0 = { debugLine.pos0.x,debugLine.pos0.y, debugLine.pos0.z };
			Vector3f aVector1 = { debugLine.pos1.x,debugLine.pos1.y, debugLine.pos1.z };
			DebugDrawer::DrawLine(aVector0, aVector1, Color(1, 0, 0, 1));
		}
		for (physx::PxU32 i = 0; i < aBuffer.getNbTriangles(); i++)
		{
			const physx::PxDebugTriangle& debugTriangle = aBuffer.getTriangles()[i];
			Vector3f aVector0 = { debugTriangle.pos0.x,debugTriangle.pos0.y, debugTriangle.pos0.z };
			Vector3f aVector1 = { debugTriangle.pos1.x,debugTriangle.pos1.y, debugTriangle.pos1.z };
			Vector3f aVector2 = { debugTriangle.pos2.x,debugTriangle.pos2.y, debugTriangle.pos2.z };
			DebugDrawer::DrawLine(aVector0, aVector1, Color(1, 0, 0, 1));
			DebugDrawer::DrawLine(aVector1, aVector2, Color(1, 0, 0, 1));
			DebugDrawer::DrawLine(aVector0, aVector2, Color(1, 0, 0, 1));
		}
	}
}

void PhysicsEngine::AddActor(physx::PxRigidActor* aActor, const int& aId, const int aLayerID)
{
	if (aActor && myPxScene)
	{
		physx::PxFilterData filterData;

		filterData.word0 = 1 << aLayerID;
		filterData.word1 = 0;

		CollisionManager* aCollisionManager = Engine::GetInstance()->GetCollisionManager().get(); //Let it dangle then, idc

		for (size_t i = 0; i < aCollisionManager->GetPhysicsCollisionLayers()[aLayerID].size(); i++)
		{
			if (aCollisionManager->GetPhysicsCollisionLayers()[aLayerID][i])
			{
				filterData.word1 |= 1 << i;
			}
		}

		const physx::PxU32 numShapes = aActor->getNbShapes();
		physx::PxShape** aShapeArray = (physx::PxShape**)malloc(sizeof(physx::PxShape*) * numShapes);
		aActor->getShapes(aShapeArray, numShapes);
		for (int i = 0; i < numShapes; i++)
		{
			physx::PxShape* shape = aShapeArray[i];
			shape->setSimulationFilterData(filterData);
			shape->setQueryFilterData(filterData);
			if (shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE)	//Bro imagine actually reading what stuff does before writing it??
			{
				shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
			}
			else
			{
				shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			}
		}
		delete aShapeArray;

		aActor->userData = (void*)aId;

		myPxScene->addActor(*aActor);
	}
	else 
	{
		printerror("Could not add Actor to PxScene, probaly Actor was nullptr");
	}
}

void PhysicsEngine::AddParticleSystem(physx::PxParticleSystem* aParticleSystem, const int& aId)
{
	aParticleSystem->userData = (void*)aId;
	myPxScene->addActor(*aParticleSystem);
	physx::PxParticleBuffer* particleBuffer = myPxPhysics->createParticleBuffer(1000, 1000, myCudaManager);
	physx::PxVec4* bufferPos = particleBuffer->getPositionInvMasses();
	myCudaManager->getCudaContext()->memcpyDtoHAsync(bufferPos, myCudaManager->getDevice(), 1000 * sizeof(physx::PxVec4), 0);
	particleBuffer->raiseFlags(physx::PxParticleBufferFlag::eUPDATE_POSITION);

	particleBuffer->setNbActiveParticles(1000);
	aParticleSystem->addParticleBuffer(particleBuffer);
}


void PhysicsEngine::ResetScene()
{
	myResetingScene = true;
	myActors.clear();
	myShapes.clear();
	myPxScene->release();
	myPxScene = nullptr;
	InitScene();
}

physx::PxRigidActor* PhysicsEngine::CreateDynamicActor(Shape aShape, Vector3f aScale, Model* aModel, std::string aMaterialName, float aMass, bool aDebugMode, Vector3f aTransformSize, bool aIsTrigger)
{
	if (myPhysxMaterialMap.find(aMaterialName) == myPhysxMaterialMap.end())
	{
		aMaterialName = "Default";
	}
	Vector3f aSize = (aScale /*(aModel->GetPivot()) * aScale*/) * 0.5f;
	physx::PxShape* actorShape = nullptr;
	std::vector<physx::PxConvexMesh*> aMesh;

	physx::PxFilterData filterData;
	filterData.word0 = 0; // word0 = own ID
	filterData.word1 = 1;
	if (aShape == Shape::PxS_Box)
	{
		if (aIsTrigger)
		{
			actorShape = myPxPhysics->createShape(physx::PxBoxGeometry(aSize.x * aTransformSize.x, aSize.y * aTransformSize.y, aSize.z * aTransformSize.z), *myPhysxMaterialMap.find(aMaterialName)->second, false, physx::PxShapeFlag::eTRIGGER_SHAPE);
		}
		else
		{
			actorShape = myPxPhysics->createShape(physx::PxBoxGeometry(aSize.x * aTransformSize.x, aSize.y * aTransformSize.y, aSize.z * aTransformSize.z), *myPhysxMaterialMap.find(aMaterialName)->second);
		}
	}
	else if (aShape == Shape::PxS_Circle)
	{
		if (aIsTrigger)
		{
			actorShape = myPxPhysics->createShape(physx::PxSphereGeometry(std::max(std::max(aSize.x, aSize.y), aSize.z)), *myPhysxMaterialMap.find(aMaterialName)->second, physx::PxShapeFlag::eTRIGGER_SHAPE);
		}
		else
		{
			actorShape = myPxPhysics->createShape(physx::PxSphereGeometry(std::max(std::max(aSize.x, aSize.y), aSize.z)), *myPhysxMaterialMap.find(aMaterialName)->second);
		}
	}
	else if (aShape == Shape::PxS_Capsule)
	{
		actorShape = myPxPhysics->createShape(physx::PxCapsuleGeometry(aScale.x, aScale.y * 0.5f), *myPhysxMaterialMap.find(aMaterialName)->second);
	}
	else if (aShape == Shape::PxS_Convex)
	{
		for (int i = 0; i < aModel->GetModelParts().size(); i++)
		{
			for (int j = 0; j < std::ceil(aModel->GetModelPart(i)->myVertices.size() / 255); j++)
			{
				int aIndex = 255;
				if (255 * j > aModel->GetModelPart(i)->myVertices.size())
				{
					aIndex = aModel->GetModelPart(i)->myVertices.size();
				}
				std::vector<Vertex> aVertexVector = std::vector<Vertex>(aModel->GetModelPart(i)->myVertices.begin(), aModel->GetModelPart(i)->myVertices.begin() + aIndex);
				aMesh.push_back(CookConvexMesh(aVertexVector, aSize * 2.0f));

			}
		}
	}
	if (!actorShape)
	{
		printerror("PxShape could not be created!");
	}
	physx::PxTransform globalTransfrom = { 0,0,0 };
	physx::PxTransform aLocalTransform = { 0,0,0 };
	if (aModel)
	{
		aLocalTransform = { (aModel->GetCenter().x * aTransformSize.x),(aModel->GetCenter().y * aTransformSize.y),(aModel->GetCenter().z * aTransformSize.z)};
	}
	physx::PxRigidActor* returnActor = myPxPhysics->createRigidDynamic(globalTransfrom);

	if (aShape != Shape::PxS_Convex)
	{
		actorShape->setLocalPose(aLocalTransform);
		actorShape->setSimulationFilterData(filterData);
		actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
	}
	else
	{
		for (int i = 0; i < aMesh.size(); i++)
		{
			if (actorShape)
			{
				returnActor->attachShape(*actorShape);
				actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
			}
			actorShape = physx::PxRigidActorExt::createExclusiveShape(*returnActor, physx::PxConvexMeshGeometry(aMesh[i]), *myPhysxMaterialMap.find(aMaterialName)->second);
		}
	}
	if (returnActor)
	{
		//actorShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		returnActor->attachShape(*actorShape);
	}

	if (actorShape) 
	{
		actorShape->release();
	}

	//dynamic_cast<physx::PxRigidDynamic*>(returnActor)->setMass(aMass);

	return returnActor;
}

physx::PxRigidStatic* PhysicsEngine::CreateStaticActor(Shape aShape, Vector3f aScale, Model* aModel, std::string aMaterialName, bool aDebugMode, Vector3f aTransformSize, Vector3f aLocalRotation, bool aIsTrigger)
{
	physx::PxFilterData filterData;
	filterData.word0 = 0; // word0 = own ID
	filterData.word1 = 1;
	if (myPhysxMaterialMap.find(aMaterialName) == myPhysxMaterialMap.end())
	{
		aMaterialName = "Default";
	}
	Vector3f aSize = (aScale /*(aModel->GetPivot()) * aScale*/) * 0.5f;
	physx::PxShape* actorShape = nullptr;
	std::vector<physx::PxConvexMesh*> aMesh;
	std::vector<physx::PxTriangleMesh*> triangleMeshList;
	if (aShape == Shape::PxS_Box)
	{
		if (aIsTrigger)
		{
			actorShape = myPxPhysics->createShape(physx::PxBoxGeometry(aSize.x * aTransformSize.x, aSize.y * aTransformSize.y, aSize.z * aTransformSize.z), *myPhysxMaterialMap.find(aMaterialName)->second, false, physx::PxShapeFlag::eTRIGGER_SHAPE);
		}
		else
		{
			actorShape = myPxPhysics->createShape(physx::PxBoxGeometry(aSize.x * aTransformSize.x, aSize.y * aTransformSize.y, aSize.z * aTransformSize.z), *myPhysxMaterialMap.find(aMaterialName)->second);
		}
	}
	else if (aShape == Shape::PxS_Circle)
	{
		if (aIsTrigger)
		{
			actorShape = myPxPhysics->createShape(physx::PxSphereGeometry(std::max(std::max(aSize.x, aSize.y), aSize.z)), *myPhysxMaterialMap.find(aMaterialName)->second, false, physx::PxShapeFlag::eTRIGGER_SHAPE);
		}
		else
		{
			actorShape = myPxPhysics->createShape(physx::PxSphereGeometry(std::max(std::max(aSize.x, aSize.y), aSize.z)), *myPhysxMaterialMap.find(aMaterialName)->second);
		}
	}
	else if (aShape == Shape::PxS_Capsule)
	{
		actorShape = myPxPhysics->createShape(physx::PxCapsuleGeometry(aScale.x, aScale.y * 0.5f), *myPhysxMaterialMap.find(aMaterialName)->second);
	}
	else if (aShape == Shape::PxS_Convex)
	{
		for (int i = 0; i < aModel->GetModelParts().size(); i++)
		{
			int aMax = std::ceil(aModel->GetModelPart(i)->myVertices.size() / 255);

			for (int j = 0; j <= aMax; j++)
			{
				int aIndex = 255 * (j + 1);
				if (aIndex > aModel->GetModelPart(i)->myVertices.size())
				{
					aIndex = aModel->GetModelPart(i)->myVertices.size();
				}
				std::vector<Vertex> aVertexVector = std::vector<Vertex>(aModel->GetModelPart(i)->myVertices.begin() + (255 * j), aModel->GetModelPart(i)->myVertices.begin() + aIndex);
				aMesh.push_back(CookConvexMesh(aVertexVector, aSize * 2.0f));

			}
		}
	}
	else if (aShape == Shape::PxS_Mesh)
	{
		for (int i = 0; i < aModel->GetModelParts().size(); i++)
		{
			triangleMeshList.push_back(CookTriangleMesh(*aModel->GetModelPart(i), aScale));
		}
	}
	physx::PxTransform globalTransfrom = { 0,0,0 };
	physx::PxTransform aLocalTransform = { 0,0,0 };
	Catbox::Quaternion tempQuat;

	tempQuat = Catbox::ToQuaternion(aLocalRotation);
	globalTransfrom.q.x = tempQuat.x;
	globalTransfrom.q.y = tempQuat.y;
	globalTransfrom.q.z = tempQuat.z;
	globalTransfrom.q.w = tempQuat.w;

	if (aModel && aShape != Shape::PxS_Mesh)
	{
		aLocalTransform.p = { (aModel->GetCenter().x) * aTransformSize.x,(aModel->GetCenter().y) * aTransformSize.y,(aModel->GetCenter().z) * aTransformSize.z };
	}


	physx::PxRigidStatic* returnActor = myPxPhysics->createRigidStatic(globalTransfrom);

	if (aShape == Shape::PxS_Convex)
	{
		for (int i = 0; i < aMesh.size(); i++)
		{
			actorShape = physx::PxRigidActorExt::createExclusiveShape(*returnActor, physx::PxConvexMeshGeometry(aMesh[i]), *myPhysxMaterialMap.find(aMaterialName)->second);
			actorShape->setLocalPose(aLocalTransform);
			returnActor->attachShape(*actorShape);
			actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
		}
	}
	else if (aShape == Shape::PxS_Mesh)
	{
		for (int i = 0; i < triangleMeshList.size(); i++)
		{
			actorShape = physx::PxRigidActorExt::createExclusiveShape(*returnActor, physx::PxTriangleMeshGeometry(triangleMeshList[i]), *myPhysxMaterialMap.find(aMaterialName)->second);

			returnActor->attachShape(*actorShape);
			actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
		}
	}
	else
	{
		actorShape->setLocalPose(aLocalTransform);
		actorShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, aDebugMode);
		actorShape->setSimulationFilterData(filterData);
		actorShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		returnActor->attachShape(*actorShape);
	}

	if (actorShape)
	{
		actorShape->release();
	}

	if (!actorShape)
	{
		printerror("PxShape could not be created!");
	}

	return returnActor;
}

physx::PxPBDParticleSystem* PhysicsEngine::CreateParticleSystem(ParticleType aType)
{
	physx::PxPBDParticleSystem* returnParticleSystem = NULL;
	returnParticleSystem = myPxPhysics->createPBDParticleSystem(*myCudaManager);
	const physx::PxU32 particlePhase = returnParticleSystem->createPhase(myDefaultParticleMaterial, physx::PxParticlePhaseFlags(physx::PxParticlePhaseFlag::eParticlePhaseFluid | physx::PxParticlePhaseFlag::eParticlePhaseSelfCollide));

	const physx::PxReal particleSpacing = 0.2f;
	const physx::PxReal fluidDensity = 1000.f;
	const physx::PxReal restOffset = 0.5f * particleSpacing / 0.6f;
	const physx::PxReal solidRestOffset = restOffset;
	const physx::PxReal fluidRestOffset = restOffset * 0.6f;
	const physx::PxReal renderRadius = fluidRestOffset;
	const physx::PxReal particleMass = fluidDensity * 1.333f * 3.14159f * renderRadius * renderRadius * renderRadius;
	returnParticleSystem->setRestOffset(restOffset);
	returnParticleSystem->setContactOffset(restOffset + 0.01f);
	returnParticleSystem->setParticleContactOffset(physx::PxMax(solidRestOffset + 0.01f, fluidRestOffset / 0.6f));
	returnParticleSystem->setSolidRestOffset(solidRestOffset);
	returnParticleSystem->setFluidRestOffset(fluidRestOffset);

	return returnParticleSystem;
}

physx::PxController* PhysicsEngine::CreateCharacterController(Shape aShape, Vector3f aSize, float aDensity, std::string aMaterialName, int aId)
{
	physx::PxBoxControllerDesc desc;
	desc.halfHeight = aSize.y * 0.5f;
	desc.halfForwardExtent = aSize.z * 0.5f;
	desc.halfSideExtent = aSize.x * 0.5f;
	desc.density = aDensity;
	desc.material = myPhysxMaterialMap[aMaterialName];
	desc.reportCallback = myCCTCallback;

	desc.userData = (void*)aId;

	physx::PxController* returnController = myCharacterController->createController(desc);

	returnController->getActor()->userData = (void*)aId;
	physx::PxShape* tempShape;
	returnController->getActor()->getShapes(&tempShape, 1);
	tempShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
	//tempShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);

	return returnController;
}

physx::PxMaterial* PhysicsEngine::CreateMaterial(std::string aName, Vector3f aMaterial)
{
	if (myPhysxMaterialMap.find(aName) == myPhysxMaterialMap.end())
	{
		physx::PxMaterial* createdMaterial = myPxPhysics->createMaterial(aMaterial.x, aMaterial.y, aMaterial.z);
		myPhysxMaterialMap.insert({ aName, createdMaterial });
		return createdMaterial;
	}
	return myPhysxMaterialMap.find(aName)->second;
}

physx::PxMaterial* PhysicsEngine::GetMaterial(std::string aName)
{
	if (myPhysxMaterialMap.find(aName) != myPhysxMaterialMap.end())
	{
		return myPhysxMaterialMap.find(aName)->second;
	}
	return nullptr;
}

physx::PxMaterial* PhysicsEngine::EditMaterial(std::string aName, Vector3f aMaterial)
{
	if (myPhysxMaterialMap.find(aName) != myPhysxMaterialMap.end())
	{
		myPhysxMaterialMap.find(aName)->second->release();
		physx::PxMaterial* createdMaterial = myPxPhysics->createMaterial(aMaterial.x, aMaterial.y, aMaterial.z);
		myPhysxMaterialMap.find(aName)->second = createdMaterial;
		return createdMaterial;
	}
	return CreateMaterial(aName, aMaterial);
}

void PhysicsEngine::InitScene()
{
	physx::PxSceneDesc sceneDesc(myPxPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	myPxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.simulationEventCallback = mySimCallback;
	sceneDesc.cpuDispatcher = myPxDispatcher;
	sceneDesc.filterShader = DefaultPxFilterFlags;
	//sceneDesc.cudaContextManager = myCudaManager;
	//sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS | physx::PxSceneFlag::eENABLE_PCM;
	//sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;

	myPxScene = myPxPhysics->createScene(sceneDesc);
	myCharacterController = PxCreateControllerManager(*myPxScene);

	myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	myPxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	physx::PxPvdSceneClient* pvdClient = myPxScene->getScenePvdClient();


	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	myResetingScene = false;
}

physx::PxConvexMesh* PhysicsEngine::CookConvexMesh(MeshData& aMeshData, Vector3f aSize)
{
	physx::PxConvexMeshDesc desc;
	physx::PxVec3 aData[255];
	for (int i = 0; i < aMeshData.myVertices.size(); i++)
	{
		physx::PxVec3 aVector3;
		aVector3.x = (aMeshData.myVertices[i].position.x * aSize.x) * 0.01f;
		aVector3.y = (aMeshData.myVertices[i].position.y * aSize.y) * 0.01f;
		aVector3.z = (aMeshData.myVertices[i].position.z * aSize.z) * 0.01f;
		aData[i] = aVector3;
	}

	desc.points.data = aData;
	desc.points.count = physx::PxU32(aMeshData.myVertices.size());
	desc.points.stride = sizeof(physx::PxVec3);
	desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	myCooker->cookConvexMesh(desc, buf, &result);
	switch (result)
	{
	case physx::PxConvexMeshCookingResult::eSUCCESS:
		printmsg("Mesh cooked!");
		break;
	case physx::PxConvexMeshCookingResult::eZERO_AREA_TEST_FAILED:
		printmsg("Mesh has no area! Failed!");
		break;
	case physx::PxConvexMeshCookingResult::ePOLYGONS_LIMIT_REACHED:
		break;
	case physx::PxConvexMeshCookingResult::eFAILURE:
		printmsg("Mesh failed!");
		break;
	default:
		break;
	}

	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* convexMesh = myPxPhysics->createConvexMesh(input);
	return convexMesh;
}
physx::PxConvexMesh* PhysicsEngine::CookConvexMesh(std::vector<Vertex>& aMeshData, Vector3f aSize)
{
	physx::PxConvexMeshDesc desc;
	physx::PxVec3 aData[255];
	for (int i = 0; i < aMeshData.size(); i++)
	{
		physx::PxVec3 aVector3;
		aVector3.x = (aMeshData[i].position.x * aSize.x) * 0.01f;
		aVector3.y = (aMeshData[i].position.y * aSize.y) * 0.01f;
		aVector3.z = (aMeshData[i].position.z * aSize.z) * 0.01f;
		aData[i] = aVector3;
	}

	desc.points.data = aData;
	desc.points.count = physx::PxU32(aMeshData.size());
	desc.points.stride = sizeof(physx::PxVec3);
	desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	myCooker->cookConvexMesh(desc, buf, &result);
	switch (result)
	{
	case physx::PxConvexMeshCookingResult::eSUCCESS:
		printmsg("Mesh cooked!");
		break;
	case physx::PxConvexMeshCookingResult::eZERO_AREA_TEST_FAILED:
		printmsg("Mesh has no area! Failed!");
		break;
	case physx::PxConvexMeshCookingResult::ePOLYGONS_LIMIT_REACHED:
		break;
	case physx::PxConvexMeshCookingResult::eFAILURE:
		printmsg("Mesh failed!");
		break;
	default:
		break;
	}

	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* convexMesh = myPxPhysics->createConvexMesh(input);
	return convexMesh;
}
physx::PxTriangleMesh* PhysicsEngine::CookTriangleMesh(const MeshData& aMeshData, Vector3f aSize)
{
	physx::PxTriangleMeshDesc desc;

	std::vector<Vertex> aData;
	for (int i = 0; i < aMeshData.myNumberOfVertices; i++)
	{
		Vertex aVector3 = aMeshData.myVertices[i];
		aVector3.position.x = (aVector3.position.x * aSize.x) * 0.01f;
		aVector3.position.y = (aVector3.position.y * aSize.y) * 0.01f;
		aVector3.position.z = (aVector3.position.z * aSize.z) * 0.01f;
		aData.push_back(aVector3);
	}

	desc.points.count = aMeshData.myNumberOfVertices;
	desc.points.data = aData.data();
	desc.points.stride = sizeof(Vertex);

	desc.triangles.count = aMeshData.myNumberOfIndices / 3;
	desc.triangles.data = aMeshData.myIndices.data();
	desc.triangles.stride = sizeof(unsigned int) * 3;

#ifdef _DEBUG
	//bool res = PxValidateTriangleMesh(myPxToleranceScale, desc);
#endif // _DEBUG
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxTriangleMeshCookingResult::Enum result;
	myCooker->cookTriangleMesh(desc, buf, &result);
	switch (result)
	{
	case physx::PxTriangleMeshCookingResult::eSUCCESS:
		printmsg("Mesh cooked!");
		break;
	case physx::PxTriangleMeshCookingResult::eFAILURE:
		printmsg("Mesh failed!");
		break;
	default:
		break;
	}

	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());

	physx::PxTriangleMesh* aTriangleMesh = myPxPhysics->createTriangleMesh(input);

	return aTriangleMesh;
}
void PhysicsEngine::SendCallbacks()
{
	for (int i = 0; i < myCallbacksParameters.size(); i++)
	{
		myCallbacks[i](myCallbacksParameters[i]);
	}
	myCallbacksParameters.clear();
	myCallbacks.clear();
}

void DefaultSimulationCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{
}

void DefaultSimulationCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
{
	
}

void DefaultSimulationCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
{

}

void DefaultSimulationCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	for (physx::PxU32 i = 0; i < nbPairs; i++)
	{
		//Shape is removed but does not get deleted. Gotta Check for this
		if (pairs[i].flags & (physx::PxContactPairFlag::eREMOVED_SHAPE_0 | physx::PxContactPairFlag::eREMOVED_SHAPE_1))
		{
			continue;
		}
		else
		{
			GameObject* gameObject0 = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)pairHeader.actors[0]->userData);
			GameObject* gameObject1 = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)pairHeader.actors[1]->userData);
			if (gameObject0 && gameObject1)
			{
				Collider* tempCollider0 = nullptr;
				Collider* tempCollider1 = nullptr;
				if (gameObject1->GetComponent<BoxCollider>())
				{
					tempCollider1 = gameObject1->GetComponent<BoxCollider>();
				}
				else if (gameObject1->GetComponent<SphereCollider>())
				{
					tempCollider1 = gameObject1->GetComponent<SphereCollider>();
				}
				else if (gameObject1->GetComponent<ConvexCollider>())
				{
					tempCollider1 = gameObject1->GetComponent<ConvexCollider>();
				}
				else if (gameObject1->GetComponent<MeshCollider>())
				{
					tempCollider1 = gameObject1->GetComponent<MeshCollider>();
				}

				if (gameObject0->GetComponent<BoxCollider>())
				{
					tempCollider0 = gameObject0->GetComponent<BoxCollider>();
				}
				else if (gameObject0->GetComponent<SphereCollider>())
				{
					tempCollider0 = gameObject0->GetComponent<SphereCollider>();
				}
				else if (gameObject0->GetComponent<ConvexCollider>())
				{
					tempCollider0 = gameObject0->GetComponent<ConvexCollider>();
				}
				else if (gameObject0->GetComponent<MeshCollider>())
				{
					tempCollider0 = gameObject0->GetComponent<MeshCollider>();
				}

				if (tempCollider0 && tempCollider1) 
				{

					if (pairs[i].flags & (physx::PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH))
					{

						gameObject1->OnCollisionEnter(tempCollider0);
						gameObject0->OnCollisionEnter(tempCollider1);
					}
					else if(pairs[i].flags & (physx::PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH))
					{
						gameObject1->OnCollisionExit(tempCollider0);
						gameObject0->OnCollisionExit(tempCollider1);
					}
				}
			}
		}
	}
}

void DefaultSimulationCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (int i = 0; i < count; i++)
	{
		if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER))
		{
			continue;
		}
		else
		{
			GameObject* triggerObject = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)pairs[i].triggerActor->userData);
			GameObject* otherObject = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)pairs[i].otherActor->userData);
			if (triggerObject && otherObject)
			{
				Collider* tempCollider = nullptr;
				if (otherObject->GetComponent<BoxCollider>())
				{
					tempCollider = otherObject->GetComponent<BoxCollider>();
				}
				else if (otherObject->GetComponent<SphereCollider>())
				{
					tempCollider = otherObject->GetComponent<SphereCollider>();
				}
				else if (otherObject->GetComponent<ConvexCollider>())
				{
					tempCollider = otherObject->GetComponent<ConvexCollider>();
				}
				else if (otherObject->GetComponent<MeshCollider>())
				{
					tempCollider = otherObject->GetComponent<MeshCollider>();
				}
				triggerObject->OnTriggerEnter(tempCollider);
			}
		}
	}
}

void DefaultSimulationCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
	printmsg("sdsdsdsds");
}


//Character Controller Callback
void DefaultCharacterControllerCallback::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	GameObject* controller = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)hit.controller->getUserData());
	if ((int)hit.actor->userData != -1)
	{
		Collider* tempCollider = nullptr;
		GameObject* shape = Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)hit.actor->userData);
		if (shape)
		{
			if (shape->GetComponent<BoxCollider>())
			{
				tempCollider = shape->GetComponent<BoxCollider>();
			}
			else if (shape->GetComponent<SphereCollider>())
			{
				tempCollider = shape->GetComponent<SphereCollider>();
			}
			else if (shape->GetComponent<ConvexCollider>())
			{
				tempCollider = shape->GetComponent<ConvexCollider>();
			}
			else if (shape->GetComponent<MeshCollider>())
			{
				tempCollider = shape->GetComponent<MeshCollider>();
			}
			//if (!tempCollider->GetIsTrigger())
			//{
			//	shape->OnCollisionStay(controller->GetComponent<BoxCollider>());
			//}
			//else 
			//{
			//	shape->OnTriggerStay(controller->GetComponent<BoxCollider>());
			//}
			tempCollider->GetGameObject().OnCollisionStay(controller->GetComponent<BoxCollider>());
			controller->OnCollisionStay(tempCollider);
		}
	}
}

void DefaultCharacterControllerCallback::onControllerHit(const physx::PxControllersHit& hit)
{
	//printmsg("Controller hit");
}

void DefaultCharacterControllerCallback::onObstacleHit(const physx::PxControllerObstacleHit& hit)
{
}

bool DefaultCCFCallback::filter(const physx::PxController& a, const physx::PxController& b)
{
	//Fuck them Rats
	/*if (Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)a.getUserData())->GetComponent<BoxCollider>() &&
		Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)b.getUserData())->GetComponent<BoxCollider>())
	{
		if (Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)a.getUserData())->GetComponent<BoxCollider>()->GetLayer() == 4 &&
			Engine::GetInstance()->GetGameObjectFactory()->GetObjectById((int)b.getUserData())->GetComponent<BoxCollider>()->GetLayer() == 4)
		{
			return false;
		}
	}*/

	return true;
}

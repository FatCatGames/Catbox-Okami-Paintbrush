#include "stdafx.h"
#include "CharacterController.h"
#include "Components/3D/ModelInstance.h"
#include "Components/Physics/Collisions/BoxCollider.h"
#include "Assets/Material.h"
#include "Physics/PhysicsEngine.h"
#include "Collisions/CollisionManager.h"
#include "Navigation/NavAgent.h"


CharacterController::~CharacterController()
{
	if (myCharacterController)
	{
		myCharacterController->release();
	}
}

void CharacterController::Awake()
{
	if (myGameObject->GetComponent<BoxCollider>())
	{
		int aLayerID = myGameObject->GetComponent<BoxCollider>()->GetCollisionLayer();

		if (myGameObject->GetComponent<ModelInstance>())
		{
			myCharacterController = Engine::GetInstance()->GetPhysicsEngine()->CreateCharacterController(Shape::PxS_Box, myGameObject->GetComponent<BoxCollider>()->GetSize(), myDensity, "Default", myGameObject->GetObjectInstanceID());
		}
		else
		{
			myCharacterController = Engine::GetInstance()->GetPhysicsEngine()->CreateCharacterController(Shape::PxS_Box, myGameObject->GetComponent<BoxCollider>()->GetSize(), myDensity, "Default", myGameObject->GetObjectInstanceID());
		}

		physx::PxShape* tempShape;
		myCharacterController->getActor()->getShapes(&tempShape, 1);

		physx::PxFilterData queryFilterData = tempShape->getQueryFilterData();
		physx::PxFilterData simulationFilterData = tempShape->getSimulationFilterData();
		myControllerFilters = queryFilterData;
		queryFilterData.word0 = 1 << aLayerID;
		simulationFilterData.word0 = 1 << aLayerID;
		queryFilterData.word1 = 0;
		simulationFilterData.word1 = 0;

		CollisionManager* aCollisionManager = Engine::GetInstance()->GetCollisionManager().get(); //Let it dangle then, idc

		if (aCollisionManager->GetPhysicsCollisionLayers().size() < aLayerID)
		{
			for (size_t i = 0; i < aCollisionManager->GetPhysicsCollisionLayers()[aLayerID].size(); i++)
			{
				if (aCollisionManager->GetPhysicsCollisionLayers()[aLayerID][i])
				{
					queryFilterData.word1 |= 1 << i;
					simulationFilterData.word1 |= 1 << i;
					myControllerFilters.word0 |= 1 << i;
				}
			}
		}

		tempShape->setQueryFilterData(queryFilterData);
		tempShape->setSimulationFilterData(simulationFilterData);
		//myControllerFilters = queryFilterData;

	}
	if (myCharacterController)
	{
		myCharacterController->setStepOffset(myStepHeight);
		myCharacterController->setFootPosition(physx::PxExtendedVec3(myFootPosition.x, myFootPosition.y, myFootPosition.z));
		myCharacterController->setContactOffset(myContactOffset);
		myCharacterController->setUpDirection(physx::PxVec3(myUpDirection.x, myUpDirection.y, myUpDirection.z));
		myCharacterController->setSlopeLimit(mySlopeOffset);
		myCharacterController->setPosition(physx::PxExtendedVec3(myTransform->worldPos().x, myTransform->worldPos().y, myTransform->worldPos().z));
		if (myGameObject->GetComponent<NavAgent>()) 
		{
			myUsingNavAgent = true;
			myGameObject->GetComponent<NavAgent>()->SetUsingCharacterController(true);
		}
	}
}

void CharacterController::Move(Vector3f aDirection)
{
	if (!myUsingNavAgent) 
	{
		myDirection = aDirection;
	}
	else 
	{
		myGameObject->GetComponent<NavAgent>()->FindPath(aDirection);
	}
}
void CharacterController::Rotate(Vector3f aAngles)
{
	myTransform->SetWorldRot(aAngles);
}

void CharacterController::Slerp(Vector3f aAngles, float rotationPower)
{
	Catbox::Quaternion targetQuat = Catbox::ToQuaternion(aAngles);
	Catbox::Quaternion currectQuat = Catbox::ToQuaternion(myTransform->worldRot());
	physx::PxQuat targetPXQuat(targetQuat.x, targetQuat.y, targetQuat.z, targetQuat.w);
	physx::PxQuat currectPXQuat(currectQuat.x, currectQuat.y, currectQuat.z, currectQuat.w);
	myTargetRotation = physx::PxSlerp(rotationPower * deltaTime, currectPXQuat, targetPXQuat);
	Catbox::Quaternion newRotation({ myTargetRotation.x, myTargetRotation.y, myTargetRotation.z, myTargetRotation.w });

	myTransform->SetWorldRot(Catbox::ToEulerAngles(newRotation));
}

bool CharacterController::CheckCollisionDirection(CollisionDirection aDirection)
{
	if (aDirection == CollisionDirection::PxCF_Down) 
	{
		if (myCollisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN) 
		{
			return true;
		}
		return false;
	}
	else if (aDirection == CollisionDirection::PxCF_Side)
	{
		if (myCollisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_SIDES)
		{
			return true;
		}
		return false;
	}
	else if (aDirection == CollisionDirection::PxCF_Top)
	{
		if (myCollisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_UP)
		{
			return true;
		}
		return false;
	}
	return false;
}

void CharacterController::SetMovementSpeed(float aMovementSpeed)
{
	myMovmentSpeed = aMovementSpeed;
}

void CharacterController::SetStepHeight(float aHeight)
{
	myStepHeight = aHeight;
}
void CharacterController::SetContactOffset(float aOffset)
{
	myContactOffset = aOffset;
}
void CharacterController::SetFootPosition(Vector3f aPosition)
{
	myFootPosition = aPosition;
}
void CharacterController::SetUpDirection(Vector3f aDirection)
{
	myUpDirection = aDirection.GetNormalized();
}

void CharacterController::AddForce(const Vector3f& aDirection, float aForce)
{
	myVelocity += aDirection * aForce;
}

void CharacterController::SetSlopeOffset(float aOffset)
{
	mySlopeOffset = aOffset;
}

CollisionDirection CharacterController::GetCollisionDirection()
{
	if (myCollisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		return CollisionDirection::PxCF_Down;
	}
	else if (myCollisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_UP)
	{
		return CollisionDirection::PxCF_Top;
	}
	else if (myCollisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_SIDES)
	{
		return CollisionDirection::PxCF_Side;
	}
	else
	{
		return CollisionDirection::PxCF_None;
	}
}

void CharacterController::SetDensity(float aDensity)
{
	myDensity = aDensity;
}

void CharacterController::SetGravity(float aGravity)
{
	myGravity = aGravity;
}

void CharacterController::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto aList = ComponentVectorCast<CharacterController>(aComponentList);

	if (ImGui::InputFloat("Movment Speed", &myMovmentSpeed))
	{
		for (auto& component : aList)
		{
			component->SetMovementSpeed(myMovmentSpeed);
		}
	}

	if (ImGui::InputFloat("Density", &myDensity))
	{
		for (auto& component : aList)
		{
			component->SetDensity(myDensity);
		}
	}

	if (ImGui::InputFloat("Gravity", &myGravity))
	{
		for (auto& component : aList)
		{
			component->SetGravity(myGravity);
		}
	}

	if (ImGui::InputFloat("Step Height", &myStepHeight))
	{
		for (auto& component : aList)
		{
			component->SetStepHeight(myStepHeight);
		}
	}

	if (ImGui::InputFloat("Contact Offset", &myContactOffset))
	{
		for (auto& component : aList)
		{
			component->SetContactOffset(myContactOffset);
		}
	}

	if (Catbox::InputFloat("Slope Offset", &mySlopeOffset))
	{
		for (auto& component : aList)
		{
			component->SetSlopeOffset(mySlopeOffset);
		}
	}

	if (Catbox::DragFloat3("Up Direction", &myUpDirection))
	{
		for (auto& component : aList)
		{
			component->SetUpDirection(myUpDirection);
		}
	}

	if (Catbox::DragFloat3("Foot Position", &myFootPosition))
	{
		for (auto& component : aList)
		{
			component->SetFootPosition(myFootPosition);
		}
	}
	if (ImGui::IsItemFocused())
	{
		DebugDrawer::DrawSphere(myTransform->worldPos() + myFootPosition, 0.2f, Color::Green());
	}
}

void CharacterController::OnCollisionStay(Collider* aCollider)
{
	if (myCollisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		myVelocity = Vector3f(0, -myGravity, 0) * deltaTime;
	}
}

void CharacterController::StopNavPathFinding()
{
	if(myUsingNavAgent)
	{
		myGameObject->GetComponent<NavAgent>()->StopPathFinding();
	}
}

void CharacterController::Update()
{
	if (myCharacterController)
	{
		Vector3f aDirection = { 0, 0, 0 };

		if (myUsingNavAgent) 
		{
			aDirection += (myGameObject->GetComponent<NavAgent>()->GetCurrentDirection() * myMovmentSpeed) + myVelocity;
		}
		else 
		{
			aDirection += (myDirection * myMovmentSpeed) + myVelocity;
		}

		myTransform->toLocalSpace();

		myVelocity += (Vector3f(0, -myGravity, 0) * deltaTime);

		physx::PxControllerFilters myFilter;
		myFilter.mFilterData = &myControllerFilters;
		myCollisionFlags = myCharacterController->move(physx::PxVec3(aDirection.x, aDirection.y, aDirection.z) * deltaTime, 0.0f, deltaTime, myFilter);
		//myCollisionFlags = myCharacterController->move(physx::PxVec3(aDirection.x, aDirection.y, aDirection.z) * deltaTime, 0.0f, deltaTime, NULL);
		myTransform->SetWorldPos(Vector3f(myCharacterController->getFootPosition().x, myCharacterController->getFootPosition().y, myCharacterController->getFootPosition().z));
	}
}

void CharacterController::Save(rapidjson::Value& aComponentData)
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Float, float>("Movement Speed", myMovmentSpeed);
	wrapper->SaveValue<DataType::Float, float>("Step Height", myStepHeight);
	wrapper->SaveValue<DataType::Float, float>("Gravity", myGravity);
	wrapper->SaveValue<DataType::Float, float>("Contact Offset", myContactOffset);
	wrapper->SaveValue<DataType::Float, float>("Slope Offset", mySlopeOffset);
	wrapper->SaveValue<DataType::Float, float>("Density", myDensity);

	wrapper->SaveValue<DataType::Float, float>("FootPosX", myFootPosition.x);
	wrapper->SaveValue<DataType::Float, float>("FootPosY", myFootPosition.y);
	wrapper->SaveValue<DataType::Float, float>("FootPosZ", myFootPosition.z);

	wrapper->SaveValue<DataType::Float, float>("UpDirX", myUpDirection.x);
	wrapper->SaveValue<DataType::Float, float>("UpDirY", myUpDirection.y);
	wrapper->SaveValue<DataType::Float, float>("UpDirZ", myUpDirection.z);
}

void CharacterController::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Movement Speed"))
	{
		myMovmentSpeed = aComponentData["Movement Speed"].GetFloat();
		myStepHeight = aComponentData["Step Height"].GetFloat();
		myGravity = aComponentData["Gravity"].GetFloat();
		myContactOffset = aComponentData["Contact Offset"].GetFloat();
		mySlopeOffset = aComponentData["Slope Offset"].GetFloat();
		myDensity = aComponentData["Density"].GetFloat();

		myFootPosition.x = aComponentData["FootPosX"].GetFloat();
		myFootPosition.y = aComponentData["FootPosY"].GetFloat();
		myFootPosition.z = aComponentData["FootPosZ"].GetFloat();

		myUpDirection.x = aComponentData["UpDirX"].GetFloat();
		myUpDirection.y = aComponentData["UpDirY"].GetFloat();
		myUpDirection.z = aComponentData["UpDirZ"].GetFloat();
	}
}

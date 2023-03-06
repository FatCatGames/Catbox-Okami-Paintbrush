#include "stdafx.h"
#include "RigidBody.h"
#include "../Physics/Intersection.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Model.h"
#include <iostream>
#include "Collisions/BoxCollider.h"
#include "Collisions/SphereCollider.h"
#include "Physics/PhysicsEngine.h"

RigidBody::~RigidBody()
{
	if (myActor)
	{
		myActor->release();
	}
}

RigidBody::RigidBody(physx::PxRigidActor* aActor)
{
	myActor = aActor;
	myShouldCull = false;
}

void RigidBody::Awake()
{
	Model* aModel = nullptr;
	std::string aMaterialName = "";
	int myLayer = 0;
	if (myGameObject->GetComponent<ModelInstance>()) 
	{
		aModel = myGameObject->GetComponent<ModelInstance>()->GetModel().get();
		aMaterialName = myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetName();
	}
	if (myGameObject->GetComponent<BoxCollider>())
	{
		myDebugMode = myGameObject->GetComponent<BoxCollider>()->GetDebugMode();
		myActor = static_cast<physx::PxRigidActor*>(Engine::GetInstance()->GetPhysicsEngine()->CreateDynamicActor(Shape::PxS_Box, myGameObject->GetComponent<BoxCollider>()->GetSize(), aModel, aMaterialName, myMass, myDebugMode, {1,1,1}, myGameObject->GetComponent<BoxCollider>()->GetIsTrigger()));
		myLayer = myGameObject->GetComponent<BoxCollider>()->GetCollisionLayer();
	}
	else if (myGameObject->GetComponent<SphereCollider>())
	{
		myDebugMode = myGameObject->GetComponent<SphereCollider>()->GetDebugMode();
		myActor = static_cast<physx::PxRigidActor*>(Engine::GetInstance()->GetPhysicsEngine()->CreateDynamicActor(Shape::PxS_Circle, Vector3f(myGameObject->GetComponent<SphereCollider>()->GetRadius() * 2.0f, 0,0), aModel, aMaterialName, myMass, myDebugMode));
		myLayer = myGameObject->GetComponent<SphereCollider>()->GetCollisionLayer();
	}
	if (myMass <= 0 || !myGravity) 
	{
		myActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	}

	//else if (myGameObject->GetComponent<CapsuleCollider>())
	//{
	//	myActor = static_cast<physx::PxRigidActor*>(Engine::GetInstance()->GetPhysicsEngine()->CreateActor(Shape::PxS_Capsule, myTransform->worldScale(), aModel));
	//}
	//else if (myGameObject->GetComponent<ConvexCollider>())
	//{
	//	myActor = static_cast<physx::PxRigidActor*>(Engine::GetInstance()->GetPhysicsEngine()->CreateActor(Shape::PxS_Convex, myTransform->worldScale(), aModel));
	//}
	if (myActor) 
	{
		physx::PxTransform aTransfrom = myActor->getGlobalPose();
		aTransfrom.p.x = myTransform->worldPos().x;
		aTransfrom.p.y = myTransform->worldPos().y;
		aTransfrom.p.z = myTransform->worldPos().z;
		Catbox::Quaternion aRot = Catbox::ToQuaternion(myTransform->worldRot());
		aTransfrom.q.x = aRot.x;
		aTransfrom.q.y = aRot.y;
		aTransfrom.q.z = aRot.z;
		aTransfrom.q.w = aRot.w;
		myActor->setGlobalPose(aTransfrom);

		//Sets Lock Flags set in Editor, if it doesn't do it here then changes will only work during playmode.
		physx::PxRigidDynamic* castedActor = static_cast<physx::PxRigidDynamic*>(myActor);
		castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, myLockTranslations[0]);
		castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, myLockTranslations[1]);
		castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, myLockTranslations[2]);
		castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, myLockRotations[0]);
		castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, myLockRotations[1]);
		castedActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, myLockRotations[2]);

		Engine::GetInstance()->GetPhysicsEngine()->AddActor(myActor, myGameObject->GetObjectInstanceID(), myLayer);
	}
}

void RigidBody::RenderInProperties(std::vector<Component*>& aComponentList)
{
	std::vector<RigidBody*>& aComponentVector = ComponentVectorCast<RigidBody>(aComponentList);
	if(ImGui::Checkbox("Use Gravity", &myGravity))
	{
		for (auto& component : aComponentVector)
		{
			component->myGravity = myGravity;
		}
	}
	if (ImGui::InputFloat("Mass", &myMass))
	{
		for (auto& component : aComponentVector)
		{
			component->myMass = myMass;
		}
	}

	ImGui::Text("Lock Translation");
	if (ImGui::Checkbox("X", &myLockTranslations[0]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[0] = myLockTranslations[0];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, myLockTranslations[0]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[0] = myLockTranslations[0];
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Y", &myLockTranslations[1]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[1] = myLockTranslations[1];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, myLockTranslations[1]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[1] = myLockTranslations[1];
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Z", &myLockTranslations[2]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[2] = myLockTranslations[2];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, myLockTranslations[2]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockTranslations[2] = myLockTranslations[2];
			}
		}
	}
	ImGui::Text("Lock Rotation");

	if (ImGui::Checkbox("X##", &myLockRotations[0]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[0] = myLockRotations[0];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, myLockRotations[0]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[0] = myLockRotations[0];
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Y##", &myLockRotations[1]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[1] = myLockRotations[1];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, myLockRotations[1]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[1] = myLockRotations[1];
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Z##", &myLockRotations[2]))
	{
		if (myActor)
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[2] = myLockRotations[2];
				static_cast<physx::PxRigidDynamic*>(component->myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, myLockRotations[2]);
				static_cast<physx::PxRigidDynamic*>(component->myActor)->wakeUp();
			}
		}
		else
		{
			for (auto& component : aComponentVector)
			{
				component->myLockRotations[2] = myLockRotations[2];
			}
		}
	}
}

void RigidBody::Update()
{
	//float x = myActor->getGlobalPose().p.x /*+ (myTransform->worldScale().x * 0.5f)*/;
	//float y = myActor->getGlobalPose().p.y /*- (myTransform->worldScale().y * 0.5f)*/;
	//float z = myActor->getGlobalPose().p.z /*- (myTransform->worldScale().z * 0.5f)*/;
	//Catbox::Quaternion aQuat;
	//aQuat.w = myActor->getGlobalPose().q.w;
	//aQuat.x = myActor->getGlobalPose().q.x;
	//aQuat.y = myActor->getGlobalPose().q.y;
	//aQuat.z = myActor->getGlobalPose().q.z;
	//Vector3f aVector = Catbox::ToEulerAngles(aQuat);
	//myTransform->SetWorldRot(aVector);
	//myTransform->SetWorldPos({ x , y , z });
}

void RigidBody::FixedUpdate()
{
	float x = myActor->getGlobalPose().p.x /*+ (myTransform->worldScale().x * 0.5f)*/;
	float y = myActor->getGlobalPose().p.y /*- (myTransform->worldScale().y * 0.5f)*/;
	float z = myActor->getGlobalPose().p.z /*- (myTransform->worldScale().z * 0.5f)*/;
	Catbox::Quaternion aQuat;
	aQuat.w = myActor->getGlobalPose().q.w;
	aQuat.x = myActor->getGlobalPose().q.x;
	aQuat.y = myActor->getGlobalPose().q.y;
	aQuat.z = myActor->getGlobalPose().q.z;
	Vector3f aVector = Catbox::ToEulerAngles(aQuat);
	myTransform->SetWorldRot(aVector);
	myTransform->SetWorldPos({ x , y , z });

	if (myScheduledForce != Vector3f::zero())
	{
		AddForce(myScheduledForce, 1);
		myScheduledForce = { 0, 0, 0 };
	}
}

RigidBody::RigidBodyData& RigidBody::GetData()
{
	return myRbData;
}

Vector3f& RigidBody::GetVelocity()
{
	return myVelocity;
}
void RigidBody::SetActorPosition(const Vector3f& aPos) 
{
	physx::PxTransform atransfrom;
	atransfrom.p.x = aPos.x;
	atransfrom.p.y = aPos.y;
	atransfrom.p.z = aPos.z;
	atransfrom.q = myActor->getGlobalPose().q;
	myActor->setGlobalPose(atransfrom);
}
void RigidBody::SetActorRotation(const Vector3f& aRot)
{
	physx::PxTransform atransfrom;
	Catbox::Quaternion aQuat = Catbox::ToQuaternion(aRot);
	atransfrom.q.x = aQuat.x;
	atransfrom.q.y = aQuat.y;
	atransfrom.q.z = aQuat.z;
	atransfrom.q.w = aQuat.w;

	atransfrom.p = myActor->getGlobalPose().p;
	myActor->setGlobalPose(atransfrom);
}


void RigidBody::SetVelocity(Vector3f aDirection, float aForce)
{
	physx::PxVec3 velocityDirection =
	{
		aDirection.x,
		aDirection.y,
		aDirection.z
	};
	velocityDirection = velocityDirection.getNormalized() * aForce;
	static_cast<physx::PxRigidDynamic*>(myActor)->setLinearVelocity(velocityDirection);
}

void RigidBody::SetAngularVelocity(Vector3f aDirection, float aForce)
{
	physx::PxVec3 velocityDirection =
	{
		aDirection.x,
		aDirection.y,
		aDirection.z
	};
	velocityDirection = velocityDirection.getNormalized() * aForce;
	static_cast<physx::PxRigidDynamic*>(myActor)->setAngularVelocity(velocityDirection);
}

Vector3f RigidBody::AddForce(Vector3f aDirection, float aForce)
{
	static_cast<physx::PxRigidDynamic*>(myActor)->addForce(physx::PxVec3(aDirection.x, aDirection.y, aDirection.z) * aForce);
	return { aDirection.x * aForce, aDirection.y * aForce, aDirection.z * aForce };
}

Vector3f RigidBody::AddForceAtPos(const float aForce, const Vector3f aPosition)
{
	physx::PxVec3 forceDirection =
	{
		myActor->getGlobalPose().p.x - aPosition.x,
		myActor->getGlobalPose().p.y - aPosition.y,
		myActor->getGlobalPose().p.z - aPosition.z
	};
	forceDirection = forceDirection.getNormalized() * aForce;
	physx::PxRigidBodyExt::addForceAtPos(*static_cast<physx::PxRigidBody*>(myActor), forceDirection, physx::PxVec3(aPosition.x, aPosition.y, aPosition.z));
	return { forceDirection.x, forceDirection.y, forceDirection.z };
}

void RigidBody::ScheduleForce(const Vector3f aPosition)
{
	myScheduledForce = aPosition;
}

void RigidBody::ChangeGravityScale(bool aGravityScale)
{
	myGravity = aGravityScale;
}

void RigidBody::ChangeMass(float aMass)
{
	static_cast<physx::PxRigidDynamic*>(myActor)->setMass(aMass);
}

void RigidBody::SetTranslationAxisLock(bool x, bool y, bool z, bool shouldWakeUp)
{
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, x);
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, y);
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, z);
	
	myLockTranslations[0] = x;
	myLockTranslations[1] = y;
	myLockTranslations[2] = z;

	if (shouldWakeUp)
	{
		static_cast<physx::PxRigidDynamic*>(myActor)->wakeUp();
	}
}

void RigidBody::SetRotationAxisLock(bool x, bool y, bool z, bool shouldWakeUp)
{
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, x);
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, y);
	static_cast<physx::PxRigidDynamic*>(myActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, z);

	myLockRotations[0] = x;
	myLockRotations[1] = y;
	myLockRotations[2] = z;

	if (shouldWakeUp)
	{
		static_cast<physx::PxRigidDynamic*>(myActor)->wakeUp();
	}
}

bool RigidBody::GetTranslationAxisLock(Axis anAxis)
{
	physx::PxU8 flags = static_cast<physx::PxRigidDynamic*>(myActor)->getRigidDynamicLockFlags();
	switch (anAxis)
	{
	case Axis::x:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
	case Axis::y:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
	case Axis::z:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
	default:
		return false;
	}
}

bool RigidBody::GetRotationAxisLock(Axis anAxis)
{
	physx::PxU8 flags = static_cast<physx::PxRigidDynamic*>(myActor)->getRigidDynamicLockFlags();
	switch (anAxis)
	{
	case Axis::x:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
	case Axis::y:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
	case Axis::z:
		return flags & physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
	default:
		return false;
	}
}

void RigidBody::Save(rapidjson::Value& /*aComponentData*/)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Bool, bool>("Use Gravity", myGravity);
	wrapper.SaveValue<DataType::Float, float>("Mass", myMass);

	wrapper.SaveValue<DataType::Bool, bool>("Lock Translation X", myLockTranslations[0]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Translation Y", myLockTranslations[1]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Translation Z", myLockTranslations[2]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Rotation X", myLockRotations[0]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Rotation Y", myLockRotations[1]);
	wrapper.SaveValue<DataType::Bool, bool>("Lock Rotation Z", myLockRotations[2]);
}
void RigidBody::Load(rapidjson::Value& aComponentData)
{	
	if (aComponentData.HasMember("Use Gravity")) 
	{
		myGravity = aComponentData["Use Gravity"].GetBool();
		myMass = aComponentData["Mass"].GetFloat();
	}

	if (aComponentData.HasMember("Lock Translation X"))
	{
		myLockTranslations[0] = aComponentData["Lock Translation X"].GetBool();
		myLockTranslations[1] = aComponentData["Lock Translation Y"].GetBool();
		myLockTranslations[2] = aComponentData["Lock Translation Z"].GetBool();
		myLockRotations[0] = aComponentData["Lock Rotation X"].GetBool();
		myLockRotations[1] = aComponentData["Lock Rotation Y"].GetBool();
		myLockRotations[2] = aComponentData["Lock Rotation Z"].GetBool();
	}		
}
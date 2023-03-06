#include "stdafx.h"
#include "CapsuleCollider.h"
#include "Components/Physics/CharacterController.h"
#include "Components/3D/ModelInstance.h"
#include "Components/Physics/RigidBody.h"
#include "Assets/Material.h"

void CapsuleCollider::Awake()
{
	if (!myGameObject->GetComponent<RigidBody>() && !myGameObject->GetComponent<CharacterController>())
	{
		if (myGameObject->GetComponent<ModelInstance>())
		{
			myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Capsule, Vector3f(myRadius, myHeight, 0), myGameObject->GetComponent<ModelInstance>()->GetModel().get(), myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetName());
		}
		else
		{
			myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Capsule, Vector3f(myRadius, myHeight, 0));
		}
		if (myStatic)
		{
			physx::PxTransform aTransfrom = myStatic->getGlobalPose();
			aTransfrom.p.x = myTransform->worldPos().x;
			aTransfrom.p.y = myTransform->worldPos().y;
			aTransfrom.p.z = myTransform->worldPos().z;
			Catbox::Quaternion aRot = Catbox::ToQuaternion(myTransform->worldRot());
			aTransfrom.q.x = aRot.x;
			aTransfrom.q.y = aRot.y;
			aTransfrom.q.z = aRot.z;
			aTransfrom.q.w = aRot.w;
			myStatic->setGlobalPose(aTransfrom);
			Engine::GetInstance()->GetPhysicsEngine()->AddActor(myStatic, myGameObject->GetObjectInstanceID(), myLayer);
		}
	}
}

bool CapsuleCollider::IsInside(const Vector3f& aPoint) const
{
    return false;
}

void CapsuleCollider::DebugDraw()
{
}

void CapsuleCollider::RenderInProperties(std::vector<Component*>& aComponentList)
{
	ImGui::InputFloat("Radius", &myRadius);
	ImGui::InputFloat("Height", &myHeight);
}

void CapsuleCollider::Save(rapidjson::Value& aComponentData)
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	wrapper->SaveValue<DataType::Float>("Radius", myRadius);
	wrapper->SaveValue<DataType::Float>("Height", myHeight);
}

void CapsuleCollider::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Radius")) 
	{
		myRadius = aComponentData["Radius"].GetFloat();
		myHeight = aComponentData["Height"].GetFloat();
	}
}

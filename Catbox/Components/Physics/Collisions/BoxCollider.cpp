#include "stdafx.h"
#include "BoxCollider.h"
#include "CollisionManager.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Model.h"
#include "Components/Physics/RigidBody.h"
#include "Components/Physics/CharacterController.h"
#include "MuninGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"
#include "Physics/PhysicsEngine.h"



BoxCollider::BoxCollider()
{
	mySize = Vector3f::one();
	myOffset = Vector3f::zero();
	myColliderType = ColliderType::Box;
}

BoxCollider::~BoxCollider()
{
	if (myStatic) 
	{
		myStatic->release();
	}
}

void BoxCollider::Init()
{
	//if (myGameObject->GetComponent<ModelInstance>())
	//{
	//	mySize = (myGameObject->GetComponent<ModelInstance>()->GetModel()->GetCenter() * 2.0f);
	//	mySize.x = std::abs(mySize.x);
	//	mySize.y = std::abs(mySize.y);
	//	mySize.z = std::abs(mySize.z);
	//}
}

void BoxCollider::Awake()
{
	if (mySize.x < 0.01)
	{
		mySize.x = 0.01;
	}
	else if (mySize.y < 0.01)
	{
		mySize.y = 0.01;
	}
	else if (mySize.z < 0.01)
	{
		mySize.z = 0.01;
	}
	if (!myGameObject->GetComponent<RigidBody>() && !myGameObject->GetComponent<CharacterController>())
	{
		if (myGameObject->GetComponent<ModelInstance>())
		{
			myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Box, mySize, myGameObject->GetComponent<ModelInstance>()->GetModel().get(), myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetName()
																					, myDebugMode, myTransform->worldScale(), myTransform->worldRot(), myIsTrigger);
		}
		else
		{
			myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Box, mySize, nullptr, "Default", myDebugMode, myTransform->worldScale(), myTransform->worldRot(), myIsTrigger);
		}
		if (myStatic)
		{
			physx::PxTransform aTransfrom = myStatic->getGlobalPose();
			aTransfrom.p.x = myTransform->worldPos().x;
			aTransfrom.p.y = myTransform->worldPos().y;
			aTransfrom.p.z = myTransform->worldPos().z;
			Catbox::Quaternion aRot = Catbox::ToQuaternion({0,0,0});
			aTransfrom.q.x = aRot.x;
			aTransfrom.q.y = aRot.y;
			aTransfrom.q.z = aRot.z;
			aTransfrom.q.w = aRot.w;
			myStatic->setGlobalPose(aTransfrom);
			Engine::GetInstance()->GetPhysicsEngine()->AddActor(myStatic, myGameObject->GetObjectInstanceID(), myLayer);
		}
	}
	
	//Collider::Awake();
}

void BoxCollider::SetSize(const Vector3f& aSize)
{
	mySize = aSize;
}

void BoxCollider::SetOffset(const Vector3f& anOffset)
{
	myOffset = anOffset;
}

bool BoxCollider::IsInside(const Vector3f& aPoint) const
{
	float epsilon = 0.0001f;
	const Vector3f min = GetMinCorner();
	const Vector3f max = GetMaxCorner();

	if (aPoint.x < min.x - epsilon) return false;
	if (aPoint.x > max.x + epsilon) return false;
	if (aPoint.y < min.y - epsilon) return false;
	if (aPoint.y > max.y + epsilon) return false;
	if (aPoint.z < min.z - epsilon) return false;
	if (aPoint.z > max.z + epsilon) return false;

	return true;
}


const Vector3f BoxCollider::GetMinCorner() const
{
	return myTransform->worldPos() + myOffset - (mySize * myTransform->worldScale() / 2.f);
}

const Vector3f BoxCollider::GetMaxCorner() const
{
	return myTransform->worldPos() + myOffset + (mySize * myTransform->worldScale() / 2.f);
}

void BoxCollider::RunInEditor()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}

void BoxCollider::Update()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}

void BoxCollider::DebugDraw()
{
	if (myGameObject->GetComponent<ModelInstance>() && myGameObject->GetComponent<ModelInstance>()->GetModel())
	{
		DebugDrawer::DrawCube(myTransform->worldPos(), myTransform->worldScale() * mySize, myTransform->worldRot(), myCurrentCollisions.empty() ? Color::Green() : Color::Red(), (myGameObject->GetComponent<ModelInstance>()->GetModel()->GetPivot() * 0.5f) * myTransform->worldScale());
	}
	else 
	{
		DebugDrawer::DrawCube(myTransform->worldPos() + myOffset, myTransform->worldScale() * mySize, Vector3f::zero(), myCurrentCollisions.empty() ? Color::Green() : Color::Red());
	}
}


void BoxCollider::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto colliders = ComponentVectorCast<BoxCollider>(aComponentList);
	Vector3f size = mySize;
	Vector3f offset = myOffset;
	int layer = myLayer;


	for (auto& collider : colliders)
	{

		if (collider->myLayer != myLayer)
		{
			layer = -1;
			break;
		}
	}

	std::vector<const char*> layerNames;
	auto& collisionLayers = Engine::GetInstance()->GetCollisionManager()->GetCollisionLayers();
	for (auto& layerName : collisionLayers)
	{
		layerNames.push_back(layerName.c_str());
	}

	if (ImGui::Checkbox("Is Trigger", &myIsTrigger))
	{
		for (auto& collider : colliders)
		{
			collider->myIsTrigger = myIsTrigger;
		}
	}

	if (ImGui::Combo("Layer", &layer, layerNames.data(), static_cast<int>(layerNames.size())))
	{
		for (auto& collider : colliders)
		{
			collider->SetLayer(layer);
		}
	}

	if (Catbox::DragFloat3("Size", &mySize, 0.01f, 0, 1000.f))
	{
		for (auto& collider : colliders)
		{
			if (mySize.x < 0.01) 
			{
				mySize.x = 0.01;
			}
			else if (mySize.y < 0.01)
			{
				mySize.y = 0.01;
			}
			else if (mySize.z < 0.01)
			{
				mySize.z = 0.01;
			}
			collider->mySize = mySize;
		}
	}

	if (Catbox::DragFloat3("Offset", &myOffset, 0.01f, 1000.f, 1000.f))
	{
		for (auto& collider : colliders)
		{
			collider->myOffset = myOffset;
		}
	}

	Catbox::Checkbox("Debug Mode", &myDebugMode);

}

void BoxCollider::OnCollisionEnter(Collider* aCollider)
{
	//printmsg("Enter");
}

void BoxCollider::OnTriggerEnter(Collider* aCollider)
{
	//printmsg(aCollider->GetGameObject().GetName());
}

void BoxCollider::OnCollisionExit(Collider* aCollider)
{
	//printmsg("Exit");
}

void BoxCollider::Save(rapidjson::Value& /*aComponentData*/)
{
	int version = 1;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();
	wrapper.SaveValue<DataType::Int>("Version", version);

	wrapper.SaveValue<DataType::Int>("Layer", myLayer);

	wrapper.SaveValue<DataType::Bool>("Trigger", myIsTrigger);

	auto sizeArray = wrapper.CreateArray();
	sizeArray.SetArray();
	sizeArray.PushBack(mySize.x, alloc);
	sizeArray.PushBack(mySize.y, alloc);
	sizeArray.PushBack(mySize.z, alloc);
	wrapper.SaveObject("Size", sizeArray);

	auto offsetArray = wrapper.CreateArray();
	offsetArray.SetArray();
	offsetArray.PushBack(myOffset.x, alloc);
	offsetArray.PushBack(myOffset.y, alloc);
	offsetArray.PushBack(myOffset.z, alloc);
	wrapper.SaveObject("Offset", offsetArray);
}

void BoxCollider::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();
	if (version >= 1)
	{
		myIsTrigger = aComponentData["Trigger"].GetBool();
	}
	if (!myHasStarted)
	{
		myLayer = aComponentData["Layer"].GetInt();
	}
	else SetLayer(aComponentData["Layer"].GetInt());
	mySize.x = aComponentData["Size"][0].GetFloat();
	mySize.y = aComponentData["Size"][1].GetFloat();
	mySize.z = aComponentData["Size"][2].GetFloat();

	myOffset.x = aComponentData["Offset"][0].GetFloat();
	myOffset.y = aComponentData["Offset"][1].GetFloat();
	myOffset.z = aComponentData["Offset"][2].GetFloat();
}

#include "stdafx.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CommonUtilities\RapidJsonWrapper.h"
#include "Components/3D/ModelInstance.h"
#include "Components/Physics/RigidBody.h"
#include "Assets/Model.h"
#include "Physics/PhysicsEngine.h"

SphereCollider::SphereCollider()
{
	myOffset = { 0,0,0 };
	myRadius = 0.5f;
	myColliderType = ColliderType::Sphere;
}

SphereCollider::~SphereCollider()
{
	if (myStatic)
	{
		myStatic->release();
	}
}


void SphereCollider::Awake()
{
	if (!myGameObject->GetComponent<RigidBody>()) 
	{
		if (myGameObject->GetComponent<ModelInstance>()) 
		{
			myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Circle, Vector3f(GetRadius() * 2.0f, 0, 0), myGameObject->GetComponent<ModelInstance>()->GetModel().get(), myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetName(), myDebugMode, myTransform->worldScale());
		}
		else 
		{
			myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Circle, Vector3f(GetRadius() * 2.0f, 0, 0), nullptr, "Default", myDebugMode, myTransform->worldScale());
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

	Collider::Awake();
}

bool SphereCollider::IsInside(const Vector3f& aPoint) const
{
	float dist = (GetPosition() - aPoint).LengthSqr();
	return dist <= (myRadius * myRadius);
}

const Vector3f SphereCollider::GetPosition() const
{
	return myTransform->worldPos() + myOffset;
}

void SphereCollider::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto colliders = ComponentVectorCast<SphereCollider>(aComponentList);
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

	if (ImGui::Combo("Layer", &layer, layerNames.data(), static_cast<int>(layerNames.size())))
	{
		for (auto& collider : colliders)
		{
			collider->SetLayer(layer);
		}
	}

	if (Catbox::DragFloat("Radius", &myRadius, 0.01f, 0, 1000.f))
	{
		for (auto& collider : colliders)
		{
			collider->myRadius = myRadius;
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

void SphereCollider::RunInEditor()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}

void SphereCollider::Update()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}

void SphereCollider::DebugDraw()
{
	auto col = myCurrentCollisions.empty() ? Color::Green() : Color::Red();
	if (myGameObject->GetComponent<ModelInstance>() && myGameObject->GetComponent<ModelInstance>()->GetModel())
	{
		DebugDrawer::DrawSphere(GetPosition() + myGameObject->GetComponent<ModelInstance>()->GetModel()->GetPivot() * 0.5f * myTransform->worldScale(), GetRadius(), col);
	}
	else 
	{
		DebugDrawer::DrawSphere(GetPosition(), GetRadius(), col);
	}
}

void SphereCollider::Save(rapidjson::Value& /*aComponentData*/)
{
	int version = 0;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();
	wrapper.SaveValue<DataType::Int>("Version", version);

	wrapper.SaveValue<DataType::Int>("Layer", myLayer);
	wrapper.SaveValue<DataType::Float>("Radius", myRadius);

	auto offsetArray = wrapper.CreateArray();
	offsetArray.SetArray();
	offsetArray.PushBack(myOffset.x, alloc);
	offsetArray.PushBack(myOffset.y, alloc);
	offsetArray.PushBack(myOffset.z, alloc);
	wrapper.SaveObject("Offset", offsetArray);
}

void SphereCollider::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Version")) 
	{
		//int version = aComponentData["Version"].GetInt();
		if (!myHasStarted)
		{
			myLayer = aComponentData["Layer"].GetInt();
		}
		else SetLayer(aComponentData["Layer"].GetInt());
		myRadius = aComponentData["Radius"].GetFloat();

		myOffset.x = aComponentData["Offset"][0].GetFloat();
		myOffset.y = aComponentData["Offset"][1].GetFloat();
		myOffset.z = aComponentData["Offset"][2].GetFloat();
	}
}

void SphereCollider::SetOffset(const Vector3f& anOffset)
{
	myOffset = anOffset;
}

void SphereCollider::SetRadius(const float& aRadius)
{
	myRadius = aRadius;
}

float SphereCollider::GetRadius() const
{
	return myRadius;
}
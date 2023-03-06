#include "stdafx.h"
#include "ConvexCollider.h"
#include "CollisionManager.h"
#include "CommonUtilities\RapidJsonWrapper.h"
#include "Components/3D/ModelInstance.h"
#include "Components/Physics/RigidBody.h"
#include "Assets/Model.h"
#include "Physics/PhysicsEngine.h"

ConvexCollider::ConvexCollider()
{
	myOffset = { 0,0,0 };
	myColliderType = ColliderType::undefined;
}

ConvexCollider::~ConvexCollider()
{
	if (myStatic)
	{
		myStatic->release();
	}
}


void ConvexCollider::Awake()
{
	if (!myGameObject->GetComponent<RigidBody>()) 
	{
		if (myGameObject->GetComponent<ModelInstance>()) 
		{
			myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Convex, myTransform->worldScale(), myModel.get(), myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetName(), false);
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

bool ConvexCollider::IsInside(const Vector3f& aPoint) const
{
	return false;
}

const Vector3f ConvexCollider::GetPosition() const
{
	return myTransform->worldPos() + myOffset;
}

void ConvexCollider::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto colliders = ComponentVectorCast<ConvexCollider>(aComponentList);
	bool edit = false;

	auto aModel = AssetRegistry::GetInstance()->AssetDropdownMenu<Model>(myModel.get(), "Convex Model", edit);
	if (edit) 
	{
		myModel = aModel;
	}
}

void ConvexCollider::RunInEditor()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}

void ConvexCollider::Update()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDraw();
	}
}

void ConvexCollider::Init()
{
	if (!myModel) 
	{
		if (myGameObject->GetComponent<ModelInstance>()) 
		{
			if (myGameObject->GetComponent<ModelInstance>()->GetModel()) 
			{
				myModel = myGameObject->GetComponent<ModelInstance>()->GetModel();
			}
		}
	}
}

void ConvexCollider::DebugDraw()
{
	//auto col = myCurrentCollisions.empty() ? Color::Green() : Color::Red();
	//if (myGameObject->GetComponent<ModelInstance>() && myGameObject->GetComponent<ModelInstance>()->GetModel())
	//{
	//	DebugDrawer::DrawSphere(GetPosition() + myGameObject->GetComponent<ModelInstance>()->GetModel()->GetPivot() * 0.5f, GetRadius(), col);
	//}
	//else 
	//{
	//	DebugDrawer::DrawSphere(GetPosition(), GetRadius(), col);
	//}
}

void ConvexCollider::Save(rapidjson::Value& /*aComponentData*/)
{
	int version = 1;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();
	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveString("Convex Model", myModel->GetName().c_str());

}

void ConvexCollider::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData["Version"].GetInt() > 0) 
	{
		myModel = AssetRegistry::GetInstance()->GetAsset<Model>(aComponentData["Convex Model"].GetString());
	}
}

void ConvexCollider::SetOffset(const Vector3f& anOffset)
{
	myOffset = anOffset;
}

#include "stdafx.h"
#include "MeshCollider.h"
#include "Assets/Model.h"
#include "Components/3D/ModelInstance.h"
#include "Physics/RigidBody.h"

MeshCollider::~MeshCollider()
{
	//if (myStatic)
	//{
	//	myStatic->isReleasable();
	//}
}

void MeshCollider::Awake()
{
	if (!myGameObject->GetComponent<RigidBody>())
	{
		if (myGameObject->GetComponent<ModelInstance>())
		{
			myStatic = Engine::GetInstance()->GetPhysicsEngine()->CreateStaticActor(Shape::PxS_Mesh, myTransform->worldScale(), myModel.get(), "Default", myDebugMode, { 1,1,1 }, myTransform->worldRot(), myIsTrigger);
			if (myStatic)
			{
				physx::PxTransform aTransfrom = myStatic->getGlobalPose();
				aTransfrom.p.x = myTransform->worldPos().x;
				aTransfrom.p.y = myTransform->worldPos().y;
				aTransfrom.p.z = myTransform->worldPos().z;
				Catbox::Quaternion aRot = Catbox::ToQuaternion({ 0,0,0 });
				aTransfrom.q.x = aRot.x;
				aTransfrom.q.y = aRot.y;
				aTransfrom.q.z = aRot.z;
				aTransfrom.q.w = aRot.w;
				myStatic->setGlobalPose(aTransfrom);
				Engine::GetInstance()->GetPhysicsEngine()->AddActor(myStatic, myGameObject->GetObjectInstanceID(), myLayer);
			}
		}
	}
}

void MeshCollider::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto colliders = ComponentVectorCast<MeshCollider>(aComponentList);
	bool edit = false;

	ImGui::Checkbox("Debug Mode", &myDebugMode);

	auto aModel = AssetRegistry::GetInstance()->AssetDropdownMenu<Model>(myModel.get(), "Convex Model", edit);
	if (edit)
	{
		myModel = aModel;
	}
}

void MeshCollider::Init()
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

void MeshCollider::Save(rapidjson::Value& /*aComponentData*/)
{
	int version = 1;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();
	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveString("Collider Model", myModel ? myModel->GetName().c_str() : "");

}

void MeshCollider::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData["Version"].GetInt() > 0)
	{
		//myModel = AssetRegistry::GetInstance()->GetAsset<Model>(aComponentData["Convex Model"].GetString());
	}
}

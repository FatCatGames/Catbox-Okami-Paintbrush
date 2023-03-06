#include "stdafx.h"
#include "SGNode_GetObjectIDByName.h"
#include "ComponentTools\SceneManager.h"

void SGNode_GetObjectIDByName::Init()
{
	SetTitle("Get Object ID");
	SetCategory("GameObjects");

	CreateDataPin<std::string>("Object name", PinDirection::Input);
	CreateDataPin<int>("ObjectID", PinDirection::Output);
}

size_t SGNode_GetObjectIDByName::DoOperation()
{
	std::string objectName = "";
	GetPinData("Object name", objectName);
	if (!objectName.empty())
	{
		GameObject* g = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->FindGameObject(objectName);
		if (g)
		{
			SetPinData("ObjectID", g->GetObjectInstanceID());
			return Exit();
		}
	}
	return 0;
}


void SGNode_GetChildByName::Init()
{
	SetTitle("Get Child");
	SetCategory("GameObjects");

	CreateDataPin<std::string>("Child name", PinDirection::Input);
	CreateDataPin<int>("ParentID", PinDirection::Input);
	SetPinData("ParentID", -1);
	CreateDataPin<int>("ChildID", PinDirection::Output);
	CreateDataPin<bool>("Success", PinDirection::Output);
}

GameObject* FindChild(Transform* aParent, const string& aName)
{
	if (aParent->GetGameObject()->GetName() == aName) return aParent->GetGameObject();
	for (auto child : aParent->GetChildren())
	{
		GameObject* obj = FindChild(child, aName);
		if (obj) return obj;
	}

	return nullptr;
}

size_t SGNode_GetChildByName::DoOperation()
{
	SetPinData("Success", false);

	int id = myOwner->GetObjectID();
	int otherID = -1;
	GetPinData("ParentID", otherID);
	if (otherID != -1)
	{
		id = otherID;
	}

	GameObject* parent = Engine::GetGameObject(id);
	if (!parent) return 0;

	std::string childName = "";
	GetPinData("Child name", childName);
	if (childName.empty()) return 0;

	GameObject* child = FindChild(parent->GetTransform(), childName);
	if (!child) return 0;
	SetPinData("ChildID", child->GetObjectInstanceID());
	SetPinData("Success", true);
	return Exit();
}
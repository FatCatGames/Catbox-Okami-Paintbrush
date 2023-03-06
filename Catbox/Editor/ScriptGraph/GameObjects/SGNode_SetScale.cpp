#include "stdafx.h"
#include "SGNode_SetScale.h"

void SGNode_SetScale::Init()
{
	SetTitle("Set Scale");
	SetCategory("GameObjects");

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<int>("ObjectID", PinDirection::Input);
	SetPinData("ObjectID", -1);
	CreateDataPin<Vector3f>("Scale", PinDirection::Input);
}

size_t SGNode_SetScale::DoOperation()
{
	int id = myOwner->GetObjectID();
	int otherID = -1;
	GetPinData("ObjectID", otherID);
	if (otherID != -1)
	{
		id = otherID;
	}
	GameObject* object = Engine::GetGameObject(id);
	if (object)
	{
		Vector3f scale = object->GetTransform()->worldScale();
		GetPinData("Scale", scale);
		object->GetTransform()->SetWorldScale(scale);
		return ExitViaPin("Out");
	}
	return 0;
}
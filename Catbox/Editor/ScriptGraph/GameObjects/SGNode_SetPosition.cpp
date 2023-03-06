#include "stdafx.h"
#include "SGNode_SetPosition.h"

void SGNode_SetPosition::Init()
{
	SetTitle("Set Position");
	SetCategory("GameObjects");

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<int>("ObjectID", PinDirection::Input);
	SetPinData("ObjectID", -1);
	CreateDataPin<Vector3f>("Pos", PinDirection::Input);
}

size_t SGNode_SetPosition::DoOperation()
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
		Vector3f pos = object->GetTransform()->worldPos();
		GetPinData("Pos", pos);
		object->GetTransform()->SetWorldPos(pos);
		return ExitViaPin("Out");
	}
	return 0;
}
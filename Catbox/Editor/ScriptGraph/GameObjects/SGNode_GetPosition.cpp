#include "stdafx.h"
#include "SGNode_GetPosition.h"
#include "ScriptGraph.h"

void SGNode_GetPosition::Init()
{
	SetTitle("Get Position");
	SetCategory("GameObjects");

	CreateDataPin<int>("ObjectID", PinDirection::Input);
	SetPinData("ObjectID", -1);
	CreateDataPin<Vector3f>("Pos", PinDirection::Output);
}

size_t SGNode_GetPosition::DoOperation()
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
		SetPinData("Pos", pos);
		return Exit();
	}
	return 0;
}
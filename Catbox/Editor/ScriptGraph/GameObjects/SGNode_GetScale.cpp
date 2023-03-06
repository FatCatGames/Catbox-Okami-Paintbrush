#include "stdafx.h"
#include "SGNode_GetScale.h"

void SGNode_GetScale::Init()
{
	SetTitle("Get Scale");
	SetCategory("GameObjects");

	CreateDataPin<int>("ObjectID", PinDirection::Input);
	SetPinData("ObjectID", -1);
	CreateDataPin<Vector3f>("Scale", PinDirection::Output);
}

size_t SGNode_GetScale::DoOperation()
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
		Vector3f scale = object->GetTransform()->worldPos();
		SetPinData("Scale", scale);
		return Exit();
	}
	return 0;
}
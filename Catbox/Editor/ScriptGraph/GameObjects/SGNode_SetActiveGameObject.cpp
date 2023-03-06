#include "stdafx.h"
#include "SGNode_SetActiveGameObject.h"
void SGNode_SetActiveGameObject::Init()
{
	SetTitle("Set Active");
	SetCategory("GameObjects");


	CreateExecPin("Input", PinDirection::Input, true);
	CreateExecPin("Output", PinDirection::Output, true);
	
	CreateDataPin<int>("ObjectID", PinDirection::Input);
	SetPinData<int>("ObjectID", -1);
	CreateDataPin<bool>("Active", PinDirection::Input);
}

size_t SGNode_SetActiveGameObject::DoOperation() 
{
	bool flag = false;
	int id = myOwner->GetObjectID();
	int otherID = -1;
	GetPinData("ObjectID", otherID);
	GetPinData("Active", flag);
	if (otherID != -1)
	{
		id = otherID;
	}
	Engine::GetInstance()->GetGameObjectFactory()->GetObjectById(id)->SetActive(flag);
	return ExitViaPin("Output");
}
void SGNode_GetActiveGameObject::Init()
{
	SetTitle("Get Active");
	SetCategory("GameObjects");

	CreateDataPin<bool>("Active", PinDirection::Output);

	CreateDataPin<int>("ObjectID", PinDirection::Input);
	SetPinData<int>("ObjectID", -1);
}

size_t SGNode_GetActiveGameObject::DoOperation()
{
	int id = myOwner->GetObjectID();
	int otherID = -1;
	GetPinData("ObjectID", otherID);
	if (otherID != -1)
	{
		id = otherID;
	}
	SetPinData("Active", Engine::GetInstance()->GetGameObjectFactory()->GetObjectById(id)->IsActive());
	return 0;
}
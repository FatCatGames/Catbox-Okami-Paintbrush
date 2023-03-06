#include "stdafx.h"
#include "SGNode_GetBlueprint.h"
#include "Components\BlueprintComponent.h"

void SGNode_GetBlueprint::Init()
{
	SetTitle("Get Blueprint");
	SetCategory("GameObjects");

	CreateDataPin<int>("ObjectID", PinDirection::Input);
	CreateDataPin<ScriptGraph*>("Blueprint", PinDirection::Output);
}

size_t SGNode_GetBlueprint::DoOperation()
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
		BlueprintComponent* bc = object->GetComponent<BlueprintComponent>();
		if (bc)
		{
			auto blueprint = bc->GetBlueprint();
			if (blueprint)
			{
				SetPinData("Blueprint", blueprint.get());
				return Exit();
			}
		}
	}
	SetPinData("Blueprint", nullptr);
	return 0;
}
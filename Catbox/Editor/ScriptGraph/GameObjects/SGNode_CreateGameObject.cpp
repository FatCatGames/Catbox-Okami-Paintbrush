#include "stdafx.h"
#include "SGNode_CreateGameObject.h"

void SGNode_CreateGameObject::Init()
{
	SetTitle("Create New Object");
	SetCategory("GameObjects");

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<std::string>("Prefab", PinDirection::Input);
	CreateDataPin<int>("ObjectID", PinDirection::Output);
}

size_t SGNode_CreateGameObject::DoOperation()
{
	std::string prefabName = "";
	GetPinData("Prefab", prefabName);
	if (!prefabName.empty())
	{
		GameObject* g = InstantiatePrefab(AssetRegistry::GetInstance()->GetAsset<Prefab>(prefabName));
		SetPinData("ObjectID", g->GetObjectInstanceID());
	}
	else 
	{
		GameObject* g = Instantiate();
		SetPinData("ObjectID", g->GetObjectInstanceID());
	}
	return ExitViaPin("Out");
}
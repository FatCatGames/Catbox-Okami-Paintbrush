#include "stdafx.h"
#include "BlueprintComponent.h"
#include "MuninScriptGraph.h"
#include "Assets\Blueprint.h"
#include "Components\Physics\Collisions\Collider.h"


void BlueprintComponent::Awake()
{
	if (!myBlueprint)
	{
		printerror(myGameObject->GetName() + " blueprint not set!");
		return;
	}
	myBlueprint->GetGraph()->SetObjectID(myGameObject->GetObjectInstanceID());
	myBlueprint->GetGraph()->SetTicking(true);
	myBlueprint->GetGraph()->Run("Begin Play");
}

void BlueprintComponent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	bool edit;
	auto newBlueprint = AssetRegistry::GetInstance()->AssetDropdownMenu<Blueprint>(myBlueprint.get(), "Blueprint", edit);
	if (edit)
	{
		myBlueprint = newBlueprint;
	}
}

void BlueprintComponent::Update()
{
	if (myBlueprint)
	{
		bool yes = myBlueprint->GetGraph()->Tick(deltaTime);
		yes = yes;
	}
}

void BlueprintComponent::Save(rapidjson::Value& aComponentData)
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	int version = 0;
	if (!myBlueprint) return;
	wrapper->SaveString("Blueprint", myBlueprint->GetName().c_str());
	wrapper->SaveValue<DataType::Int, int>("Version", version);
}

void BlueprintComponent::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Blueprint"))
	{
		int version = aComponentData["Version"].GetInt();
		myBlueprint = AssetRegistry::GetInstance()->GetAsset<Blueprint>(aComponentData["Blueprint"].GetString());
	}
}

void BlueprintComponent::OnCollisionEnter(Collider* aCollider)
{
	auto graph = myBlueprint->GetGraph();
	if (!graph)
	{
		printerror(myGameObject->GetName() + " has no blueprint assigned!");
		return;
	}
	ScriptGraphNodePayload tickPayload;
	tickPayload.SetVariable("Collided object ID", aCollider->GetGameObject().GetObjectInstanceID());
	graph->RunWithPayload("On Collision Enter", tickPayload);
}

void BlueprintComponent::OnCollisionStay(Collider* aCollider)
{
	auto graph = myBlueprint->GetGraph();
	if (!graph)
	{
		printerror(myGameObject->GetName() + " has no blueprint assigned!");
		return;
	}
	ScriptGraphNodePayload tickPayload;
	tickPayload.SetVariable("Collided object ID", aCollider->GetGameObject().GetObjectInstanceID());
	graph->RunWithPayload("On Collision Stay", tickPayload);
}

void BlueprintComponent::OnCollisionExit(Collider* aCollider)
{
	auto graph = myBlueprint->GetGraph();
	if (!graph)
	{
		printerror(myGameObject->GetName() + " has no blueprint assigned!");
		return;
	}
	ScriptGraphNodePayload tickPayload;
	tickPayload.SetVariable("Collided object ID", aCollider->GetGameObject().GetObjectInstanceID());
	graph->RunWithPayload("On Collision Exit", tickPayload);
}

void BlueprintComponent::OnOverlapBegin(Collider* aCollider)
{
	auto graph = myBlueprint->GetGraph();
	if (!graph)
	{
		printerror(myGameObject->GetName() + " has no blueprint assigned!");
		return;
	}
	ScriptGraphNodePayload tickPayload;
	tickPayload.SetVariable("Collided object ID", aCollider->GetGameObject().GetObjectInstanceID());
	graph->RunWithPayload("On Overlap Begin", tickPayload);
}

void BlueprintComponent::OnOverlap(Collider* aCollider)
{
	auto graph = myBlueprint->GetGraph();
	if (!graph)
	{
		printerror(myGameObject->GetName() + " has no blueprint assigned!");
		return;
	}
	ScriptGraphNodePayload tickPayload;
	tickPayload.SetVariable("Collided object ID", aCollider->GetGameObject().GetObjectInstanceID());
	graph->RunWithPayload("On Overlap", tickPayload);
}

void BlueprintComponent::OnOverlapEnd(Collider* aCollider)
{
	auto graph = myBlueprint->GetGraph();
	if (!graph)
	{
		printerror(myGameObject->GetName() + " has no blueprint assigned!");
		return;
	}
	ScriptGraphNodePayload tickPayload;
	tickPayload.SetVariable("Collided object ID", aCollider->GetGameObject().GetObjectInstanceID());
	graph->RunWithPayload("On Overlap End", tickPayload);
}
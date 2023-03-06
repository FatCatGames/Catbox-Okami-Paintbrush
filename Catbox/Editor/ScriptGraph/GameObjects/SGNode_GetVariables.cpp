#include "stdafx.h"
#include "SGNode_GetVariables.h"
#include "BlueprintComponent.h"
#include "ScriptGraph.h"
#include "Assets/Blueprint.h"

#pragma region oh god dont open it

#define DefineVariableNode(ClassName, VarName, TypeName) void ClassName::Init(){\
	std::string title = "Get ";\
	title += VarName;\
	SetTitle(title);\
	SetCategory("GameObjects");\
	CreateDataPin<int>("ObjectID", PinDirection::Input);\
	std::string pinName = VarName;\
	pinName += " name";\
	CreateDataPin<string>(pinName, PinDirection::Input);\
	CreateDataPin<TypeName>("Out", PinDirection::Output);\
}\
size_t ClassName::DoOperation()\
{\
int id = myOwner->GetObjectID(); \
int otherID = -1; \
GetPinData("ObjectID", otherID); \
if (otherID != -1) id = otherID; \
\
GameObject* object = Engine::GetGameObject(id); \
if (object)\
{\
BlueprintComponent* bc = object->GetComponent<BlueprintComponent>(); \
if (bc)\
{\
auto blueprint = bc->GetBlueprint(); \
if (blueprint)\
{\
string varName;\
string pinName = VarName;\
pinName += " name";\
GetPinData(pinName, varName);\
auto variables = blueprint->GetGraph()->GetGraphSchema()->GetVariables();\
if (variables.count(varName) != 0)\
{\
	SetRawPinData("Out", variables.at(varName)->Data.Ptr, sizeof(TypeName));\
	return Exit();\
}\
printerror("Could not get variable " + varName + " from " + object->GetName()); \
return 0; \
}\
printerror("Tried to get blueprint from " + object->GetName() + " but the BlueprintComponent is unassigned!"); \
return 0; \
}\
printerror("Tried to get blueprint from " + object->GetName() + " but it does not have a BlueprintComponent!"); \
return 0; \
}\
return 0; \
}\

#pragma endregion


void SGNode_GetBool::Init() {
	std::string title = "Get Bool";
	SetTitle(title);
	SetCategory("GameObjects");
	CreateDataPin<int>("ObjectID", PinDirection::Input);
	CreateDataPin<string>("Bool name", PinDirection::Input);
	CreateDataPin<bool>("Out", PinDirection::Output);
}

size_t SGNode_GetBool::DoOperation()
{
	int id = myOwner->GetObjectID();
	int otherID = -1;
	GetPinData("ObjectID", otherID);
	if (otherID != -1) id = otherID;

	GameObject* object = Engine::GetGameObject(id);
	if (object)
	{
		BlueprintComponent* bc = object->GetComponent<BlueprintComponent>();
		if (bc)
		{
			auto blueprint = bc->GetBlueprint();
			if (blueprint)
			{
				string varName;
				GetPinData("Bool name", varName);
				auto variables = blueprint->GetGraph()->GetGraphSchema()->GetVariables();
				if (variables.count(varName) != 0)
				{
					SetRawPinData("Out", variables.at(varName)->Data.Ptr, sizeof(bool));
					bool value = *static_cast<bool*>(variables.at(varName)->Data.Ptr);
					return Exit();
				}
				printerror("Could not get variable " + varName + " from " + object->GetName());
				return 0;
			}
			printerror("Tried to get blueprint from " + object->GetName() + " but the BlueprintComponent is unassigned!");
			return 0;
		}
		printerror("Tried to get blueprint from " + object->GetName() + " but it does not have a BlueprintComponent!");
		return 0;
	}
	return 0;
}


//DefineVariableNode(SGNode_GetBool, "Bool", bool)
DefineVariableNode(SGNode_GetInt, "Int", int)
DefineVariableNode(SGNode_GetFloat, "Float", float)
DefineVariableNode(SGNode_GetString, "String", string)
DefineVariableNode(SGNode_GetVector3, "Vector3f", Vector3f)
#include "stdafx.h"
#include "SGNode_SetVariables.h"
#include "BlueprintComponent.h"
#include "ScriptGraph.h"
#include "Assets/Blueprint.h"

#pragma region oh god dont open it

#define DefineVariableNode(ClassName, VarName, TypeName) void ClassName::Init(){\
	std::string title = "Set ";\
	title += VarName;\
	SetTitle(title);\
	SetCategory("GameObjects");\
CreateExecPin("In", PinDirection::Input, true);\
CreateExecPin("Out", PinDirection::Output, true);\
	CreateDataPin<int>("ObjectID", PinDirection::Input);\
	std::string pinName = VarName;\
	pinName += " name";\
	CreateDataPin<string>(pinName, PinDirection::Input);\
	CreateDataPin<TypeName>("Value", PinDirection::Input, true);\
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
TypeName newValue;\
GetPinData("Value", newValue);\
auto variables = blueprint->GetGraph()->GetGraphSchema()->GetVariables();\
if (variables.count(varName) != 0)\
{\
	variables.at(varName)->Data.SetData(newValue);\
	return ExitViaPin("Out");\
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


//void SGNode_SetString::Init()
//{
//	SetTitle("Set String");
//	SetCategory("GameObjects");
//	CreateExecPin("In", PinDirection::Input, true);
//	CreateExecPin("Out", PinDirection::Output, true);
//	CreateDataPin<int>("ObjectID", PinDirection::Input);
//	CreateDataPin<string>("String name", PinDirection::Input);
//	CreateDataPin<string>("Value", PinDirection::Input);
//}
//size_t SGNode_SetString::DoOperation()
//{
//	int id = myOwner->GetObjectID();
//	int otherID = -1;
//	GetPinData("ObjectID", otherID);
//	if (otherID != -1) id = otherID;
//
//	GameObject* object = Engine::GetGameObject(id);
//	if (object)
//	{
//		BlueprintComponent* bc = object->GetComponent<BlueprintComponent>();
//		if (bc)
//		{
//			auto blueprint = bc->GetBlueprint();
//			if (blueprint)
//			{
//				string varName;
//				GetPinData("String name", varName);
//				string newValue;
//				GetPinData("Value", newValue);
//				auto variables = blueprint->GetGraph()->GetGraphSchema()->GetVariables();
//				if (variables.count(varName) != 0)
//				{
//					variables.at(varName)->Data.SetData(newValue);
//					return ExitViaPin("Out");
//				}
//				printerror("Could not get variable " + varName + " from " + object->GetName());
//				return 0;
//			}
//			printerror("Tried to get blueprint from " + object->GetName() + " but the BlueprintComponent is unassigned!");
//			return 0;
//		}
//		printerror("Tried to get blueprint from " + object->GetName() + " but it does not have a BlueprintComponent!");
//		return 0;
//	}
//	return 0;
//}
// 
// 
//

DefineVariableNode(SGNode_SetBool, "Bool", bool)
DefineVariableNode(SGNode_SetInt, "Int", int)
DefineVariableNode(SGNode_SetFloat, "Float", float)
DefineVariableNode(SGNode_SetString, "String", string)
DefineVariableNode(SGNode_SetVector3, "Vector3f", Vector3f)
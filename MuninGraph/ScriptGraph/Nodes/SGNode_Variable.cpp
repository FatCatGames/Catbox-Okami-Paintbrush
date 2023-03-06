#include "MuninGraph.pch.h"
#include "SGNode_Variable.h"
#include "ScriptGraph/ScriptGraphVariable.h"

void SGNode_SetVariable::Init()
{
	SetTitle("Set");
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateVariablePin("VAR", PinDirection::Input);
	CreateVariablePin("Get", PinDirection::Output, true);
}

void SGNode_SetVariable::SetNodeVariable(const std::shared_ptr<ScriptGraphVariable>& aVariable)
{
	myVariable = aVariable;
	for (const auto& [pinUID, pin] : GetPins())
	{
		if (pin.GetType() == ScriptGraphPinType::Variable && pin.GetPinDirection() == PinDirection::Input)
		{
			RenamePin(pin.GetUID(), myVariable->Name);
		}
	}
}

size_t SGNode_SetVariable::DoOperation()
{
	if(myVariable)
	{
		if (GetRawPinData(myVariable->Name, myVariable->Data.Ptr, myVariable->GetTypeData()->GetTypeSize()))
		{
			SetRawPinData("Get", myVariable->Data.Ptr, myVariable->GetTypeData()->GetTypeSize());
		}
	}
	else
	{
		return ExitWithError("Invalid Variable!");
	}

	return ExitViaPin("Out");
}

ImVec4 SGNode_SetVariable::GetNodeHeaderColor() const
{
	ImVec4 col = ScriptGraphDataTypeRegistry::GetDataTypeColor(myVariable->GetTypeData()->GetType());
	col.w = 128.0f;
	return col;
}

void SGNode_GetVariable::Init()
{
	SetTitle("Get");
	CreateVariablePin("VAR", PinDirection::Output);
}

void SGNode_GetVariable::SetNodeVariable(const std::shared_ptr<ScriptGraphVariable>& aVariable)
{
	myVariable = aVariable;
	for(const auto& [pinUID, pin] : GetPins())
	{
		if(pin.GetType() == ScriptGraphPinType::Variable && pin.GetPinDirection() == PinDirection::Output)
		{
			RenamePin(pin.GetUID(), myVariable->Name);
		}
	}
}

size_t SGNode_GetVariable::DoOperation()
{
	if(myVariable)
	{
		SetRawPinData(myVariable->Name, myVariable->Data.Ptr, myVariable->GetTypeData()->GetTypeSize());
	}
	else
	{
		return ExitWithError("Invalid Variable!");
	}

	return Exit();
}

ImVec4 SGNode_GetVariable::GetNodeHeaderColor() const
{
	ImVec4 col = ScriptGraphDataTypeRegistry::GetDataTypeColor(myVariable->GetTypeData()->GetType());
	col.w = 128.0f;
	return col;
}

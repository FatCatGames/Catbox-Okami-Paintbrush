#include "stdafx.h"
#include "SGNode_Branch.h"

void SGNode_Branch::Init()
{
	SetTitle("Branch");
	SetCategory("Flow Control");

	CreateExecPin("Exec", PinDirection::Input, true);
	CreateDataPin<bool>("Condition", PinDirection::Input);
	CreateExecPin("True", PinDirection::Output);
	CreateExecPin("False", PinDirection::Output);
}

size_t SGNode_Branch::DoOperation()
{
	bool condition;
	GetPinData("Condition", condition);
	if (condition) return ExitViaPin("True");
	ExitViaPin("False");
}
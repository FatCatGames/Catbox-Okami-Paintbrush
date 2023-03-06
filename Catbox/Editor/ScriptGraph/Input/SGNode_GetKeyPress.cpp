#include "stdafx.h"
#include "SGNode_GetKeyPress.h"

void SGNode_GetKeyPress::Init()
{
	SetTitle("Get key press");
	SetCategory("Input");

	CreateExecPin("Exec", PinDirection::Input, true);
	CreateExecPin("True", PinDirection::Output);
	CreateExecPin("False", PinDirection::Output);
	CreateDataPin<string>("Key", PinDirection::Input);
}

size_t SGNode_GetKeyPress::DoOperation()
{
	std::string aKey;
	GetPinData("Key", aKey);
	if(Input::GetKeyPress(aKey))
	{
		return ExitViaPin("True");
	}
	return ExitViaPin("False");
}



void SGNode_GetKeyHeld::Init()
{
	SetTitle("Get key held");
	SetCategory("Input");

	CreateExecPin("Exec", PinDirection::Input, true);
	CreateExecPin("True", PinDirection::Output);
	CreateExecPin("False", PinDirection::Output);
	CreateDataPin<string>("Key", PinDirection::Input);
}

size_t SGNode_GetKeyHeld::DoOperation()
{
	std::string aKey;
	GetPinData("Key", aKey);
	if (Input::GetKeyHeld(aKey))
	{
		return ExitViaPin("True");
	}
	return ExitViaPin("False");
}

#include "stdafx.h"
#include "SGNode_Print.h"

void SGNode_Print::Init()
{
	SetTitle("Print");
	SetCategory("Utility");

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<std::string>("Text", PinDirection::Input);
}

size_t SGNode_Print::DoOperation()
{
	std::string msg;
	GetPinData("Text", msg);
	print(msg);
	return ExitViaPin("Out");
}

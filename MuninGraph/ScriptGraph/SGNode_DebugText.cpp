#include "MuninGraph.pch.h"
#include "SGNode_DebugText.h"

#include <iostream>

void SGNode_DebugText::Init()
{
	SetTitle("Debug Print");
	SetCategory("Debug");
	SetDescription("Prints a provided text string to stdout.");

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<std::string>("Text", PinDirection::Input);
}

size_t SGNode_DebugText::DoOperation()
{
	std::string msg;
	if(GetPinData("Text", msg))
	{
		std::cout << msg << std::endl;
	}

	return ExitViaPin("Out");
}

ImVec4 SGNode_DebugText::GetNodeHeaderColor() const
{
	return ImVec4(200, 150, 0, 255);
}

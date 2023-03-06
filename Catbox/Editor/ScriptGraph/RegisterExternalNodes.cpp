#include "stdafx.h"
#include "RegisterExternalNodes.h"

void MVNode_TestNode::Init()
{
	SetTitle("Model Viewer Test Node");
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<float>("Value", PinDirection::Input);
}

size_t MVNode_TestNode::DoOperation()
{
	return ExitViaPin("Out");
}
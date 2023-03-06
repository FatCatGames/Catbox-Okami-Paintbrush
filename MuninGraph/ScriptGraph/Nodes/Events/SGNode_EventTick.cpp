#include "MuninGraph.pch.h"
#include "SGNode_EventTick.h"

void SGNode_EventTick::Init()
{
	SetTitle("Tick");
	SetDescription("An event node that fires every frame.");
	SetCategory("Events");

	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<float>("Delta Time", PinDirection::Output);
}

ImVec4 SGNode_EventTick::GetNodeHeaderColor() const
{
	return ImVec4(255, 0, 0, 128);
}

size_t SGNode_EventTick::DoOperation()
{
	// Value of Delta Time is set by the ExecWithPayload call.
	return ExitViaPin("Out");
}

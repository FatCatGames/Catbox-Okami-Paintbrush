#include "MuninGraph.pch.h"
#include "SGNode_EventBeginPlay.h"

void SGNode_EventBeginPlay::Init()
{
	SetTitle("Begin Play");
	SetDescription("An event node that fires when the object that owns it spawns.");
	SetCategory("Events");

	CreateExecPin("Out", PinDirection::Output, true);
}

ImVec4 SGNode_EventBeginPlay::GetNodeHeaderColor() const
{
	return ImVec4(255, 0, 0, 128);
}

size_t SGNode_EventBeginPlay::DoOperation()
{
	return ExitViaPin("Out");
}

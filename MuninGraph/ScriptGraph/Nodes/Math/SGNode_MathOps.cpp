#include "MuninGraph.pch.h"
#include "SGNode_MathOps.h"

void SGNode_MathAdd::Init()
{
	SetTitle("Add");
	SetDescription("Calculates the addition between two float values.");
	SetCategory("Math");

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathAdd::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA + inB;
		SetPinData("Result", result);
		return ExitViaPin("Out");
	}

	return 0;
}

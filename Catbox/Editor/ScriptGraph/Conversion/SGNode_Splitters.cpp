#include "stdafx.h"
#include "SGNode_Splitters.h"

void SGNode_Vec3fSplitter::Init()
{
	SetTitle("Vector3f Splitter");
	SetCategory("Casts");
	CreateDataPin<Vector3f>("Value", PinDirection::Input);
	CreateDataPin<float>("X", PinDirection::Output);
	CreateDataPin<float>("Y", PinDirection::Output);
	CreateDataPin<float>("Z", PinDirection::Output);
}

size_t SGNode_Vec3fSplitter::DoOperation()
{
	Vector3f val;
	GetPinData("Value", val);
	SetPinData("X", val.x);
	SetPinData("Y", val.y);
	SetPinData("Z", val.z);
	return Exit();
}
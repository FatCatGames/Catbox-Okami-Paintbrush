#include "stdafx.h"
#include "SGNode_Constructors.h"

void SGNode_Vec3fConstructor::Init()
{
	SetTitle("Vector3f Constructor");
	SetCategory("Casts");
	CreateDataPin<float>("X", PinDirection::Input);
	CreateDataPin<float>("Y", PinDirection::Input);
	CreateDataPin<float>("Z", PinDirection::Input);
	CreateDataPin<Vector3f>("Vec3f", PinDirection::Output);
}

size_t SGNode_Vec3fConstructor::DoOperation()
{
	Vector3f val;
	GetPinData("X", val.x);
	GetPinData("Y", val.y);
	GetPinData("Z", val.z);
	SetPinData("Vec3f", val);
	return Exit();
}
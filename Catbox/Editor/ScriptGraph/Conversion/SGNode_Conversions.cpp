#include "stdafx.h"
#include "SGNode_Conversions.h"
static std::string theString;


void SGNode_FloatToString::Init()
{
	SetTitle("Float to String");
	SetCategory("Casts");
	CreateDataPin<float>("Value", PinDirection::Input);
	CreateDataPin<std::string>("Text", PinDirection::Output);
}

size_t SGNode_FloatToString::DoOperation()
{
	float val = 0;
	GetPinData("Value", val);
	SetPinData("Text", std::to_string(val));
	return Exit();
}



void SGNode_Vector3fToString::Init()
{
	SetTitle("Vector3f to String");
	SetCategory("Casts");
	CreateDataPin<Vector3f>("Value", PinDirection::Input);
	CreateDataPin<std::string>("Text", PinDirection::Output);
}

size_t SGNode_Vector3fToString::DoOperation()
{
	Vector3f val;
	GetPinData("Value", val);
	theString = val.ToString();
	SetPinData("Text", theString);
	return Exit();
}
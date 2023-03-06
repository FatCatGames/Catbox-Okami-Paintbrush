#include "stdafx.h"
#include "SGNode_GetOpposite.h"

void SGNode_GetOpposite::Init()
{
	SetTitle("Get Opposite");
	SetCategory("Utility");

	CreateDataPin<bool>("Value", PinDirection::Input, true);
	CreateDataPin<bool>("Return Value", PinDirection::Output, true);
}

size_t SGNode_GetOpposite::DoOperation()
{
	bool b;
	GetPinData("Value", b);
	SetPinData("Return Value", !b);
	return 0;
}

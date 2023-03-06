#include "stdafx.h"
#include "SGNode_GetMousePos.h"

void SGNode_GetMousePos::Init()
{
	SetTitle("Get Mouse Position");
	SetCategory("Input");

	CreateDataPin<float>("X", PinDirection::Output);
	CreateDataPin<float>("Y", PinDirection::Output);
}

size_t SGNode_GetMousePos::DoOperation()
{
	Vector2i screenPos = Engine::GetInstance()->ViewportToScreenPos(Input::GetMousePosition());
	float percentX = screenPos.x / static_cast<float>(DX11::GetResolution().x);
	float percentY = screenPos.y / static_cast<float>(DX11::GetResolution().y);

	SetPinData("X", percentX);
	SetPinData("Y", percentY);
	return Exit();
}

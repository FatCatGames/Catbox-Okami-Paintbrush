#include "stdafx.h"
#include "SGNode_MouseToWorld.h"

void SGNode_MouseToWorld::Init()
{
	SetTitle("Mouse to World");
	SetCategory("Input");

	//CreateExecPin("In", PinDirection::Input, true);
	//CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<float>("Y-Pos", PinDirection::Input);
	CreateDataPin<Vector3f>("World Pos", PinDirection::Output);
}

size_t SGNode_MouseToWorld::DoOperation()
{
	float y = 0;
	GetPinData("Y-Position", y);
	Vector2i screenPos = Engine::GetInstance()->ViewportToScreenPos(Input::GetMousePosition());
	Vector3f worldPos = GraphicsEngine::GetInstance()->GetMainCamera()->MouseToWorldPos(screenPos, y);
	SetPinData("World Pos", worldPos);

	return Exit();
}

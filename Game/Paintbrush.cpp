#include "Game.pch.h"
#include "Paintbrush.h"
#include "Canvas.h"

void Paintbrush::Update()
{
	auto mousePos = Input::GetMousePosition();
	auto screenPos = Engine::GetInstance()->ViewportToScreenPos(mousePos.x, mousePos.y);
	myTransform->SetWorldPos(Engine::GetInstance()->GetActiveCamera()->MouseToWorldPos(screenPos, 0.3f));

	//if (Input::GetKeyHeld(KeyCode::MOUSELEFT))
	{
		Canvas::GetInstance()->Paint(screenPos.x, screenPos.y, Input::GetKeyHeld(KeyCode::MOUSELEFT));
	}

	if (Input::GetKeyReleased(KeyCode::CTRL))
	{
		Canvas::GetInstance()->Clear();
	}
}

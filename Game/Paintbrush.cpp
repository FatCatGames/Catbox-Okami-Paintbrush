#include "Game.pch.h"
#include "Paintbrush.h"

void Paintbrush::Update()
{
	auto mousePos = Input::GetMousePosition();
	myTransform->SetWorldPos(Engine::GetInstance()->GetActiveCamera()->MouseToWorldPos(Engine::GetInstance()->ViewportToScreenPos(mousePos.x, mousePos.y), 0.3f));
}

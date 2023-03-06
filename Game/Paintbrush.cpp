#include "Game.pch.h"
#include "Paintbrush.h"

void Paintbrush::Update()
{
	Engine::GetInstance()->GetActiveCamera()->MouseToWorldPos(Input::GetMousePosition(), 0.5f);
}

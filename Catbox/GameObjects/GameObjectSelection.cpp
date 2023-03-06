#include "stdafx.h"
#include "GameObjectSelection.h"
#include "CommonUtilities/InputHandler.cpp"
#include "Graphics\Rendering\DX11\DX11.h"

GameObject* GameObjectSelection::GetHoveredObject()
{
	int x = Input::GetMousePosition().x;
	int y = Input::GetMousePosition().y;
	Vector2i pos = Engine::GetInstance()->ViewportToScreenPos(x, y);
	int id = DX11::GetScreenObjectId(static_cast<UINT>(pos.x), static_cast<UINT>(pos.y));
	//print("%f", x);
	if (id == 0) return nullptr;
	return Engine::GetGameObject(id);
}

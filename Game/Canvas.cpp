#include "Game.pch.h"
#include "Canvas.h"
#include "CanvasPS.h"
#include "Components\3D\ModelInstance.h"
#include "Assets\Material.h"

Canvas* Canvas::Instance;

Canvas::Canvas()
{
	if (!Instance) Instance = this;
	else printerror("2 canvases in scene");
}

void Canvas::Awake()
{
	myShader = dynamic_cast<CanvasPS*>(myGameObject->GetComponent<ModelInstance>()->GetMaterial(0)->GetPixelShader().get());
}

void Canvas::Paint(int anXPos, int anYPos, int aRadius, const Color& aColor)
{
	myShader->Paint(anXPos, anYPos, aRadius, aColor);
}

void Canvas::Clear()
{
	myShader->Clear();
}

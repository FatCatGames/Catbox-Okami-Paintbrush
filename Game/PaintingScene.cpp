#include "Game.pch.h"
#include "PaintingScene.h"


PaintingScene* PaintingScene::Instance;

PaintingScene::PaintingScene()
{
	Instance = this;
}

#include "Game.pch.h"
#include "GameScene.h"

GameScene* GameScene::Instance;

GameScene::GameScene()
{
	Instance = this;
}

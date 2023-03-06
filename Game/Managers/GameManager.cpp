#include "Game.pch.h"
#include "GameManager.h"

GameManager* GameManager::Instance;

GameManager::GameManager() 
{
	Instance = this;
}

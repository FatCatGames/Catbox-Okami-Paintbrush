#pragma once

class GameManager;
class Project
{
public:
	static void Setup();
	static void Update();
	static void Start();
	static std::shared_ptr<GameManager> gameManager;
};
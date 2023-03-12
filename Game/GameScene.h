#pragma once

class GameScene : public Component
{
public:
	static GameScene* GetInstance() { return Instance; }
	GameScene();

private:
	static GameScene* Instance;

};
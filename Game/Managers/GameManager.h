#pragma once


class GameManager 
{
public:
	static inline GameManager* GetInstance() { return Instance; }
	GameManager();
	~GameManager() = default;

private:
	static GameManager* Instance;
};
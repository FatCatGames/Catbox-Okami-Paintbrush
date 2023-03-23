#pragma once

enum class CollisionLayer : unsigned int
{
	Default = 0,
	Player = 1,
	Interact = 2,
	Tree = 3,
	Bomb = 4
};

class GameManager 
{
public:
	static inline GameManager* GetInstance() { return Instance; }
	GameManager();
	~GameManager() = default;
	Transform* GetPlayer() { return myPlayer; }
	void SetPlayer(Transform* aPlayer) { myPlayer = aPlayer; }

private:
	static GameManager* Instance;
	Transform* myPlayer;
};
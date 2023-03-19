#pragma once


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
#pragma once

#include "Game.h"
#include "Timer.h"
#include "..\Catbox\CommonUtilities\Vector.h"
#include "NetworkGameCommands.h"
#include <unordered_map>
struct object 
{
	Vector3f pos;
	Vector3f dir = Vector3f::right();
};


class Quake : public Game
{
public:
	void Start(std::vector<unsigned short> aPlayerList) override;
	void Update() override;
	void ProcessMessage(GameMessage* aGameMessage) override;
private:
	Catbox::Timer myTimer;
	std::unordered_map<unsigned short, object> myObjects;
	std::vector<Vector3f> mySpawnPositions = {Vector3f(-5.5f,0.0f, 44.5f), Vector3f(-5.5f,0.0f, -25.5f) };
	float myStartTimer = 2.0f;
	float mySpawnCooldown = 1;
	float myCurrentCooldown = 1;
	float myStepTimer = 1.0f / 60.0f;
};


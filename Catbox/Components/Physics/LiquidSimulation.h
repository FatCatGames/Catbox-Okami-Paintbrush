#pragma once
#include "GameObjects/Component.h"
#include "Physics/PhysicsEngine.h"
class LiquidSimulation : public Component
{
public:
	void Awake() override;
	void Update() override;

private:
	physx::PxPBDParticleSystem* myParticleSystem = NULL;
};


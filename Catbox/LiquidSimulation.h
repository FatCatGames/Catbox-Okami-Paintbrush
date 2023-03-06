#pragma once
#include "Physics/PhysicsEngine.h"
#include "GameObjects/Component.h"
class LiquidSimulation : public Component
{
public:

private:
	physx::PxPBDParticleSystem* myParticleSystem = NULL;
	int myNumOfParticles = 0;

	//particle system numbers
	float myParticleSpacing = 0.0f;
	float myFluidDensity = 1000.0f;
	float myRestOffset = 0.0f;
	float mySolidRestOffset = 0.0f;
	float myFluidRestOffset = 0.0f;
	float myRenderRadius = 0.0f;
	float myParticleMass = 0.0f;
};


#include "stdafx.h"
#include "LiquidSimulation.h"

void LiquidSimulation::Awake()
{
	myParticleSystem = Engine::GetInstance()->GetPhysicsEngine()->CreateParticleSystem();
	Engine::GetInstance()->GetPhysicsEngine()->AddParticleSystem(myParticleSystem, myGameObject->GetObjectInstanceID());
	myParticleSystem->setGridSizeX(10);
	myParticleSystem->setGridSizeY(10);
	myParticleSystem->setGridSizeZ(10);
}

void LiquidSimulation::Update()
{

}

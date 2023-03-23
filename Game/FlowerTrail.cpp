#include "Game.pch.h"
#include "FlowerTrail.h"

void FlowerTrail::Awake()
{
	mySizeCurve.AddCoord({ 0.f, 0.f });
	mySizeCurve.AddCoord({ 0.3f, 1.f });
	mySizeCurve.AddCoord({ 0.7f, 1.f });
	mySizeCurve.AddCoord({ 1.f, 0.f });

	mySizeMultiplier = Catbox::GetRandom(0.7f, 1.f);
}

void FlowerTrail::Update()
{
	if (!myHasAppliedOffset)
	{
		myTransform->Translate(Vector3f::right() * Catbox::GetRandom(-0.5f, 0.5f));
		myTransform->Translate(Vector3f::forward() * Catbox::GetRandom(-0.5f, 0.5f));
		myHasAppliedOffset = true;
	}
	myCurrentLifeTime += deltaTime;
	myTransform->SetWorldScale(Vector3f::one() * mySizeCurve.Evaluate(myCurrentLifeTime / myTargetLifetime) * mySizeMultiplier);

	if(myCurrentLifeTime > myTargetLifetime)
	{
		myGameObject->Destroy();
	}
}
#include "Game.pch.h"
#include "FlowerTrail.h"

void FlowerTrail::Awake()
{
	myTransform->GetChildren()[0]->SetLocalScale(myTransform->GetChildren()[0]->localScale() * Catbox::GetRandom(0.7f, 1.f));
}

void FlowerTrail::Update()
{
	if (!myHasAppliedOffset)
	{
		myTransform->Translate(Vector3f::right() * Catbox::GetRandom(-0.5f, 0.5f));
		myTransform->Translate(Vector3f::forward() * Catbox::GetRandom(-0.5f, 0.5f));
		myHasAppliedOffset = true;
	}
}

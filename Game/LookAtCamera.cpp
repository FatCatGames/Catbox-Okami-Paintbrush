#include "Game.pch.h"
#include "LookAtCamera.h"

void LookAtCamera::Update()
{
	myTransform->LookAt(Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetTransform()->worldPos());
}

void LookAtCamera::RunInEditor()
{
	Update();
}

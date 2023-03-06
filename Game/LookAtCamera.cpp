#include "Game.pch.h"
#include "LookAtCamera.h"

void LookAtCamera::Update()
{
	myTransform->LookAtHorizontal(GraphicsEngine::GetInstance()->GetMainCamera()->GetTransform()->worldPos());
}

void LookAtCamera::RunInEditor()
{
	myTransform->LookAtHorizontal(Editor::GetInstance()->GetEditorCamera().GetTransform().worldPos());
}

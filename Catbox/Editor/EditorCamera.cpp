#include "stdafx.h"
#include "EditorCamera.h"
#include "CommonUtilities/InputHandler.h"
#include "Components/3D/Camera.h"
#include "GameObjects/Transform.h"
#include <UtilityFunctions.hpp>
#include "Graphics\Rendering\DX11\DX11.h"
#include "Windows\PropertiesWindow.h"
#include "Windows\SceneHierarchyWindow.h"
#include <ImGuizmo.h>
#include "Windows/SceneWindow.h"

EditorCamera::EditorCamera()
{
	myCamera.OverrideTransform(myTransform);
	myTransform.SetWorldPos({ 0, 0.7f, 1.5f });
	myTransform.SetLocalRot({ 20, 180, 0 });
	myCamera.myPostProcessingVolume = std::make_shared<PostProcessingVolume>();
	Engine::GetInstance()->GetGraphicsEngine()->SetCamera(&myCamera);

	if (Engine::GetInstance()->GetMainCamera() == &myCamera || Engine::GetInstance()->GetMainCamera() == &myUICamera)
	{
		Engine::GetInstance()->SetMainCamera(nullptr);
	}

	myUICamera.OverrideTransform(myUICameraTransform);
	myUICameraTransform.SetLocalPos(0, 0, -1);
}

void EditorCamera::ScrollZoom()
{
	myCamera.GetTransform()->Translate(myCamera.GetTransform()->forward() * Input::GetMouseScroll() * myScrollSpeed * deltaTime);
}

void EditorCamera::Movement()
{
	if (Input::GetKeyHeld(KeyCode::MOUSERIGHT) && !ImGui::GetIO().WantCaptureKeyboard)
	{
		float forward = 0;
		float right = 0;
		float up = 0;
		forward += (Input::GetKeyHeld(KeyCode::W));
		forward -= (Input::GetKeyHeld(KeyCode::S));
		right += (Input::GetKeyHeld(KeyCode::D));
		right -= (Input::GetKeyHeld(KeyCode::A));
		up -= (Input::GetKeyHeld(KeyCode::Q));
		up += (Input::GetKeyHeld(KeyCode::E));

		if (forward != 0 || right != 0 || up != 0) 
		{
			Vector3f rightVector = myCamera.GetTransform()->right() * right * myMoveSpeed * deltaTime;
			Vector3f forwardVector = myCamera.GetTransform()->forward() * forward * myMoveSpeed * deltaTime;
			Vector3f upVector = Vector3f::up() * up * myMoveSpeed * deltaTime;
			myCamera.GetTransform()->Translate(rightVector + forwardVector + upVector);
		}
	}
}

void EditorCamera::Rotate()
{
	if (Input::GetKeyHeld(KeyCode::MOUSERIGHT) && Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneWindow>(WindowType::Scene)->IsHovered())
	{
		Vector2i delta = Input::GetMouseDelta();
		if (delta.x != 0 || delta.y != 0) 
		{
			myCamera.OnTransformChanged();
			Vector3f rot = myTransform.localRot();
			rot.x += delta.y * myRotateSpeed * deltaTime;
			rot.y += delta.x * myRotateSpeed * deltaTime;
			rot.x = Catbox::Clamp(rot.x, -90.f, 90.f);
			rot.z = 0;
			myTransform.SetLocalRot(rot);
		}
	}
}


Transform& EditorCamera::GetTransform()
{
	return myTransform;
}

#include "stdafx.h"
#include "Gizmos.h"
#include <ImGuizmo.h>
#include "EditorCamera.h"
#include "3D/Camera.h"
#include "Editor\Windows\SceneWindow.h"
#include "Editor\EditorCamera.h"
#include "CommonUtilities/InputHandler.h"
#include "Editor/Commands/EditorCommandHandler.h"
#include "Editor/Commands/TransformCommand.h"

static float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f };

bool drawTransformGizmos = true;
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
static Transform::Mode currentMode(Transform::Mode::Local);
static Transform::Operation currentOperation(Transform::Operation::Translate);
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
static Vector3f startValue;
static Vector3f valueLastFrame;
static Vector3f endValue;
static bool useSnap = false;
static bool snapToggle = false;
static float* moveSnap;
static float* rotateSnap;
static float* scaleSnap;
static float* snap;

void Gizmos::Update(Camera* aCamera)
{
	ImGuizmo::SetOrthographic(false);

	auto viewMatrix = Catbox::Matrix4x4<float>::GetFastInverse(aCamera->GetTransform()->GetWorldTransformMatrix());
	myView = Catbox::Matrix4x4<float>::ToArray(viewMatrix);
	auto& projectionMatrix = aCamera->GetProjectionMatrix();
	myProjection = Catbox::Matrix4x4<float>::ToArray(projectionMatrix);

	if (!Input::GetKeyHeld(KeyCode::MOUSERIGHT) && !ImGui::GetIO().WantCaptureKeyboard)
	{
		bool switchToMove = Input::GetKeyPress(KeyCode::W);
		bool switchToRotate = Input::GetKeyPress(KeyCode::E);
		bool switchToScale = Input::GetKeyPress(KeyCode::R);

		if (Input::GetKeyPress(KeyCode::SPACE))
		{
			switchToRotate |= (currentOperation == Transform::Operation::Translate);
			switchToScale |= (currentOperation == Transform::Operation::Rotate);
			switchToMove |= (currentOperation == Transform::Operation::Scale);
		}

		if (switchToMove || switchToRotate || switchToScale)
		{
			if (!Editor::GetInstance()->IsPrefabBrushMode() || !Input::GetKeyHeld(KeyCode::SHIFT)) 
			{
				drawTransformGizmos = true;
				Editor::GetInstance()->SetPrefabBrushMode(false);

				if (switchToMove)
				{
					mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
					currentOperation = Transform::Operation::Translate;
					snap = moveSnap;
				}
				else if (switchToRotate)
				{
					mCurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
					currentOperation = Transform::Operation::Rotate;
					snap = rotateSnap;
				}
				else if (switchToScale)
				{
					mCurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
					currentOperation = Transform::Operation::Scale;
					snap = scaleSnap;
				}
			}
		}
		else
		{
			if (Input::GetKeyPress(KeyCode::Q))
			{
				drawTransformGizmos = false;
			}
			else if (Input::GetKeyPress(KeyCode::T))
			{
				drawTransformGizmos = false;
				Editor::GetInstance()->SetPrefabBrushMode(true);
			}
		}
	}

	if (Input::GetKeyPress(KeyCode::TAB))
	{
		if (mCurrentGizmoMode == ImGuizmo::LOCAL)
		{
			mCurrentGizmoMode = ImGuizmo::WORLD;
			currentMode = Transform::Mode::World;
		}
		else
		{
			mCurrentGizmoMode = ImGuizmo::LOCAL;
			currentMode = Transform::Mode::Local;
		}
	}

	useSnap = Input::GetKeyHeld(KeyCode::SHIFT);
	if (snapToggle) useSnap = !useSnap;
}


void Gizmos::DrawGrid(SceneWindow* /*aViewport*/)
{
	ImGuizmo::DrawGrid(&myView[0], &myProjection[0], identityMatrix, 100.f);
}


void Gizmos::DrawTransformGizmos(SceneWindow* /*aViewport*/)
{
	if (!drawTransformGizmos) return;

	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	Catbox::Matrix4x4<float> sharedMatrix;
	Vector3f averagePosition;
	Vector3f averageScale;
	Vector3f averageRotation;

	for (size_t i = 0; i < mySelectedObjects.size(); i++)
	{
		averagePosition += mySelectedObjects[i]->worldPos();
		averageScale += mySelectedObjects[i]->worldScale();
		averageRotation += mySelectedObjects[i]->worldRot();
	}
	averagePosition /= static_cast<float>(mySelectedObjects.size());
	averageScale /= static_cast<float>(mySelectedObjects.size());
	averageRotation /= static_cast<float>(mySelectedObjects.size());

	Catbox::Matrix4x4<float> rotationMatrix;
	rotationMatrix = rotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(averageRotation.z)) * rotationMatrix;
	rotationMatrix = rotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(averageRotation.y)) * rotationMatrix;
	rotationMatrix = rotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(averageRotation.x)) * rotationMatrix;
	if (currentOperation == Transform::Operation::Scale)
	{
		sharedMatrix = sharedMatrix * Vector4f(averageScale.x, averageScale.y, averageScale.z, 1);
	}
	sharedMatrix = sharedMatrix * rotationMatrix;
	sharedMatrix(4, 1) = averagePosition.x;
	sharedMatrix(4, 2) = averagePosition.y;
	sharedMatrix(4, 3) = averagePosition.z;


	auto objectSpace = Catbox::Matrix4x4<float>::ToArray(sharedMatrix);


	if (ImGuizmo::Manipulate(&myView[0], &myProjection[0], mCurrentGizmoOperation, mCurrentGizmoMode, &objectSpace[0], NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL))
	{
		Catbox::Matrix4x4<float> matrixResult(objectSpace);

		if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
		{
			if (!myEditLastFrame)
			{
				startValue = averagePosition;
				endValue = startValue;
				valueLastFrame = startValue;
			}
			Vector3f translation = Catbox::Matrix4x4<float>::GetTranslationFromMatrix(matrixResult) - averagePosition;
			translation.x = Catbox::Clamp(translation.x, -10.f, 10.f);
			translation.y = Catbox::Clamp(translation.y, -10.f, 10.f);
			translation.z = Catbox::Clamp(translation.z, -10.f, 10.f);
			endValue += translation;
			for (auto& object : mySelectedObjects)
			{
				object->Translate(translation);
			}
		}
		else if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
		{
			if (!myEditLastFrame)
			{
				startValue = averageRotation;
				valueLastFrame = averageRotation;
			}
			endValue = Catbox::Matrix4x4<float>::GetRotationFromMatrix(matrixResult);
			std::cout << (endValue).y << std::endl;
			for (auto& object : mySelectedObjects)
			{
				object->Rotate(endValue - valueLastFrame);
			}
			valueLastFrame = endValue;
		}
		else //SCALE
		{
			if (!myEditLastFrame)
			{
				startValue = Catbox::Matrix4x4<float>::GetScaleFromMatrix(matrixResult);
				endValue = startValue;
				valueLastFrame = Vector3f::zero();
			}
			Vector3f newScale = Catbox::Matrix4x4<float>::GetScaleFromMatrix(matrixResult) - startValue;
			Vector3f translation = newScale - valueLastFrame;
			translation.x = Catbox::Clamp(translation.x, -10.f, 10.f);
			translation.y = Catbox::Clamp(translation.y, -10.f, 10.f);
			translation.z = Catbox::Clamp(translation.z, -10.f, 10.f);
			for (auto& object : mySelectedObjects)
			{
				object->SetWorldScale(object->worldScale() + translation);
			}
			valueLastFrame = newScale;
			endValue += translation;
		}

		myEditLastFrame = true;
	}
	else if (!ImGuizmo::IsUsing() && myEditLastFrame)
	{
		EditorCommandHandler::AddCommand(std::make_shared<TransformCommand>(mySelectedObjects, currentOperation, currentMode, endValue - startValue));
		myEditLastFrame = false;
		printmsg(std::to_string(endValue.x - startValue.x));
	}

	for (size_t i = 0; i < mySelectedObjects.size(); i++)
	{
		if (mySelectedObjects[i]->IsParentedToBone())
		{
			DebugDrawer::DrawSphere(mySelectedObjects[i]->GetParent()->worldPos(), 0.5f, Color::Orange());
		}
	}
	//float size = aViewport->GetSize().x;
	//ImGuizmo::ViewManipulate(&myView[0], 8, ImVec2((aViewport->GetPosition().x + size) - 128, aViewport->GetPosition().y), ImVec2(size/11.f, size/11.f), 0x10101010);

}

void Gizmos::RenderGizmos(SceneWindow* aViewport)
{
	DrawTransformGizmos(aViewport);

	//ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);
		/*if (ImGuizmo::IsUsing())
		{

		}*/

}

void Gizmos::ClearSelectedObjects()
{
	mySelectedObjects.clear();
}

void Gizmos::AddSelectedObject(GameObject* anObject)
{
	mySelectedObjects.push_back(anObject->GetTransform());
}


void Gizmos::RemoveSelectedObject(GameObject* anObject)
{
	Transform* objectTransform = anObject->GetTransform();
	for (size_t i = 0; i < mySelectedObjects.size(); i++)
	{
		if (mySelectedObjects[i] == objectTransform)
		{
			mySelectedObjects.erase(mySelectedObjects.begin() + i);
		}
	}
}

void Gizmos::SetMoveSnap(float settings[3])
{
	moveSnap = settings;
	if (currentOperation == Transform::Operation::Translate) snap = moveSnap;
}

void Gizmos::SetRotateSnap(float settings[3])
{
	rotateSnap = settings;
	if (currentOperation == Transform::Operation::Rotate) snap = rotateSnap;
}

void Gizmos::SetScaleSnap(float settings[3])
{
	scaleSnap = settings;
	if (currentOperation == Transform::Operation::Scale) snap = scaleSnap;
}

void Gizmos::SetSnapToggle(bool aSnapToggle)
{
	snapToggle = aSnapToggle;
}

bool Gizmos::IsUsing()
{
	return ImGuizmo::IsUsing();
}

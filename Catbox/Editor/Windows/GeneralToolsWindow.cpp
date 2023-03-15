#include "stdafx.h"
#include "GeneralToolsWindow.h"
#include "Editor\Windows\SceneHierarchyWindow.h"
#include "Editor\Windows\PropertiesWindow.h"
#include "Editor\Commands\CreateDeleteObjectCommand.h"
#include "Editor\Commands\EditorCommandHandler.cpp"
#include "3D\ModelInstance.h"
#include "Assets\Material.h"
#include "Editor\Windows\SceneWindow.h"

GeneralToolsWindow::GeneralToolsWindow()
{
	myPreview = new GameObject();
	mySelectedPrefabModel = myPreview->AddComponent<ModelInstance>();
	mySelectedPrefabModel->SetRenderToUse(Renderer::Forward);
}

GeneralToolsWindow::~GeneralToolsWindow()
{
	delete myPreview;
}

void GeneralToolsWindow::Render()
{
	if (!BeginWindow("General Tools", true)) return;

	bool edit;
	auto newPrefab = AssetRegistry::GetInstance()->AssetDropdownMenu<Prefab>(mySelectedPrefab.get(), "Prefab", edit);
	if (edit)
	{
		mySelectedPrefab = newPrefab;
		if (mySelectedPrefab)
		{
			GameObject temp;
			temp.LoadFromPath(mySelectedPrefab->GetPath().string().c_str());
			auto m = temp.GetComponent<ModelInstance>();
			std::shared_ptr<Model> model = m ? m->GetModel() : nullptr;
			mySelectedPrefabModel->SetModel(model);
			if (mySelectedPrefabModel->GetMaterial(0))
			{
				std::shared_ptr<Material> mat = m ? m->GetMaterial(0) : nullptr;
				auto& materials = mySelectedPrefabModel->GetMaterials();
				for (size_t i = 0; i < materials.size(); i++)
				{
					if (!materials[i]) continue;
					mySelectedPrefabModel->SetMaterial(materials[i], i, true);
					auto col = mySelectedPrefabModel->GetMaterial(i)->GetColor();
					col.a -= 0.1f;
					mySelectedPrefabModel->GetMaterial(i)->SetColor(col);
					std::shared_ptr<Texture> tex = mat ? mat->GetAlbedoTexture() : nullptr;
					mySelectedPrefabModel->GetMaterial(0)->SetAlbedoTexture(tex);
				}
			}
		}
		else mySelectedPrefabModel->SetModel(nullptr);
	}

	ImGui::Spacing();

	bool brushModeActive = Editor::GetInstance()->IsPrefabBrushMode();
	if (ImGui::Checkbox("Prefab Brush mode", &brushModeActive))
	{
		Editor::GetInstance()->SetPrefabBrushMode(brushModeActive);
	}

	auto selectedVec = Editor::GetInstance()->GetSelectedObjects();
	GameObject* selected = nullptr;
	if (!selectedVec.empty()) selected = selectedVec.front();

	if (brushModeActive)
	{
		auto& editorCam = Editor::GetInstance()->GetEditorCamera();
		Vector3f pos = editorCam.GetCamera()->MouseToWorldPos(Input::GetMousePosition());
		pos.y = myYPos;

		mySelectedPrefabModel->Render();
		DebugDrawer::DrawSphere(pos, 0.1f, Color::Green());

		bool placePrefab = false;
		if (Input::GetKeyHeld(KeyCode::SHIFT))
		{
			placePrefab = Input::GetKeyPress(KeyCode::E);
			Vector2i cursorDelta = Input::GetMouseDelta();

			if (Input::GetKeyHeld(KeyCode::MOUSEMIDDLE))
			{
				float diffX = cursorDelta.x * 0.02f;
				float diffY = -cursorDelta.y * 0.03f;
				Vector3f moveRight = editorCam.GetTransform().right() * diffX;
				Vector3f moveForward = editorCam.GetTransform().forward() * diffY;
				moveRight.y = 0;
				moveForward.y = 0;
				myPreview->GetTransform()->Translate(moveRight + moveForward);
			}
			else
			{
				float scroll = Input::GetMouseScroll() * deltaTime;
				float multiplier = (editorCam.GetTransform().worldPos() - pos).Length();
				if (scroll != 0)
				{
					myYPos += scroll * multiplier; //Move brush up/down
					Vector3f worldPos = myPreview->GetTransform()->worldPos();
					worldPos.y = myYPos;
					myPreview->GetTransform()->SetWorldPos(worldPos);
				}

				if (Input::GetKeyHeld(KeyCode::MOUSELEFT))
				{
					float diff = cursorDelta.x * 0.01f;
					myPreview->GetTransform()->SetWorldScale(myPreview->GetTransform()->worldScale() + Vector3f::one() * diff);
				}
				else if (Input::GetKeyHeld(KeyCode::MOUSERIGHT))
				{
					float diff = cursorDelta.x;
					myPreview->GetTransform()->Rotate(Vector3f::up() * -diff);
				}
			}
		}
		else
		{
			std::vector<SceneWindow*> sceneWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<SceneWindow>(WindowType::Scene);
			if (sceneWindows.size() == 0) return;
			bool sceneWindowHovered = false;
			for (auto& sceneWindow : sceneWindows)
			{
				if (sceneWindow->IsHovered())
				{
					sceneWindowHovered = true;
					break;
				}
			}

			if (sceneWindowHovered)
			{
				editorCam.ScrollZoom();
				editorCam.Rotate();
			}

			myPreview->GetTransform()->SetWorldPos(pos);

			if (Input::GetKeyPress(KeyCode::MOUSELEFT) && mySelectedPrefab)
			{
				placePrefab = true;
			}
		}

		if (placePrefab)
		{
			selected = InstantiatePrefab(mySelectedPrefab);
			selected->Select();
			Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
			Editor::GetInstance()->GetGizmos().AddSelectedObject(selected);

			selected->GetTransform()->SetWorldPos(myPreview->GetTransform()->worldPos());
			selected->GetTransform()->SetWorldRot(myPreview->GetTransform()->worldRot());
			selected->GetTransform()->SetWorldScale(myPreview->GetTransform()->worldScale());
			EditorCommandHandler::AddCommand(std::make_shared<CreateDeleteObjectCommand>(selected, CreateDeleteObjectCommand::Operation::Create));
		}
	}


	if (mySelectedPrefab)
	{
		if (ImGui::Button(("Replace selected objects with " + mySelectedPrefab->GetName()).c_str()))
		{
			if (auto hierarchy = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy))
			{
				auto& gizmos = Editor::GetInstance()->GetGizmos();
				auto selected = hierarchy->GetSelectedObjects();

				hierarchy->ClearSelectedObjects();
				hierarchy->Reset();
				gizmos.ClearSelectedObjects();
				if (auto properties = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<PropertiesWindow>(WindowType::Properties))
				{
					properties->DeselectAll();
				}

				for (auto& obj : selected)
				{
					auto newObj = InstantiatePrefab(mySelectedPrefab);
					auto tOld = obj->GetTransform();
					auto tNew = newObj->GetTransform();
					tNew->SetWorldPos(tOld->worldPos());
					tNew->SetWorldRot(tOld->worldRot());
					tNew->SetWorldScale(tOld->worldScale());
					tNew->SetParent(tOld->GetParent());
					newObj->SetName(obj->GetName().c_str());

					EditorCommandHandler::AddCommand(std::make_shared<CreateDeleteObjectCommand>(obj, CreateDeleteObjectCommand::Operation::Delete));
					EditorCommandHandler::AddCommand(std::make_shared<CreateDeleteObjectCommand>(newObj, CreateDeleteObjectCommand::Operation::Create));

					obj->Destroy();
					newObj->AddSelected();
					hierarchy->AddSelectedObject(newObj);
					gizmos.AddSelectedObject(newObj);
				}
			}
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Snapping"))
	{
		if (ImGui::Button("Snap object positions"))
		{
			auto hierarchy = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
			if (hierarchy)
			{
				std::vector<GameObject*> selectedObjects = hierarchy->GetSelectedObjects();
				for (auto& obj : selectedObjects)
				{
					Vector3f worldPos = obj->GetTransform()->worldPos();
					obj->GetTransform()->SetWorldPos(roundf(worldPos.x), roundf(worldPos.y), roundf(worldPos.z));
				}
			}
		}

		if (ImGui::Button("Snap object rotations"))
		{
			auto hierarchy = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
			if (hierarchy)
			{
				std::vector<GameObject*> selectedObjects = hierarchy->GetSelectedObjects();
				for (auto& obj : selectedObjects)
				{
					Vector3f worldRot = obj->GetTransform()->worldRot();
					obj->GetTransform()->SetWorldRot(roundf(worldRot.x), roundf(worldRot.y), roundf(worldRot.z));

					float decimal_part = fmod(worldRot.x, 90.0f);
					int quotient = static_cast<int>(worldRot.x / 90.0f);
					worldRot.x = (quotient + static_cast<int>(decimal_part < 45)) * 90;

					decimal_part = fmod(worldRot.y, 90.0f);
					quotient = static_cast<int>(worldRot.y / 90.0f);
					worldRot.y = (quotient + static_cast<int>(decimal_part < 45)) * 90;

					decimal_part = fmod(worldRot.z, 90.0f);
					quotient = static_cast<int>(worldRot.z / 90.0f);
					worldRot.z = (quotient + static_cast<int>(decimal_part < 45)) * 90;
				}
			}
		}
	}


	if (ImGui::CollapsingHeader("Randomize"))
	{
		ImGui::Spacing();
		ImGui::Checkbox("Randomize size", &myRandomizeSize);
		ImGui::SetNextItemWidth(75);
		ImGui::DragFloat("Min##randsize", &myMinSize, 0.1f, -100, 100, "%.2g");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(75);
		ImGui::DragFloat("Max##randsize", &myMaxSize, 0.1f, -100, 100, "%.2g");

		ImGui::Checkbox("Randomize rotation", &myRandomizeRotation);
		ImGui::SetNextItemWidth(75);
		ImGui::DragFloat("Min##randrot", &myMinRot, 1, -360, 360, "%.3g");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(75);
		ImGui::DragFloat("Max##randrot", &myMaxRot, 1, -360, 360, "%.3g");

		if (ImGui::Button("Randomize selected"))
		{
			auto hierarchy = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
			if (hierarchy)
			{
				std::vector<GameObject*> selectedObjects = hierarchy->GetSelectedObjects();
				for (auto& obj : selectedObjects)
				{
					if (myRandomizeSize) obj->GetTransform()->SetLocalScale(Vector3f::one() * Catbox::GetRandom(myMinSize, myMaxSize));
					if (myRandomizeRotation) obj->GetTransform()->SetLocalRot(Vector3f::up() * Catbox::GetRandom(myMinRot, myMaxRot));
				}
			}
		}
	}

	EndWindow();
}
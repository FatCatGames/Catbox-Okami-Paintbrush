#include "stdafx.h"
#include "Editor.h"
#include "CommonUtilities/InputHandler.h"
#include "CommonUtilities\UtilityFunctions.hpp"

#include "ComponentHandler.h"
#include "Components/Physics/Collisions/SphereCollider.h"
#include "Components/3D/Camera.h"
#include "ComponentTools\SceneManager.h"
#include "Windows\SceneWindow.h"
#include "Physics/Intersection.h"

#include "Assets/AssetLoader.h"

#include "Windows/PropertiesWindow.h"
#include "Windows\SceneHierarchyWindow.h"
#include "Windows\VisualScriptingWindow.h"
#include "Windows/AssetBrowserWindow.h"
#include "Windows/AnimationControllerWindow.h"
#include "Editor/Commands/EditorCommandHandler.h"
#include <Editor/Commands/CreateDeleteObjectCommand.h>
#include "Physics/Collisions/CollisionManager.h"
#include <ImGuizmo.h>
#include "GameObjects\GameObjectSelection.h"
#include "PopupManager.h"
#include "ScriptGraph/RegisterSGNodes.h"
#include "ScriptGraph/BlueprintEditor.h"
#include "ScriptGraph/AnimationController/AnimationControllerEditor.h"

Editor* Editor::Instance = nullptr;

Editor::Editor()
{
	if (Instance == nullptr)
	{
		Instance = this;
		myBlueprintEditor = std::make_shared<BlueprintEditor>();
		myBlueprintEditor->Init();
		myAnimationControllerEditor = std::make_shared<AnimationControllerEditor>();
		myAnimationControllerEditor->Init();
		SGNodes::RegisterNodes();
		myUserPrefs.LoadUserPreferences();
	}
	else assert(true && "Created an editor when it already exists!");
}

void Editor::Setup()
{

}

void Editor::LoadDefaultScene()
{
	auto parser = RapidJsonWrapper::GetInstance();
	if (!parser->DocumentExists("Temp/DefaultScene.json")) return;
	auto& doc = parser->LoadDocument("Temp/DefaultScene.json");
	string scenePath = doc["Last Scene"].GetString();
	if (!scenePath.empty() && parser->DocumentExists(scenePath.c_str()))
	{
		string sceneName = Catbox::GetNameFromPath(scenePath);
		auto scene = AssetRegistry::GetInstance()->GetAsset<Scene>(sceneName);
		if (!scene) scene = CreateAsset<Scene>(scenePath);
		Engine::GetInstance()->GetSceneManager()->LoadScene(scene);
	}
}

void Editor::Update()
{
	bool sceneWindowOpen = false;
	auto hierarchy = myWindowHandler.GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
	std::vector<SceneWindow*> sceneWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<SceneWindow>(WindowType::Scene);
	mySceneWindowHovered = false;
	bool mySceneWindowFocused = false;
	bool myHierarchyFocused = false;
	for (auto& sceneWindow : sceneWindows)
	{
		if (sceneWindow->IsHovered())
		{
			mySceneWindowHovered = true;
		}
		if (sceneWindow->IsFocused())
		{
			mySceneWindowFocused = true;
		}
		if (sceneWindow->IsOpen())
		{
			sceneWindowOpen = true;
		}
	}
	if (hierarchy->IsFocused())
	{
		myHierarchyFocused = true;
	}

	PopupManager::Update();


	if (!PLAYMODE && !ImGui::GetIO().WantCaptureKeyboard)
	{
		//UI edit mode
		if (Input::GetKeyPress(KeyCode::U) && mySceneWindowHovered)
		{
			SetUIEditMode(!myUIEditMode);
		}

		//Focus on object
		if (Input::GetKeyPress(KeyCode::F) && sceneWindowOpen)
		{
			auto hierarchy = myWindowHandler.GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
			if (hierarchy)
			{
				auto& selectedObjects = hierarchy->GetSelectedObjects();
				if (!selectedObjects.empty())
				{
					Vector3f averagePos;
					float largestDistSqr = 6;
					for (size_t i = 0; i < selectedObjects.size(); i++)
					{
						averagePos += selectedObjects[i]->GetTransform()->worldPos();
						for (size_t j = 0; j < selectedObjects.size(); j++)
						{
							if (i == j) continue;
							float distSqr = (selectedObjects[i]->GetTransform()->worldPos() - selectedObjects[j]->GetTransform()->worldPos()).LengthSqr();
							if (distSqr > largestDistSqr) largestDistSqr = distSqr;
						}
					}

					averagePos /= static_cast<float>(selectedObjects.size());
					float distModifier = sqrt(largestDistSqr);
					myEditorCamera.GetTransform().SetWorldPos(averagePos + (myEditorCamera.GetTransform().back() * distModifier));
				}
			}
		}

		//Autosaving
		myTimeSinceSave += deltaTime;
		if (myTimeSinceSave > 300 && EditorCommandHandler::GetCommandCountSinceSave() >= 10)
		{
			struct tm newtime;
			time_t now = time(0);
			localtime_s(&newtime, &now);

			std::string sceneName = myCurrentScene->GetName() + "_" + std::to_string(newtime.tm_mon + 1) + "_" + std::to_string(newtime.tm_mday) + "_" + std::to_string(newtime.tm_hour) + std::to_string(newtime.tm_min);
			PopupManager::CreatePopup("Autosave", 3, { 200,75 }, "Autosaving...", { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 });

			if (!std::filesystem::exists("Temp/Autosaves"))
			{
				CreateDirectoryA("Temp/Autosaves", 0);
			}
			myCurrentScene->SaveAsset(("Temp/Autosaves/" + sceneName + ".scene").c_str());
		}
	}



	//Delete
	if (Input::GetKeyPress(KeyCode::DELETE_BUTTON) && myCanDeleteObjects && !ImGui::GetIO().WantCaptureKeyboard)
	{
		if (mySceneWindowFocused || myHierarchyFocused)
		{
			auto& selectedObjects = hierarchy->GetSelectedObjects();
			for (int i = selectedObjects.size() - 1; i >= 0; i--)
			{
				EditorCommandHandler::AddCommand(std::make_shared<CreateDeleteObjectCommand>(selectedObjects[i], CreateDeleteObjectCommand::Operation::Delete));
				selectedObjects[i]->Destroy();
			}

			Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
			hierarchy->ClearSelectedObjects();
			auto properties = myWindowHandler.GetWindowOfType<PropertiesWindow>(WindowType::Properties);
			if (properties)
			{
				properties->DeselectAll();
			}
		}
	}

	if (Input::GetKeyHeld(KeyCode::CTRL))
	{
		auto visualScriptingWindow = myWindowHandler.GetWindowOfType<VisualScriptingWindow>(WindowType::VisualScripting);
		auto animatorWindow = myWindowHandler.GetWindowOfType<AnimationControllerWindow>(WindowType::AnimationControllerEditor);

		//Undo
		if (Input::GetKeyPress(KeyCode::Z))
		{
			if (Input::GetKeyHeld(KeyCode::SHIFT))
			{
				EditorCommandHandler::Redo();
			}
			else EditorCommandHandler::Undo();
		}

		//Redo
		else if (Input::GetKeyPress(KeyCode::Y))
		{
			EditorCommandHandler::Redo();
		}

		//Duplicate
		if (Input::GetKeyPress(KeyCode::D))
		{
			if (hierarchy && sceneWindowOpen)
			{
				std::vector<GameObject*> selectedObjects = hierarchy->GetSelectedObjects();
				hierarchy->ClearSelectedObjects();
				Editor::GetInstance()->GetGizmos().ClearSelectedObjects();

				for (size_t i = 0; i < selectedObjects.size(); i++)
				{
					GameObject* copy = selectedObjects[i]->Duplicate();
					hierarchy->AddSelectedObject(copy);
					Editor::GetInstance()->GetGizmos().AddSelectedObject(copy);
					if (i == 0) copy->Select();
					else copy->AddSelected();
				}
			}
		}


		//Saving
		if (Input::GetKeyPress(KeyCode::S))
		{
			if (!PLAYMODE)
			{
				if (visualScriptingWindow && visualScriptingWindow->IsFocused() && visualScriptingWindow->IsOpen())
				{
					PopupManager::CreatePopup("SavedBlueprint", 2, { 250,75 }, "Saved Blueprint!", { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 });
					myBlueprintEditor->SaveCurrent();
				}
				else if (animatorWindow && animatorWindow->IsFocused() && animatorWindow->IsOpen())
				{
					PopupManager::CreatePopup("SavedController", 2, { 250,75 }, "Saved Controller!", { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 });
					myAnimationControllerEditor->SaveCurrent();
				}
				else
				{
					PopupManager::CreatePopup("SavedScene", 2, { 200,75 }, "Saved Scene!", { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 });
					myCurrentScene->SaveAsset(myCurrentScene->GetPath().string().c_str());
				}
			}
		}
	}
	else if (!PLAYMODE)
	{
		if (!sceneWindows.empty())
		{
			for (auto& sceneWindow : sceneWindows)
			{
				if (sceneWindow->IsHovered())
				{
					mySceneWindowHovered = true;
					break;
				}
			}

			if (mySceneWindowHovered)
			{
				if (Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera() == myEditorCamera.GetCamera())
				{
					myEditorCamera.Movement();

					if (!myPrefabBrushMode)
					{
						myEditorCamera.ScrollZoom();
						myEditorCamera.Rotate();
						SelectObjectCheck();
					}
				}
				else if (!myPrefabBrushMode)
				{
					SelectObjectCheck();
				}
			}
			else
			{
				if (!myPrefabBrushMode && Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera() == myEditorCamera.GetCamera())
				{
					myEditorCamera.Rotate();
				}
			}
		}
	}
}

void Editor::Render()
{
	myMainMenuBar.Render();
	myWindowHandler.Render();
	PopupManager::Render();
}


void Editor::OpenScene(std::shared_ptr<Scene> aScene)
{
	myCurrentScene = aScene;
}


void Editor::SetUIEditMode(bool aEditMode)
{
	myUIEditMode = aEditMode;
	if (myUIEditMode)
	{
		Engine::GetInstance()->GetGraphicsEngine()->SetCamera(myEditorCamera.GetUICamera());
	}
	else
	{
		Engine::GetInstance()->GetGraphicsEngine()->SetCamera(myEditorCamera.GetCamera());
	}
}

void Editor::SetPrefabBrushMode(bool aEditMode)
{
	myPrefabBrushMode = aEditMode;
}


void Editor::DropFile(std::string aPath)
{
	std::string fileName = Catbox::GetNameFromPath(aPath);
	std::string fileExtension = Catbox::GetExtensionFromPath(aPath);
	std::string targetPath = "Assets\\";
	std::string assetTargetPath = "Assets\\";
	std::string workingDir = std::filesystem::current_path().string();

	AssetBrowserWindow* assetBrowser = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<AssetBrowserWindow>(WindowType::AssetBrowser);
	if (assetBrowser)
	{
		targetPath = workingDir + "\\" + assetBrowser->GetCurrentFolder().string() + "\\" + fileName + fileExtension;
		assetTargetPath = assetBrowser->GetCurrentFolder().string() + "\\" + fileName + fileExtension;
	}

	Catbox::CopyFileK(aPath, targetPath);
	auto newAsset = AssetLoader::LoadAsset(assetTargetPath);
	if (assetBrowser)
	{
		assetBrowser->AddAssetToCurrentFolder(newAsset);
	}
}

void Editor::ClearAllSelectedObjects()
{
	auto hierarchy = myWindowHandler.GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
	auto properties = myWindowHandler.GetAllWindowsOfType<PropertiesWindow>(WindowType::Properties);
	if (hierarchy) hierarchy->ClearSelectedObjects();
	if (!properties.empty())
	{
		for (auto& propertyWindow : properties)
		{
			propertyWindow->DeselectAll();
		}
	}
	myGizmos.ClearSelectedObjects();
}

void Editor::ResetHierarchy()
{
	auto hierarchy = myWindowHandler.GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
	if (hierarchy) hierarchy->Reset();
}

std::vector<GameObject*> Editor::GetSelectedObjects()
{
	SceneHierarchyWindow* hierarchy = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
	if (hierarchy) return hierarchy->GetSelectedObjects();
	return std::vector<GameObject*>();
}

void Editor::SelectObjectCheck()
{
	if (Input::GetKeyPress(KeyCode::MOUSELEFT) && !ImGuizmo::IsOver())
	{
		if (Input::GetKeyHeld(KeyCode::CTRL) && Input::GetKeyHeld(KeyCode::ALT))
		{
			myIsBoxSelecting = true;
			int x = Input::GetMousePosition().x;
			int y = Input::GetMousePosition().y;
			mySelectRectMin = Engine::GetInstance()->ViewportToScreenPos(x, y);
		}
		else
		{
			SceneHierarchyWindow* hierarchy = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
			std::vector<PropertiesWindow*> propertiesWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<PropertiesWindow>(WindowType::Properties);
			if (auto obj = GameObjectSelection::GetHoveredObject())
			{
				if (Input::GetKeyHeld(KeyCode::SHIFT))
				{
					if (obj->IsSelected(0))
					{
						obj->Deselect();
						if (hierarchy) hierarchy->RemoveSelectedObject(obj);
						Editor::GetInstance()->GetGizmos().RemoveSelectedObject(obj);
					}
					else
					{
						obj->AddSelected();
						if (hierarchy)
						{
							hierarchy->AddSelectedObject(obj);
							hierarchy->FocusOnObject(obj);
						}
						Editor::GetInstance()->GetGizmos().AddSelectedObject(obj);
					}
				}
				else
				{
					obj->Select();
					if (hierarchy)
					{
						hierarchy->ClearSelectedObjects();
						hierarchy->AddSelectedObject(obj);
						hierarchy->FocusOnObject(obj);
					}
					Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
					Editor::GetInstance()->GetGizmos().AddSelectedObject(obj);
				}

			}
			else
			{
				propertiesWindows[0]->DeselectAll();
				if (hierarchy) hierarchy->ClearSelectedObjects();
				Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
			}
		}
	}
	if (myIsBoxSelecting && Input::GetKeyReleased(KeyCode::MOUSELEFT))
	{
		SceneHierarchyWindow* hierarchy = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
		std::vector<PropertiesWindow*> propertiesWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<PropertiesWindow>(WindowType::Properties);

		if (myIsBoxSelecting)
		{
			if (!Input::GetKeyHeld(KeyCode::SHIFT))
			{
				if (hierarchy) hierarchy->ClearSelectedObjects();
				Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
			}

			std::vector<GameObject*> objects;
			myIsBoxSelecting = false;
			int x = Input::GetMousePosition().x;
			int y = Input::GetMousePosition().y;
			Vector2i selectRectMax = Engine::GetInstance()->ViewportToScreenPos(x, y);

			std::vector<UINT> selectedIds = DX11::GetScreenObjectIds(mySelectRectMin.x, mySelectRectMin.y, selectRectMax.x, selectRectMax.y);
			for (size_t i = 0; i < selectedIds.size(); i++)
			{
				auto obj = Engine::GetInstance()->GetGameObject(selectedIds[i]);
				if (obj)
				{
					obj->AddSelected();
					if (hierarchy) hierarchy->AddSelectedObject(obj);
					Editor::GetInstance()->GetGizmos().AddSelectedObject(obj);
				}
			}
		}
	}
}

#include "stdafx.h"
#include "SceneWindow.h"
#include "ComponentTools\SceneManager.h"
#include "CommonUtilities\UtilityFunctions.hpp"
#include <ImGuizmo.h>
#include "Editor\Commands\EditorCommandHandler.h"
#include "Assets/AssetLoader.h"
#include "PopupManager.h"

SceneWindow::SceneWindow()
{
	myTexture.CreateScreenSizeTexture(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	myTexture.CreateRenderTargetView();
	mySceneManager = Engine::GetInstance()->GetSceneManager();
	myPlayButton = AssetLoader::LoadTexture("Resources/Icons/PlayButton.dds");
	myPauseButton = AssetLoader::LoadTexture("Resources/Icons/PauseButton.dds");
	myResumeButton = AssetLoader::LoadTexture("Resources/Icons/ResumeButton.dds");
	myStopButton = AssetLoader::LoadTexture("Resources/Icons/StopButton.dds");
}

void SceneWindow::Render()
{
	std::string label = "Scene";
	bool inScene = mySceneManager->GetCurrentScene();
	/*if (inScene)
	{
		label = mySceneManager->GetCurrentScene()->GetName();
		if (EditorCommandHandler::GetCommandCountSinceSave() > 0) label += "*";
	}*/
	
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
	if (!ImGui::Begin((label + "###SceneWindow").c_str(), 0, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar))
	{
		myIsOpen = false;
		ImGui::End();
		ImGui::PopStyleColor();
		return;
	}
	myIsOpen = true;
	if (ImGui::BeginMenuBar())
	{
		ImGui::SetWindowFontScale(1.f);
		ImGui::SetCursorPos({ ImGui::GetCursorPos().x, 26 });

		auto& userPrefs = Editor::GetInstance()->GetUserPrefs();
		if (ImGui::Checkbox("Grid Snap", &userPrefs.mySnapToggle))
		{
			Editor::GetInstance()->GetGizmos().SetSnapToggle(userPrefs.mySnapToggle);
			userPrefs.SaveUserPreferences();
		}

		if (ImGui::Checkbox("Debug Drawer", &userPrefs.myDebugDrawerToggle))
		{
			Engine::GetInstance()->GetGraphicsEngine()->SetDebugDrawerToggle(userPrefs.myDebugDrawerToggle);
			userPrefs.SaveUserPreferences();
		}

		ImGui::Checkbox("Stop with Escape", &myCanEscExit);

		ImGui::SetWindowFontScale(1.3f);

		bool isPlayMode = Engine::GetInstance()->IsGameRunning();
		float iconSize = 16;
		if (inScene)
		{
			if (isPlayMode)
			{
				bool isPaused = Engine::GetInstance()->IsGamePaused();
				DrawPlayButton(myPlayButton, iconSize, -32, 0.4f);

				if (isPaused)
				{
					if (DrawPlayButton(myResumeButton, iconSize, 0, 1))
					{
						Engine::GetInstance()->SetGamePaused(!isPaused);
					}
				}
				else
				{
					if (DrawPlayButton(myPauseButton, iconSize, 0, 1))
					{
						Engine::GetInstance()->SetGamePaused(!isPaused);
					}
				}

				if (DrawPlayButton(myStopButton, iconSize, 32, 1)
					|| (Input::GetKeyPress(KeyCode::ESCAPE) && myCanEscExit))
				{
					Engine::GetInstance()->StopPlayMode();
				}
			}
			else
			{
				if (DrawPlayButton(myPlayButton, iconSize, -32, 1))
				{
					//Disgusting, but hear me out:
					bool cameraFound = false;
					std::vector<GameObject*> sceneObjects;
					for (auto& object : Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetRootObjects())
					{
						SearchChildren(sceneObjects, object);
					}

					for (size_t i = 0; i < sceneObjects.size(); i++)
					{
						if (!sceneObjects[i]->GetComponent<Camera>())
						{
							continue;
						}
						cameraFound = true;
						Editor::GetInstance()->SetUIEditMode(false);
						Engine::GetInstance()->StartPlayMode();
						break;
					}
					if (!cameraFound)
					{
						PopupManager::CreatePopup("NoCamera", 2, { 300,75 }, "Missing Camera!", { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 });
					}
				}

				DrawPlayButton(myPauseButton, iconSize, 0, 0.4f);
				DrawPlayButton(myStopButton, iconSize, 32, 0.4f);
			}
		}
		else
		{
			DrawPlayButton(myPlayButton, iconSize, -32, 0.4f);
			DrawPlayButton(myPauseButton, iconSize, 0, 0.4f);
			DrawPlayButton(myStopButton, iconSize, 32, 0.4f);
		}

		ImGui::EndMenuBar();
	}
	ImGui::PopStyleColor();

	RenderInFrontCheck();

	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	myIsHovered = ImGui::IsWindowHovered();

	auto pos = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();
	size.y = size.x / DX11::GetAspectRatio();

	if (mySize.x != size.x || mySize.y != size.y || myPos.x != pos.x || myPos.y != pos.y)
	{
		if (size.x < 20) size.x = 20;
		myPos = pos;
		mySize = size;
	}

	ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
	ImGui::Image(static_cast<ImTextureID>(myTexture.GetShaderResourceView().Get()), { size.x, size.y });
	
	

	if (inScene)
	{
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_ASSET.prefab");
			if (!payload) payload = ImGui::AcceptDragDropPayload("DRAG_ASSET.fbx");
			if (!payload) payload = ImGui::AcceptDragDropPayload("DRAG_ASSET.scene");
			if (payload)
			{
				std::filesystem::path aPath = *(std::filesystem::path*)payload->Data;
				AssetRegistry::GetInstance()->AutoMakeGameObject(std::filesystem::path(aPath));
			}
			ImGui::EndDragDropTarget();
		}
	}
	else
	{
		ImGui::SetWindowFontScale(1.3f);
		const char* text1 = "No scene selected!";
		auto textWidth = ImGui::CalcTextSize(text1).x;
		ImGui::SetCursorPos({ (mySize.x / 2.f) - textWidth / 2.f, mySize.y / 2.f - 10 });
		ImGui::Text(text1);

		const char* text2 = "Open a scene or create a new one in the Asset Browser.";
		textWidth = ImGui::CalcTextSize(text2).x;
		ImGui::SetCursorPos({ (mySize.x / 2.f) - textWidth / 2.f, mySize.y / 2.f + 10 });
		ImGui::Text(text2);
	}

	ImGui::End();
}

bool SceneWindow::DrawPlayButton(std::shared_ptr<Texture>& aTexture, float& anIconSize, float anOffset, float aTint)
{
	ImGui::SetCursorPos({ mySize.x / 2 + anOffset, 24 });
	return ImGui::ImageButton(static_cast<ImTextureID>(aTexture->GetShaderResourceView().Get()), { anIconSize, anIconSize },
		{ 0, 0 }, { 1, 1 }, -1, { 0, 0, 0, 0 }, { aTint, aTint, aTint, 1 });
}

void SceneWindow::SearchChildren(std::vector<GameObject*>& aGameObjectRefrence, GameObject* aObject)
{
	aGameObjectRefrence.push_back(aObject);
	for (auto& object : aObject->GetTransform()->GetChildren())
	{
		SearchChildren(aGameObjectRefrence, object->GetGameObject());
	}
}

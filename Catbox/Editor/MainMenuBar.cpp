#include "stdafx.h"
#include "MainMenuBar.h"
#include "ComponentTools\SceneManager.h"
#include "CommonUtilities\UtilityFunctions.hpp"

void MainMenuBar::Render()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//ImGui::MenuItem("(demo menu)", NULL, false, false);
			if (ImGui::MenuItem("Create new scene"))
			{
				auto path = Catbox::SaveFile("Catbox Scene(*.scene)\0*.scene\0");

				if (!path.empty())
				{
					auto newScene = CreateAsset<Scene>(path);
					newScene->SaveAsset(path.c_str());
					//mySceneManager->AddScene(newScene);
					Engine::GetInstance()->GetSceneManager()->LoadScene(newScene);
				}
			}
			if (ImGui::MenuItem("Open scene", "Ctrl+O"))
			{
				auto path = Catbox::OpenFile("Catbox Scene(*.scene)\0*.scene\0");
				if (!path.empty())
				{
					std::string name = Catbox::GetNameFromPath(path);
					std::shared_ptr<Scene> scene = AssetRegistry::GetInstance()->GetAsset<Scene>(name);
					if (scene)
					{
						Engine::GetInstance()->GetSceneManager()->LoadScene(scene);
					}
					else
					{
						scene = CreateAsset<Scene>(path);
						//mySceneManager->AddScene(scene);
						Engine::GetInstance()->GetSceneManager()->LoadScene(scene);
					}
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("UI"))
		{
			bool uiEditMode = Editor::GetInstance()->IsUIEditMode();
			bool uiVisible = Editor::GetInstance()->IsUIVisible();
			std::string tag = uiEditMode ? "Exit" : "Enter";
			tag += " UI edit mode";
			if (ImGui::MenuItem(tag.c_str(), "U"))
			{
				Editor::GetInstance()->SetUIEditMode(!uiEditMode);
			}

			tag = uiVisible ? "Hide" : "Show";
			tag += " UI";
			if (ImGui::MenuItem(tag.c_str()))
			{
				Editor::GetInstance()->SetUIVisible(!uiVisible);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			bool savePrefs = true;

			if (ImGui::MenuItem("Asset Browser"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::AssetBrowser);
			}
			else if (ImGui::MenuItem("Animation Controller Editor"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::AnimationControllerEditor);
			}
			/*if (ImGui::MenuItem("Hierarchy"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::SceneHierarchy);
			}*/
			else if (ImGui::MenuItem("Console"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Console);
			}
			else if (ImGui::MenuItem("Chat"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::ChatWindow);
			}
			else if (ImGui::MenuItem("Debugging"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::DebugRenderer);
			}
			else if (ImGui::MenuItem("General Tools"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::GeneralTools);
			}
			else if (ImGui::MenuItem("Catbox Settings"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::CatboxSettings);
			}
			else if (ImGui::MenuItem("Navigation"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Navigation);
			}
			else if (ImGui::MenuItem("Physics"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Physics);
			}
			else if (ImGui::MenuItem("Post Processing"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::PostProcessingEditor);
			}
			else if (ImGui::MenuItem("Shader Editor"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Shader);
			}
			else if (ImGui::MenuItem("Performace Statistics"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Performace);
			}
			else if (ImGui::MenuItem("Visual Scripting"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::VisualScripting);
			}
			else if (ImGui::MenuItem("HotKey Settings"))
			{
				Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::HotKey);
			}			else savePrefs = false;
			if(savePrefs) Editor::GetInstance()->GetUserPrefs().SaveUserPreferences();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

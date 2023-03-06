#include "stdafx.h"
#include "EditorWindowHandler.h"
#include "EditorWindow.h"
#include "PropertiesWindow.h"
#include "SceneHierarchyWindow.h"
#include "CatboxSettingsWindow.h"
#include "AssetBrowserWindow.h"
#include "SceneWindow.h"
#include "PostProcessingWindow.h"
#include "DebuggingWindow.h"
#include "ConsoleWindow.h"
#include "PhysicsWindow.h"
#include "NavigationWindow.h"
#include "ShaderWindow.h"
#include "GeneralToolsWindow.h"
#include "PerformanceWindow.h"
#include "VisualScriptingWindow.h"
#include "HotKeyWindow.h"
#include "AnimationControllerWindow.h"
#include "ChatWindow.h"

void EditorWindowHandler::Render()
{
	for (size_t i = 0; i < myActiveWindows.size(); i++)
	{
		myActiveWindows[i]->Render();
	}
}


std::shared_ptr<EditorWindow> EditorWindowHandler::OpenWindow(WindowType aWindowType)
{
	if (!myWindows[aWindowType].empty()) return nullptr;
	std::shared_ptr<EditorWindow> wnd;
	switch (aWindowType)
	{
	case WindowType::Properties: wnd = std::make_shared<PropertiesWindow>(); break;
	case WindowType::SceneHierarchy: wnd = std::make_shared<SceneHierarchyWindow>(); break;
	case WindowType::CatboxSettings: wnd = std::make_shared<CatboxSettingsWindow>(); break;
	case WindowType::AssetBrowser: wnd = std::make_shared<AssetBrowserWindow>(); break;
	case WindowType::Scene: wnd = std::make_shared<SceneWindow>(); break;
	case WindowType::PostProcessingEditor: wnd = std::make_shared<PostProcessingWindow>(); break;
	case WindowType::DebugRenderer: wnd = std::make_shared<DebuggingWindow>(); break;
	case WindowType::Console: wnd = std::make_shared<ConsoleWindow>(); break;
	case WindowType::Physics: wnd = std::make_shared<PhysicsWindow>(); break;
	case WindowType::Navigation: wnd = std::make_shared<NavigationWindow>(); break;
	case WindowType::Shader: wnd = std::make_shared<ShaderWindow>(); break;
	case WindowType::GeneralTools: wnd = std::make_shared<GeneralToolsWindow>(); break;
	case WindowType::Performace: wnd = std::make_shared<PerformanceWindow>(); break;
	case WindowType::VisualScripting: wnd = std::make_shared<VisualScriptingWindow>(); break;
	case WindowType::HotKey: wnd = std::make_shared<HotKeyWindow>(); break;
	case WindowType::AnimationControllerEditor: wnd = std::make_shared<AnimationControllerWindow>(); break;
	case WindowType::ChatWindow: wnd = std::make_shared<ChatWindow>(); break;
	default: return nullptr;
	}

	myWindows[aWindowType].push_back(wnd);
	myActiveWindows.push_back(wnd);
	wnd->myType = aWindowType;
	return wnd;
}

void EditorWindowHandler::CloseWindow(EditorWindow* aWindow)
{
	for (size_t i = 0; i < myActiveWindows.size(); i++)
	{
		if (myActiveWindows[i].get() == aWindow)
		{
			myActiveWindows.erase(myActiveWindows.begin() + i);
			Editor::GetInstance()->GetUserPrefs().SaveUserPreferences();
			break;
		}
	}

	auto& windowsOfType = myWindows[aWindow->GetType()];
	for (size_t i = 0; i < windowsOfType.size(); i++)
	{
		if (windowsOfType[i].get() == aWindow)
		{
			windowsOfType.erase(windowsOfType.begin() + i);
			break;
		}
	}
}

void EditorWindowHandler::BringToFront(EditorWindow* aWindow)
{
	for (size_t i = 0; i < myActiveWindows.size(); i++)
	{
		if (myActiveWindows[i].get() == aWindow) 
		{
			std::shared_ptr<EditorWindow> wnd = myActiveWindows[i];
			myActiveWindows.erase(myActiveWindows.begin() + i);
			myActiveWindows.insert(myActiveWindows.begin(), wnd);
			break;
		}
	}
}

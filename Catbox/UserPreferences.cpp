#include "stdafx.h"
#include "UserPreferences.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include "Editor/Windows/EditorWindow.h"


void UserPreferences::SaveUserPreferences()
{
	int version = 8;
	rapidjson::Document output;
	output.SetObject();
	auto& alloc = output.GetAllocator();
	rapidjson::Value value;

	value.SetInt(version);
	output.AddMember("Version", value, alloc);
	value.SetInt(static_cast<int>(myTheme));
	output.AddMember("Theme", value, alloc);
	value.SetFloat(myCamMoveSpeed);
	output.AddMember("CamMoveSpeed", value, alloc);
	value.SetFloat(myCamScrollSpeed);
	output.AddMember("CamScrollSpeed", value, alloc);
	value.SetFloat(myCamRotateSpeed);
	output.AddMember("CamTurnSpeed", value, alloc);

	myClearColor.ParseToJsonObject(output, output, "ClearColor");
	mySelectionOutlineColor.ParseToJsonObject(output, output, "OutlineColor");

	value.SetObject();
	rapidjson::Value moveArray(rapidjson::kArrayType);
	moveArray.SetArray();
	moveArray.PushBack(myMoveSnap[0], alloc);
	moveArray.PushBack(myMoveSnap[1], alloc);
	moveArray.PushBack(myMoveSnap[2], alloc);
	value.AddMember("Move", moveArray, alloc);

	rapidjson::Value rotArray(rapidjson::kArrayType);
	rotArray.PushBack(myRotateSnap[0], alloc);
	rotArray.PushBack(myRotateSnap[1], alloc);
	rotArray.PushBack(myRotateSnap[2], alloc);
	value.AddMember("Rotate", rotArray, alloc);

	rapidjson::Value scaleArray(rapidjson::kArrayType);
	scaleArray.PushBack(myScaleSnap[0], alloc);
	scaleArray.PushBack(myScaleSnap[1], alloc);
	scaleArray.PushBack(myScaleSnap[2], alloc);
	value.AddMember("Scale", scaleArray, alloc);

	rapidjson::Value snapToggle;
	snapToggle.SetBool(mySnapToggle);
	value.AddMember("Snap Toggle", snapToggle, alloc);

	output.AddMember("Snap Settings", value, alloc);

	rapidjson::Value debugDrawerToggle;
	debugDrawerToggle.SetBool(myDebugDrawerToggle);
	output.AddMember("Debug Drawer Toggle", debugDrawerToggle, alloc);

	rapidjson::Value windows(rapidjson::kArrayType);
	std::vector<std::shared_ptr<EditorWindow>>& activeWindows = Editor::GetInstance()->GetWindowHandler().GetActiveWindows();
	for (int i = static_cast<int>(activeWindows.size()-1); i >= 0; i--)
	{
		windows.PushBack(static_cast<int>(activeWindows[i]->GetType()), alloc);
	}
	output.AddMember("Open Windows", windows, alloc);

	value.SetString(rapidjson::StringRef(myDefaultIP.c_str()));
	output.AddMember("IP", value, alloc);
	value.SetString(rapidjson::StringRef(myUsername.c_str()));
	output.AddMember("Username", value, alloc);
	value.SetString(rapidjson::StringRef(myChatColor.c_str()));
	output.AddMember("ChatColor", value, alloc);

	std::ofstream ofs("EditorSettings.json");
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	output.Accept(writer);
}

void UserPreferences::LoadUserPreferences()
{
	FILE* fp;
	bool error = fopen_s(&fp, "EditorSettings.json", "rb");
	if (error) //no editor settings file found
	{
		ImGui::StyleColorsDark();
		Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::AssetBrowser);
		Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::SceneHierarchy);
		Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Properties);
		Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Scene);
		return;
	}

	rapidjson::Document document;
	char readbuffer[65536]{};
	rapidjson::FileReadStream is(fp, readbuffer, sizeof(readbuffer));
	document.ParseStream(is);
	fclose(fp);

	myTheme = (Theme)document["Theme"].GetInt();
	if (myTheme == Theme::Night) ImGui::StyleColorsDark();
	else ImGui::StyleColorsLight();

	if (document.HasMember("Version"))
	{
		int version = document["Version"].GetInt();
		if (version >= 1)
		{
			myClearColor.LoadFromJson(document["ClearColor"].GetObj());
			Engine::GetInstance()->GetGraphicsEngine()->SetClearColor(myClearColor);
			if (version >= 2)
			{
				myCamMoveSpeed = document["CamMoveSpeed"].GetFloat();
				myCamScrollSpeed = document["CamScrollSpeed"].GetFloat();
				myCamRotateSpeed = document["CamTurnSpeed"].GetFloat();
				Editor::GetInstance()->GetEditorCamera().SetMoveSpeed(myCamMoveSpeed);
				Editor::GetInstance()->GetEditorCamera().SetScrollSpeed(myCamScrollSpeed);
				Editor::GetInstance()->GetEditorCamera().SetRotateSpeed(myCamRotateSpeed);

				if (version >= 3)
				{
					auto snapSettings = document["Snap Settings"].GetObj();
					auto moveSettings = snapSettings["Move"].GetArray();
					auto rotSettings = snapSettings["Rotate"].GetArray();
					auto scaleSettings = snapSettings["Scale"].GetArray();

					for (int i = 0; i < 3; i++)
					{
						myMoveSnap[i] = moveSettings[i].GetFloat();
						myRotateSnap[i] = rotSettings[i].GetFloat();
						myScaleSnap[i] = scaleSettings[i].GetFloat();
					}

					if (version >= 6)
					{
						mySnapToggle = snapSettings["Snap Toggle"].GetBool();
						if (version >= 7)
						{
							myDefaultIP = document["IP"].GetString();
							myUsername = document["Username"].GetString();
							myChatColor = document["ChatColor"].GetString();
						}
						if (version >= 8) 
						{
							myDebugDrawerToggle = document["Debug Drawer Toggle"].GetBool();
							Engine::GetInstance()->GetGraphicsEngine()->SetDebugDrawerToggle(myDebugDrawerToggle);
						}
					}

					Editor::GetInstance()->GetGizmos().SetMoveSnap(myMoveSnap);
					Editor::GetInstance()->GetGizmos().SetRotateSnap(myRotateSnap);
					Editor::GetInstance()->GetGizmos().SetScaleSnap(myScaleSnap);
					Editor::GetInstance()->GetGizmos().SetSnapToggle(mySnapToggle);

					if (document.HasMember("Open Windows"))
					{
						auto windows = document["Open Windows"].GetArray();
						for (auto& wnd : windows)
						{
							Editor::GetInstance()->GetWindowHandler().OpenWindow(static_cast<WindowType>(wnd.GetInt()));
						}
					}
					else
					{
						Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::AssetBrowser);
						Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::SceneHierarchy);
						Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Properties);
						Editor::GetInstance()->GetWindowHandler().OpenWindow(WindowType::Scene);
					}

					if (version >= 5)
					{
						mySelectionOutlineColor.LoadFromJson(document["OutlineColor"].GetObj());

					}
				}
			}
		}
	}
}

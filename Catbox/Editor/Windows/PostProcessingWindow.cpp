#include "stdafx.h"
#include "PostProcessingWindow.h"
#include "CommonUtilities\RapidJsonWrapper.h"
#include "Components/3D/Camera.h"
#include "CommonUtilities/UtilityFunctions.hpp"

PostProcessingWindow::PostProcessingWindow()
{
}

void PostProcessingWindow::Render()
{
	if (!BeginWindow("Post processing settings", true)) return;

	if (Catbox::Checkbox("Preview in editor", &myPreviewEnabled))
	{
		Editor::GetInstance()->GetEditorCamera().GetCamera()->GetPostProcessingVolume()->SetEnabled(myPreviewEnabled);
	}


	std::vector<const char*> profileNames;
	auto profileList = AssetRegistry::GetInstance()->GetAssets<PostProcessingProfile>();
	for (auto& p : profileList)
	{
		profileNames.push_back(p->GetName().c_str());
	}

	ImGui::SetNextItemWidth(100);
	if (ImGui::Combo("Profile", &mySelectedProfileIndex, profileNames.data(), static_cast<int>(profileNames.size())))
	{
		mySelectedProfile = std::dynamic_pointer_cast<PostProcessingProfile>(profileList[mySelectedProfileIndex]);
		Editor::GetInstance()->GetEditorCamera().GetCamera()->GetPostProcessingVolume()->SetProfile(mySelectedProfile);
	}

	if (mySelectedProfile)
	{
		mySelectedProfile->RenderInProperties();
	}


	if (ImGui::Button("Create new", { 100, 30 }))
	{
		auto path = Catbox::SaveFile("Post-process Profile(*.profile)\0*.profile\0");

		if (!path.empty())
		{
			mySelectedProfile = CreateAsset<PostProcessingProfile>(path);
			mySelectedProfileIndex = static_cast<int>(profileList.size() - 1);
			if (Editor::GetInstance()->GetEditorCamera().GetCamera()->GetPostProcessingVolume())
			{
				Editor::GetInstance()->GetEditorCamera().GetCamera()->GetPostProcessingVolume()->SetProfile(mySelectedProfile);
			}
			mySelectedProfile->SaveAsset(path.c_str());
		}
	}


	ImGui::SameLine();
	if (ImGui::Button("Load", { 70, 30 }))
	{
		auto path = Catbox::OpenFile("Post-process Profile (*.profile)\0*.profile\0");
		if (!path.empty())
		{
			mySelectedProfile = AssetRegistry::GetInstance()->GetAsset<PostProcessingProfile>(Catbox::GetNameFromPath(path));
			if (!mySelectedProfile)
			{
				mySelectedProfile = CreateAsset<PostProcessingProfile>(path);
			}

			mySelectedProfile->LoadFromPath(path.c_str());


			if (Editor::GetInstance()->GetEditorCamera().GetCamera()->GetPostProcessingVolume())
			{
				Editor::GetInstance()->GetEditorCamera().GetCamera()->GetPostProcessingVolume()->SetProfile(mySelectedProfile);
			}
		}
	}

	EndWindow();
}

void PostProcessingWindow::Save()
{

}

#include "stdafx.h"
#include "Editor\Windows\PropertiesWindow.h"
#include "CommonUtilities/UtilityFunctions.hpp"
#include "CommonUtilities/InputHandler.h"

int Asset::globalAssetCount = 0;
Asset::Asset()
{
	myRuntimeId = std::to_string(globalAssetCount++);
	myAssetId.GenerateNew();
	myRootAsset = this;
}

void Asset::SetPath(const std::string aPath)
{
	myPath = aPath;
	myPathStr = aPath;
	myName = Catbox::GetNameFromPath(aPath);
}

void Asset::RenderInProperties()
{
	std::vector<Asset*> assetList;
	assetList.push_back(this);
	RenderInProperties(assetList);
}

const ImGuiPayload* Asset::DragDropTarget()
{
	if (ImGui::BeginDragDropTarget())
	{
		const auto& payload = ImGui::AcceptDragDropPayload(("DRAG_ASSET" + myPath.extension().string()).c_str());
		ImGui::EndDragDropTarget();
		return payload;
	}
	return nullptr;
}

void Asset::RenderInProperties(std::vector<Asset*>& /*anAssetList*/)
{
	RenderNameInputBox();
	ImGui::Text("Custom RenderInProperties not created for selected asset type");
}

void Asset::Deselect()
{
	auto properties = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<PropertiesWindow>(WindowType::Properties);
	for (size_t i = 0; i < properties.size(); i++)
	{
		properties[i]->Deselect(this);
	}
}

void Asset::Select()
{
	Editor::GetInstance()->SetCanDeleteObjects(true);
	std::vector<PropertiesWindow*> propertiesWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<PropertiesWindow>(WindowType::Properties);
	for (size_t i = 0; i < propertiesWindows.size(); i++)
	{
		if (!propertiesWindows[i]->IsLocked())
		{
			propertiesWindows[i]->SetSelected(this);
			return;
		}
	}
}

void Asset::AddSelected()
{
	Editor::GetInstance()->SetCanDeleteObjects(true);
	std::vector<PropertiesWindow*> propertiesWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<PropertiesWindow>(WindowType::Properties);
	for (size_t i = 0; i < propertiesWindows.size(); i++)
	{
		if (!propertiesWindows[i]->IsLocked())
		{
			propertiesWindows[i]->AddSelected(this);
			return;
		}
	}
}

void Asset::ExtractNameFromPath()
{
	myName = Catbox::GetNameFromPath(myPath.string());
}

bool Asset::operator<(Asset& aAsset) const
{
	return (myName < aAsset.myName);
}
bool Asset::operator>(Asset& aAsset) const
{
	return (myName < aAsset.myName);
}

void Asset::RenderNameInputBox()
{
	char nameInput[64] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	std::string emptyText = "##MaterialNameInput";
	if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	{
		myName = nameInput;
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}
}
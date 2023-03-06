#include "stdafx.h"
#include "Blueprint.h"
#include "Editor/ScriptGraph/BlueprintEditor.h"
#include "ScriptGraph/ScriptGraph.h"


void Blueprint::SaveAsset(const char* aPath)
{
	if (!myGraph)
	{
		myGraph = ScriptGraphSchema::CreateScriptGraph();
	}
	Editor::GetInstance()->GetBlueprintEditor()->SaveGraph(myGraph, myPath.string());
}

void Blueprint::LoadFromPath(const char* aPath)
{
	myGraph = ScriptGraphSchema::CreateScriptGraph();

	if (EDITORMODE)
	{
		Editor::GetInstance()->GetBlueprintEditor()->LoadGraph(myGraph, myPath.string());
	}
	else
	{
		std::ifstream file(aPath);

		const std::string inGraph = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		file.close();

		ScriptGraphSchema::DeserializeScriptGraph(myGraph, inGraph, EDITORMODE);
	}
}

void Blueprint::DoubleClickAsset()
{
	Editor::GetInstance()->GetBlueprintEditor()->SetGraph(myGraph, myPath.string());
}

void Blueprint::RenderInProperties(std::vector<Asset*>& anAssetList)
{
	char nameInput[32] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	std::string emptyText = "##BlueprintNameInput";
	if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	{
		myName = nameInput;
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}
}

#include "stdafx.h"
#include "ScriptGraphEditorBase.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include "imgui_stdlib.h"
#include "ScriptGraph/ScriptGraphTypes.h"
#include "ScriptGraph/Nodes/SGNode_Variable.h"
#include "ScriptGraph/Nodes/Math/SGNode_MathOps.h"

#include "RegisterExternalTypes.h"



// Copied from MuninUtils.
// Measures differences in two strings.
template<typename T>
std::enable_if_t<std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, typename T::size_type>
LevenshteinDistance(const T& aString, const T& aSearchString)
{
	if (aString.size() > aSearchString.size())
	{
		return LevenshteinDistance(aSearchString, aString);
	}

	using TSize = typename T::size_type;
	const TSize minSize = aString.size();
	const TSize maxSize = aSearchString.size();
	std::vector<TSize> levenDistance(minSize + 1);

	for (TSize s = 0; s <= minSize; ++s)
	{
		levenDistance[s] = s;
	}

	for (TSize s = 1; s <= maxSize; ++s)
	{
		TSize lastDiag = levenDistance[0], lastDiagMem;
		++levenDistance[0];

		for (TSize t = 1; t <= minSize; ++t)
		{
			lastDiagMem = levenDistance[t];
			if (aString[t - 1] == aSearchString[s - 1])
			{
				levenDistance[t] = lastDiag;
			}
			else
			{
				levenDistance[t] = std::min(std::min(levenDistance[t - 1], levenDistance[t]), lastDiag) + 1;
			}
			lastDiag = lastDiagMem;
		}
	}

	return levenDistance[minSize];
}


void ScriptGraphEditorBase::NodeContextMenu(size_t aNodeUID)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("NodeContextMenu"))
	{
		if (ImGui::BeginMenu("Node Context Menu"))
		{
			if (ImGui::MenuItem("Herp")) {}
			if (ImGui::MenuItem("Derp")) {}
			if (ImGui::MenuItem("Slerp")) {}

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void ScriptGraphEditorBase::LinkContextMenu(size_t aLinkUID)
{

}




void ScriptGraphEditorBase::HandleScriptGraphError(
	size_t aNodeUID, const std::string& anErrorMessage)
{
	myState.errorIsErrorState = true;
	myState.errorMessage = anErrorMessage;
	myState.errorNodeId = aNodeUID;

	ImNodeEd::SelectNode(myState.errorNodeId);
	ImNodeEd::NavigateToSelection();
	ImNodeEd::DeselectNode(myState.errorNodeId);

	myState.isTicking = false;
}

void ScriptGraphEditorBase::ClearErrorState()
{
	myState.errorIsErrorState = false;
	myState.errorMessage = "";
	myState.errorNodeId = 0;
}


void ScriptGraphEditorBase::Init()
{
	nodeEditorContext = ImNodeEd::CreateEditor(&nodeEditorCfg);
	myState.initNavToContent = true;
}
#pragma once
#include <memory>
#include <vector>

#include "imgui.h"

#include "MuninScriptGraph.h"

namespace ImNodeEd = ax::NodeEditor;

class ScriptGraphEditorBase
{
protected:
	struct SearchMenuItem
	{
		std::string Title;
		std::string Value;
		size_t Rank = SIZE_MAX;
		std::string Tag;
	};

	struct ContextMenuItem
	{
		std::string Title;
		std::string Value;
		std::string Tag;
		std::string Acessibility = "Public";
	};

	struct ContextMenuCategory
	{
		std::string Title;
		std::vector<ContextMenuItem> Items;
	};

	struct EditorInterfaceState
	{
		// The current value of the node search list.
		std::string bgCtxtSearchField;
		bool bgCtxtSearchFieldChanged = false;
		bool bgCtxtSearchFocus = false;
		std::vector<SearchMenuItem> bgCtxtSearchFieldResults;

		std::string varNewVarNameField;
		int varNewVarTypeIdx = 0;
		ScriptGraphDataObject varNewVarValue;
		int varInlineEditingIdx = -1;

		ContextMenuCategory bgCtxtVariablesCategory;
		std::string varToDelete;

		bool flowShowFlow = false;
		bool isTicking = false;

		bool errorIsErrorState = false;
		std::string errorMessage;
		size_t errorNodeId = 0;

		bool initNavToContent = false;
	} myState;
	std::string myCurrentGraphPath;
	std::vector<std::string> myComponents;

	ImNodeEd::Config nodeEditorCfg;
	ImNodeEd::EditorContext* nodeEditorContext;
	// TEMP

	std::vector<ax::NodeEditor::NodeId> myCopyVector;


	// Context Menues
	void NodeContextMenu(size_t aNodeUID);
	void LinkContextMenu(size_t aLinkUID);
	virtual void UpdateVariableContextMenu() {};

	
	void HandleScriptGraphError(size_t aNodeUID, const std::string& anErrorMessage);

	void ClearErrorState();

public:
	/*void Copy();
	void Paste();*/
	virtual void Init();
	void SetComponents(std::vector<std::string> aComponentList) { myComponents = aComponentList;  };
	void ClearComponets() { myComponents.clear(); }
};

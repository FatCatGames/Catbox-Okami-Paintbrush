#pragma once
#include "ScriptGraphEditorBase.h"
#include <memory>
#include <vector>

#include "imgui.h"

#include "MuninScriptGraph.h"
namespace ImNodeEd = ax::NodeEditor;

class BlueprintEditor : public ScriptGraphEditorBase
{
public:
	void Render();

private:

	// Graph Rendering stuff
	void RenderNode(const ScriptGraphNode* aNode);
	void RenderPin(const ScriptGraphPin* aPin);
	void DrawPinIcon(const ScriptGraphPin* aPin, ImVec4 aPinColor, bool isConnected) const;

protected:
	
	// Actual storage of looking up categories when generating.
	static inline std::unordered_map<std::string, ContextMenuCategory> myBgContextCategories;
	// Sorted list of context category names.
	static inline std::vector<std::string> myBgContextCategoryNamesList;


	// TEMP
	std::unique_ptr<ScriptGraphSchema> mySchema;
	std::shared_ptr<ScriptGraph> myGraph;
	std::vector<ax::NodeEditor::NodeId> myCopyVector;

	void UpdateVariableContextMenu() override;

	// Context Menues
	void BackgroundContextMenu();
	void NodeContextMenu(size_t aNodeUID);
	void LinkContextMenu(size_t aLinkUID);

	// Modals
	void TriggerEntryPoint();
	void EditVariables();

	// Event Stuff
	void HandleEditorCreate();
	void HandleEditorDelete();


public:
	void Copy();
	void Paste();
	void Init() override;
	void Update(float aDeltaTime);
	void LoadGraph(std::shared_ptr<ScriptGraph>& aGraph, const std::string& aPath);
	void SetGraph(std::shared_ptr<ScriptGraph>& aGraph, const std::string& aPath);
	void SaveGraph(const std::shared_ptr<ScriptGraph> aGraph, const std::string& aPath);
	void SaveCurrent();

};

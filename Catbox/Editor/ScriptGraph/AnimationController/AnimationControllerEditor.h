#pragma once
#include "../ScriptGraphEditorBase.h"

#include "MuninGraph.h"
#include "AnimationGraphPin.h"
#include "AnimationGraphNode.h"
#include "AnimationGraph.h"
#include "AnimationGraphSchema.h"
#include "ComponentTools\Event.h"

struct AnimationGraphPin;
class AnimationGraphSchema;
class AnimationGraph;
class AnimationController;

class AnimationControllerEditor : public ScriptGraphEditorBase
{
public:
	void Render();
	void SetController(AnimationController* aController);
	void ClearController();

private:
	void CreateNode();
	// Modals
	void TriggerEntryPoint();
	void EditVariables();
	void RenderPropertyMenu(std::vector<const NodeGraphEdge*> aSelectedEdges);
	void RenderNode(const AnimationGraphNode* aNode);
	void RenderVariableMenu();
	bool RenderLayerMenu();
	// Event Stuff
	void HandleEditorCreate();

public:
	std::unique_ptr<AnimationGraphSchema> mySchema;
	std::shared_ptr<AnimationGraph> myGraph = nullptr;
	AnimationController* myCurrentController;
	GameObject* mySelectedObject;

	void ShowBoneList(int aiD);

	//void Copy();
	//void Paste();
	void RightClickMenu();
	void Update(float aDeltaTime);
	void Init();
	void LoadGraph(AnimationController* aController, const std::string& aPath);
	void SaveGraph(AnimationController* aController, const std::string& aPath);
	void SetGraph(std::shared_ptr<AnimationGraph>& aGraph, const std::string& aPath);
	void SetSelectedObject(GameObject* anObject);
	void ClearSelectedObject();


	void SaveCurrent();
	void HandleEditorDelete();
	Listener myOnSceneLoadedListener;
};

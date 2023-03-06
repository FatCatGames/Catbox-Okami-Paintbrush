#pragma once
#include <functional>
#include <memory>
#include "Graphics\Animations\Animation.h"
class AnimationController;
class Skeleton;
struct AnimationGraphNodePayload;
struct ScriptGraphVariable;
class AnimationGraphPin;
class AnimationGraphNode;
struct NodeGraphEdge;
class AnimationGraphSchema;
class AnimationGraph;
class AnimationTransition;

class AnimationGraphInternal : public NodeGraph<AnimationGraphNode, AnimationGraphPin, NodeGraphEdge, AnimationGraphSchema>
{
	// All AnimationGraph private things go in here.
	typedef NodeGraph<AnimationGraphNode, AnimationGraphPin, NodeGraphEdge, AnimationGraphSchema> Super;

	// Our Schema can do whatever it wants.
	friend AnimationGraphSchema;
	friend AnimationGraph;
	AnimationGraphInternal() = default;

	// A map of all nodes that can start execution flow along with a node handle.
	// This allows you to call a specific start node at a specific point via this handle.
	std::shared_ptr<AnimationGraphNode> myEntryPoint;
	std::shared_ptr<AnimationGraphNode> myCurrentNode;
	std::unordered_map<size_t, AnimationGraphNode*> myAnyStateNodes;

	// Reverse lookup to find the entry handle from the Node UId;
	std::unordered_map<size_t, std::string> myNodeUIDToEntryHandle;
	std::unordered_map<size_t, AnimationTransition> myTransitions = {};

	AnimationController* myController;
	std::unordered_map<std::string, std::shared_ptr<ScriptGraphVariable>>* myVariables;

	std::vector<size_t> myLastExecutedPath;

	bool bShouldTick = false;
	int myObjectID = -1;

public:
	typedef std::function<void(const class AnimationGraph&, size_t, const std::string&)> ScriptGraphErrorHandlerSignature;

private:
	ScriptGraphErrorHandlerSignature myErrorDelegate;

protected:

	// Node interface goes here
	virtual const AnimationGraphPin& GetDataSourcePin(size_t aPinUID) const override;

	void ReportEdgeFlow(size_t anEdgeUID);

	void ReportFlowError(size_t aNodeUID, const std::string& anErrorMessage) const;

public:
	AnimationState myCurrentState;
	int myFrameIndex = 0;
	int myNextFrameIndex = 0;
	float myTimeSinceFrame = 0;
	float myFramePercent = 0;
	bool isActive = false;
	std::array<bool, 128> myBoneMask;
	std::vector<int> myBoneMaskRootIDs;

	const std::vector<size_t>& GetLastExecutedPath() const { return myLastExecutedPath; }
	void ResetLastExecutedPath() { myLastExecutedPath.clear(); }
	std::unordered_map<size_t, AnimationTransition>& GetTransitions() { return myTransitions; }
	[[nodiscard]] const std::unordered_map<size_t, AnimationGraphNode*>& GetAnyStateNodes() const { return myAnyStateNodes; }


	void BindErrorHandler(ScriptGraphErrorHandlerSignature&& aErrorHandler);
	void UnbindErrorHandler();

	void RegenerateBoneMask(std::shared_ptr<Skeleton> aSkeleton);
	bool Animate();
	bool Run();
	void Reset();
	void SetObjectID(int anID) { myObjectID = anID; }
	int GetObjectID() { return myObjectID; }
	void SetController(AnimationController* aController);
	AnimationController* GetController() { return myController; }
	std::shared_ptr<AnimationGraphNode> GetEntryPoint() { return myEntryPoint; }
	std::shared_ptr<AnimationGraphNode> GetCurrentNode() { return myCurrentNode; }
};

class AnimationGraph : public AnimationGraphInternal
{
	// These classes can access Protected items in ScriptGraphInternal.
	friend class AnimationGraphNode;
	friend class NodeGraphNode<AnimationGraphPin, AnimationGraph, AnimationGraphSchema>;
};

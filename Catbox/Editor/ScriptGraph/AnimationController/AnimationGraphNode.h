#pragma once
#include "AnimationGraphPin.h"
#include "../Graph/NodeGraphNode.h"
#include "AnimationGraph.h"
#include "AnimationGraphSchema.h"
#include "ScriptGraphDataObject.h"
#include "AnimationEvent.h"

class AnimationGraph;
class Animation;

class AnimationGraphNode : public NodeGraphNode<AnimationGraphPin, AnimationGraph, AnimationGraphSchema>, public std::enable_shared_from_this<AnimationGraphNode>, public Munin::ObjectUUID<AnimationGraphNode>
{
	typedef NodeGraphNode<AnimationGraphPin, AnimationGraph, AnimationGraphSchema> ParentClass;

	bool isExecNode = false;
	bool isEntryNode = false;
	bool isAnyStateNode = false;
	std::shared_ptr<Animation> myAnimation{nullptr};
	std::string myDescription;
	std::string myCategory = "Default";
	std::string myAcessiblity = "Public";
	std::string myErrorMessage;
	ImVec4 aPos;
	bool hasErrored = false;

protected:
	void CreatePin(const std::string& aLabel, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		AddPin(AnimationGraphPin::CreatePin(AsSharedPtr(), aLabel, PinIcon::Exec, aDirection, hideLabelOnNode));
		isExecNode = true;
	}

	size_t ExitViaPin(const std::string& aPinLabel) override;

	size_t ExitWithError(const std::string& anErrorMessage);

	void SetDescription(const std::string& aDescription);
	void SetCategory(const std::string& aCategory);
	void SetAcessibility(const std::string& aAcess);

	std::shared_ptr<AnimationGraphNode> AsSharedPtr() { return shared_from_this(); }

public:

	void Init();
	size_t Exit();
	size_t DoOperation() override;
	void SetIsEntryNode(bool aIsEntryNode) { isEntryNode = aIsEntryNode; }
	void SetAnimation(std::shared_ptr<Animation> anAnimation) { myAnimation = anAnimation; }
	void SetIsAnyState(bool aState);
	/**
	 * Called when this node is asked to Execute. This will ONLY happen if the node has at least one Exec Input Pin.
	 * By default this function will call DoOperation on the node but you may modify this behavior as you please.
	 * @param anEntryPinUID The Pin that caused this node to Execute (i.e. an Input Exec pin we have). If we have no input Exec pins this will be 0!
	 * @returns The Pin Index of the pin we want to exit on after performing our operation or 0 if we don't have a pin to exit on.
	 */
	size_t Exec(size_t anEntryPinUID);

	std::unordered_map<size_t, AnimationTransition*> myTransitions;
	std::vector<AnimationEvent> myEvents;
	~AnimationGraphNode() = default;

	FORCEINLINE bool IsExecNode() const { return isExecNode; }
	FORCEINLINE bool IsEntryNode() const { return isEntryNode; }
	FORCEINLINE bool IsAnyStateNode() const { return isAnyStateNode; }
	FORCEINLINE bool IsInternalOnly() const { return false; }
	FORCEINLINE bool IsSimpleNode() const { return false; }
	bool IsRunning() const { return isRunning; }

	FORCEINLINE const std::string& GetDescription() const { return myDescription; }
	FORCEINLINE const std::string& GetCategory() const { return myCategory; }
	FORCEINLINE const std::string& GetAcessibility() const { return myAcessiblity; }
	FORCEINLINE std::shared_ptr<Animation> GetAnimation() const { return myAnimation; }
	FORCEINLINE std::vector<AnimationEvent>& GetEvents() { return myEvents; }

	FORCEINLINE ImVec4 GetNodeHeaderColor() const override { return ImVec4(80, 124, 153, 255); };

	// Controls how many instances of this node may coexist in the same graph.
	// If > 0 this will be enforced.
	FORCEINLINE unsigned MaxInstancesPerGraph() const { return 0; }

	FORCEINLINE bool HasError() const { return hasErrored; }
	FORCEINLINE const std::string& GetErrorMessage() const { return myErrorMessage; }

	int myObjectID = -1;
	bool isRunning = false;
	bool myShouldLoop = true;
	bool myCanTransitionToItself = false;
};

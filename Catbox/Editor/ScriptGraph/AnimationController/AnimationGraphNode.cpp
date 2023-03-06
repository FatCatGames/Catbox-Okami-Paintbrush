#include "stdafx.h"
#include "MuninGraph.pch.h"
#include "AnimationGraphNode.h"

#include "ScriptGraphTypes.h"

size_t AnimationGraphNode::ExitViaPin(const std::string& aPinLabel)
{
	const size_t pinUID = NodeGraphNode<AnimationGraphPin, AnimationGraph, AnimationGraphSchema>::ExitViaPin(aPinLabel);
	const AnimationGraphPin& pin = *GetPin(pinUID);

	if(pin.IsPinConnected())
	{
		const size_t exitEdgeUID = pin.GetEdges()[0];
		const NodeGraphEdge& exitEdge = GetOwner()->GetEdgeFromUID(exitEdgeUID);
		const AnimationGraphPin& targetPin = GetOwner()->GetPinFromUID(exitEdge.ToUID);

		GetOwner()->ReportEdgeFlow(exitEdgeUID);

		return targetPin.GetOwner()->Exec(targetPin.GetUID());
	}

	return pinUID;
}

void AnimationGraphNode::Init()
{
	CreatePin("In", PinDirection::Input, true);
	CreatePin("Out", PinDirection::Output, true);
}

size_t AnimationGraphNode::ExitWithError(const std::string& anErrorMessage)
{
	hasErrored = true;
	myErrorMessage = anErrorMessage;
	const auto uuidPtr = AsUUIDAwarePtr(this);
	GetOwner()->ReportFlowError(uuidPtr->GetUID(), anErrorMessage);
	return 0;
}

void AnimationGraphNode::SetDescription(const std::string& aDescription)
{
	myDescription = aDescription;
}

void AnimationGraphNode::SetCategory(const std::string& aCategory)
{
	myCategory = aCategory;
}

void AnimationGraphNode::SetAcessibility(const std::string& aAcess)
{
	myAcessiblity = aAcess;
}


size_t AnimationGraphNode::Exit()
{
	hasErrored = false;
	return ParentClass::Exit();
}

size_t AnimationGraphNode::DoOperation()
{
	return Exit();
}

void AnimationGraphNode::SetIsAnyState(bool aState)
{
	isAnyStateNode = aState;
}


size_t AnimationGraphNode::Exec(size_t anEntryPinUID)
{
	isRunning = true;
	hasErrored = false;
	return DoOperation();
}

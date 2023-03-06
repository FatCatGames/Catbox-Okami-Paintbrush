#include "stdafx.h"
#include "MuninGraph.pch.h"
#include "MuninGraph.h"
#include "AnimationGraphPin.h"
#include "AnimationGraphNode.h"
#include "AnimationGraph.h"
#include "AnimationGraphSchema.h"
#include <fstream>
#include "..\nlohmann\json.hpp"

#include "AnimationGraphNode.h"
#include "Nodes/SGNode_Variable.h"
#include "AnimationTransition.h"
#include "Assets\AnimationController.h"
#include <Animator.cpp>
#include <stack>


const AnimationGraphPin& AnimationGraphInternal::GetDataSourcePin(size_t aPinUID) const
{
	const AnimationGraphPin& dataPin = Super::GetDataSourcePin(aPinUID);
	if (dataPin.GetUID() != aPinUID && !dataPin.GetOwner()->IsExecNode())
	{
		dataPin.GetOwner()->DoOperation();
	}

	return dataPin;
}

void AnimationGraphInternal::ReportEdgeFlow(size_t anEdgeUID)
{
	myLastExecutedPath.push_back(anEdgeUID);
}

void AnimationGraphInternal::ReportFlowError(size_t aNodeUID, const std::string& anErrorMessage) const
{
	if (myErrorDelegate)
	{
		myErrorDelegate(*dynamic_cast<const AnimationGraph*>(this), aNodeUID, anErrorMessage);
	}
}

bool AnimationGraphInternal::Animate()
{
	if (!myCurrentNode)
	{
		Run();
		return false;
	}

	myCurrentNode->DoOperation();


	for (auto [transitionUID, transition] : myCurrentNode->myTransitions)
	{
		auto& conditions = transition->GetConditions();
		if (conditions.empty())
		{
			if (myCurrentState == AnimationState::Finished || !myCurrentNode->GetAnimation())
			{
				myCurrentNode->isRunning = false;
				myCurrentNode = transition->GetToNode();
				myCurrentNode->isRunning = true;
				return true;
			}
			continue;
		}
		else if (!transition->HasExitTime || myCurrentState == AnimationState::Finished)
		{
			bool conditionsMet = true;
			for (size_t i = 0; i < conditions.size(); i++)
			{
				if (!conditions[i].ConditionMet())
				{
					conditionsMet = false;
					break;
				}
			}

			if (conditionsMet)
			{
				myCurrentNode->isRunning = false;
				myCurrentNode = transition->GetToNode();
				myCurrentNode->isRunning = true;
				return true;
			}
		}
	}

	for (auto [nodeID, node] : myAnyStateNodes)
	{
		for (auto [transitionUID, transition] : node->myTransitions)
		{
			if (myCurrentNode->GetAnimation() == transition->GetToNode()->GetAnimation())
			{
				if (!node->myCanTransitionToItself) continue;
			}

			auto& conditions = transition->GetConditions();
			if (conditions.empty()) continue; //Add exit time here

			bool conditionsMet = true;
			for (size_t i = 0; i < conditions.size(); i++)
			{
				if (!conditions[i].ConditionMet())
				{
					conditionsMet = false;
					break;
				}
			}

			if (conditionsMet)
			{
				myCurrentNode->isRunning = false;
				myCurrentNode = transition->GetToNode();
				myCurrentNode->isRunning = true;
				return true;
			}
		}
	}

	return false;
}

void AnimationGraphInternal::RegenerateBoneMask(std::shared_ptr<Skeleton> aSkeleton)
{
	for (size_t i = 0; i < myBoneMask.size(); i++)
	{
		myBoneMask[i] = false;
	}

	std::stack<unsigned int> bonesToVisit;

	for (size_t i = 0; i < myBoneMaskRootIDs.size(); i++)
	{
		bonesToVisit.push(myBoneMaskRootIDs[i]);
	}

	while (!bonesToVisit.empty())
	{
		unsigned int currentBoneID = bonesToVisit.top();
		bonesToVisit.pop();

		if (myBoneMask[currentBoneID]) continue;

		myBoneMask[currentBoneID] = true;
		for (unsigned int bone : aSkeleton->bones[currentBoneID].childBoneIndices)
		{
			bonesToVisit.push(bone);
		}
	}
}

bool AnimationGraphInternal::Run()
{
	if (myEntryPoint)
	{
		myCurrentNode = myEntryPoint;
		myLastExecutedPath.clear();
		return myEntryPoint->Exec(0) != 0;
	}
	return false;
}

void AnimationGraphInternal::Reset()
{
	myCurrentState = AnimationState::Playing;
	myFrameIndex = 0;
	myNextFrameIndex = 1;
	myFramePercent = 0;
	myTimeSinceFrame = 0;
}

void AnimationGraphInternal::SetController(AnimationController* aController)
{
	myController = aController;
	myVariables = &aController->GetVariables();
}


void AnimationGraphInternal::BindErrorHandler(ScriptGraphErrorHandlerSignature&& aErrorHandler)
{
	myErrorDelegate = aErrorHandler;
}

void AnimationGraphInternal::UnbindErrorHandler()
{
	myErrorDelegate = nullptr;
}

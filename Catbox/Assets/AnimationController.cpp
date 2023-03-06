#include "stdafx.h"
#include "AnimationController.h"
#include "Editor\Windows\AnimationControllerWindow.h"
#include "Editor\ScriptGraph\AnimationController\AnimationControllerEditor.h"

#include "Editor\ScriptGraph\AnimationController\AnimationGraphSchema.h"
#include "Graphics\Animations\SkeletonData.h"
#include "ScriptGraphVariable.h"
#include "Animator.h"
#include <mutex>

std::mutex myGuardLock;

void AnimationController::SaveAsset(const char* aPath)
{
	if (myLayers.empty())
	{
		myLayers.insert({ "Default", AnimationGraphSchema::CreateAnimationGraph(this) });
	}
	Editor::GetInstance()->GetAnimationControllerEditor()->SaveGraph(this, myPath.string());
}

void AnimationController::LoadFromPath(const char* aPath)
{
	if (EDITORMODE)
	{
		Editor::GetInstance()->GetAnimationControllerEditor()->LoadGraph(this, myPath.string());
	}
	else
	{
		std::ifstream file(aPath);

		const std::string inGraph = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		file.close();

		AnimationGraphSchema::DeserializeAnimationGraph(this, inGraph);
	}
}

void AnimationController::DoubleClickAsset()
{
	Editor::GetInstance()->GetAnimationControllerEditor()->ClearSelectedObject();
	Editor::GetInstance()->GetAnimationControllerEditor()->SetController(this);
	Editor::GetInstance()->GetAnimationControllerEditor()->SetGraph(myLayers.begin()->second, myPath.string());
}

void AnimationController::RenderInProperties(std::vector<Asset*>& anAssetList)
{
	char nameInput[32] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	std::string emptyText = "##ACNameInput";
	if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	{
		myName = nameInput;
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}

	bool edit = false;
	std::shared_ptr<Skeleton> newSk = AssetRegistry::GetInstance()->AssetDropdownMenu(mySkeleton.get(), "Skeleton", edit);
	if (edit)
	{
		mySkeleton = newSk;
	}
}

void AnimationController::Reset()
{
	for (auto& layer : myLayers)
	{
		layer.second->Reset();
	}
}

std::shared_ptr<Animation> AnimationController::UpdateAnimations(Animator* anActor)
{
	const std::scoped_lock<std::mutex> lock(myGuardLock);

	auto& skeleton = anActor->GetSkeleton();
	std::array<Catbox::Matrix4x4<float>, BONE_LIMIT> boneMatrices;
	skeleton->Reset();
	
	bool checkBaseLayer = true;
	bool animate = true;
	for (auto& [id, layer] : myLayers)
	{
		//animationResults.emplace_back();
		if (!layer->isActive) continue;
		bool printName = false;
		if (layer->Animate())
		{
			printName = true;
			layer->Reset();
			auto& events = layer->GetCurrentNode()->GetEvents();
			for (size_t i = 0; i < events.size(); i++)
			{
				if (events[i].myFrame == layer->myFrameIndex)
				{
					events[i].Execute(anActor);
				}
			}

			if (!layer->GetCurrentNode()->GetAnimation())
			{
				continue;
			}
		}

		bool shouldBlend = true;
		auto currentNode = layer->GetCurrentNode();
		auto currentAnimation = currentNode->GetAnimation();
		
		if (checkBaseLayer && !currentAnimation) animate = false;
		checkBaseLayer = false;

		if (currentAnimation && layer->myCurrentState == AnimationState::Playing)
		{
			float timePerFrame = 1 / currentAnimation->framesPerSecond;
			layer->myTimeSinceFrame += deltaTime;
			if (layer->myTimeSinceFrame >= timePerFrame)
			{
				layer->myTimeSinceFrame = 0;

				if (++layer->myFrameIndex >= currentAnimation->frameCount)
				{
					if (!currentNode->myShouldLoop)
					{
						shouldBlend = false;
						layer->myFrameIndex = currentAnimation->frameCount - 1;
						layer->myCurrentState = AnimationState::Finished;
					}
					else layer->myFrameIndex = 1;
				}

				auto& events = currentNode->GetEvents();
				for (size_t i = 0; i < events.size(); i++)
				{
					if (events[i].myFrame == layer->myFrameIndex)
					{
						events[i].Execute(anActor);
					}
				}
			}


			layer->myFramePercent = layer->myTimeSinceFrame / timePerFrame;
			layer->myNextFrameIndex = layer->myFrameIndex + 1;

			if (layer->myNextFrameIndex >= currentAnimation->frameCount)
			{
				if (currentNode->myShouldLoop)
				{
					layer->myNextFrameIndex = (layer->myNextFrameIndex % currentAnimation->frameCount) + 1;
				}
				else
				{
					layer->myNextFrameIndex = layer->myFrameIndex;
				}
			}
		}

		
		if (currentAnimation)
		{
			if (shouldBlend)
			{
				skeleton->UpdateAnimationHierarchy(0, currentAnimation->frames[layer->myFrameIndex], currentAnimation->frames[layer->myNextFrameIndex], Catbox::Matrix4x4<float>(), layer->myFramePercent, layer->myBoneMask);
			}
			else
			{
				skeleton->UpdateAnimationHierarchyNoBlend(0, currentAnimation->frames[layer->myFrameIndex], Catbox::Matrix4x4<float>(), layer->myBoneMask);
			}
		}
	}

	auto ogTransform = anActor->GetTransform()->GetWorldTransformMatrix();

	for (size_t i = 0; i < skeleton->boneTransforms.size(); i++)
	{
		Catbox::Matrix4x4<float> m(skeleton->boneMatrices[i]);
		m = m.Transpose(m);

		m(4, 1) *= 0.01f;
		m(4, 2) *= 0.01f;
		m(4, 3) *= 0.01f;
		m = m * ogTransform;
		Vector3f scale = m.GetScaleFromMatrix(m);
		Vector3f rot = m.GetRotationFromMatrix(m);
		Vector3f pos = Vector3f(m(4, 1), m(4, 2), m(4, 3));

		skeleton->boneTransforms[i].SetWorldTransform(pos, rot, scale);
	}

	if (animate) 
	{
		for (size_t i = 0; i < skeleton->sharedData->bones.size(); i++)
		{
			skeleton->boneMatrices[i] *= skeleton->sharedData->bones[i].bindPoseInverse;
		}
	}

	return nullptr;
}

bool AnimationController::GetShouldLoop()
{
	return myCurrentLayer->GetCurrentNode()->myShouldLoop;
}

std::shared_ptr<AnimationController> AnimationController::MakeInstance()
{
	auto newInstance = std::make_shared<AnimationController>();
	newInstance->SetPath(myPath.string());

	std::ifstream file(myPath);
	const std::string inGraph = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();
	AnimationGraphSchema::DeserializeAnimationGraph(newInstance.get(), inGraph);

	newInstance->myRootAsset = this;
	return newInstance;
}

std::shared_ptr<AnimationGraph> AnimationController::AddLayer(std::string& aName)
{
	auto newGraph = AnimationGraphSchema::CreateAnimationGraph(this);
	myLayers.insert({ aName, newGraph });
	if (!myCurrentLayer)
	{
		myCurrentLayer = newGraph;
		myCurrentLayer->isActive = true;
	}

	for (size_t i = 0; i < newGraph->myBoneMask.size(); i++)
	{
		newGraph->myBoneMask[i] = true;
	}
	return newGraph;
}

void AnimationController::RemoveLayer(std::string& aName)
{
	myLayers.erase(aName);
}

void AnimationController::SetActiveLayer(const std::string& aLayer, bool aIsActive)
{
	if (myLayers.find(aLayer) != myLayers.end())
	{
		myLayers.at(aLayer)->isActive = aIsActive;
	}
	else
	{
		printerror("Tried to access layer " + aLayer + " but it does not exist!");
	}
}

void AnimationController::SetSkeleton(std::shared_ptr<Skeleton> aSkeleton)
{
	mySkeleton = aSkeleton;
	for (auto& layer : myLayers)
	{
		for (size_t i = 0; i < layer.second->myBoneMask.size(); i++)
		{
			layer.second->myBoneMask[i] = true;
		}
	}
}

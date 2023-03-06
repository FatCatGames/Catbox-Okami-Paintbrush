#include "stdafx.h"
#include "Animator.h"
#include "Graphics/Animations/SkeletonData.h"
#include "Graphics/Animations/Animation.h"
#include "Components/3D/ModelInstance.h"
#include "../Game/Managers/GameManager.h"
#include "../ComponentTools/ThreadPool.h"
#include "Assets\AnimationController.h"
#include "Editor\ScriptGraph\AnimationController\AnimationControllerEditor.h"


void Animator::Save(rapidjson::Value& /*aComponentData*/)
{
	int version = 1;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Int>("AnimationIndex", myAnimationIndex);
	wrapper.SaveString("Controller", myController ? myController->GetName().c_str() : "");
}

void Animator::Load(rapidjson::Value& aComponentData)
{
	if (!aComponentData.HasMember("Version")) return;
	int version = aComponentData["Version"].GetInt();


	myAnimationIndex = aComponentData["AnimationIndex"].GetInt();
	

	if (version > 0)
	{
		std::string controller = aComponentData["Controller"].GetString();
		if (!controller.empty())
		{
			myController = AssetRegistry::GetInstance()->GetAsset<AnimationController>(controller);
			if (myController)
			{
				myController = myController->MakeInstance();
				myCurrentAnimation = myController->GetCurrentAnimation();
			}
		}
	}
}

void Animator::GetChildrenBoneIDs(const int& aID, std::vector<int>& aOutVector)
{
	if (aID != mySkeleton->sharedData->GetRoot()->id)
	{
		aOutVector.push_back(aID);
		for (auto& bone : mySkeleton->sharedData->bones)
		{
			if (bone.parentBoneIndex == aID)
			{
				aOutVector.push_back(bone.id);
				GetChildrenBoneIDs(bone.id, aOutVector);
			}
		}
	}
}

void Animator::SetModelInstance(ModelInstance* aModelInstance)
{
	myModel = aModelInstance;
	if (aModelInstance)
	{
		mySkeleton = aModelInstance->GetSkeleton();
	}
	else
	{
		mySkeleton = nullptr;
	}
	OnModelChanged();
}


Animator::~Animator()
{
	if (EDITORMODE)
	{
		if (Editor::GetInstance()->GetAnimationControllerEditor()->myCurrentController == myController.get())
		{
			Editor::GetInstance()->GetAnimationControllerEditor()->ClearSelectedObject();
			Editor::GetInstance()->GetAnimationControllerEditor()->ClearController();
		}
	}
}

void Animator::Init()
{
	ModelInstance* mi = myGameObject->GetComponent<ModelInstance>();
	if (mi) SetModelInstance(mi);
}

void Animator::Awake()
{
	myShouldCull = true;
	if (myController)
	{
		myCurrentAnimation = myController->GetCurrentAnimation();
	}
}

void Animator::Update()
{
	if (!myController || !mySkeleton || !mySkeleton->sharedData) return;
	Animate();
}

void Animator::RunInEditor()
{
	if (!myController || !mySkeleton || !mySkeleton->sharedData) return;
	if (myGameObject->IsSelected(0))
	{
		Animate();
	}
}

void Animator::SetActiveLayer(const std::string& aLayer, bool aIsActive)
{
	if (myController)
	{
		myController->SetActiveLayer(aLayer, aIsActive);
	}
}

void Animator::SetBool(const std::string& aName, bool aValue)
{
	if (myController->GetVariables().count(aName) == 0)
	{
		printerror("No variable found by the name of " + aName);
		return;
	}
	myController->GetVariables().at(aName)->Data.SetData(aValue);
}

void Animator::SetFloat(const std::string& aName, float aValue)
{
	if (myController->GetVariables().count(aName) == 0)
	{
		printerror("No variable found by the name of " + aName);
		return;
	}
	myController->GetVariables().at(aName)->Data.SetData(aValue);
}

void Animator::SetInt(const std::string& aName, int aValue)
{
	if (myController->GetVariables().count(aName) == 0)
	{
		printerror("No variable found by the name of " + aName);
		return;
	}
	myController->GetVariables().at(aName)->Data.SetData(aValue);
}

void Animator::Animate()
{
	myController->UpdateAnimations(this);
}

void Animator::RenderInProperties(std::vector<Component*>& aComponentList)
{
	myHoveredIndex = -1;
	std::vector<Animator*> animatorList;
	for (auto& component : aComponentList)
	{
		Animator* anim = dynamic_cast<Animator*>(component);
		animatorList.push_back(anim);
		if (!anim->mySkeleton)
		{
			if (aComponentList.size() == 1) ImGui::Text("Selected model has no skeleton");
			else ImGui::Text("Some selected models have no skeleton");
			return;
		}
	}

	std::shared_ptr<Skeleton> sharedSkeleton = animatorList[0]->mySkeleton->sharedData;
	if (!sharedSkeleton)
	{
		ImGui::Text("No skeleton found");
		return;
	}

	int index = animatorList[0]->myAnimationIndex;
	for (auto& animator : animatorList)
	{
		if (animator->mySkeleton->sharedData != sharedSkeleton)
		{
			ImGui::Text("Selected animators do not share skeleton");
			return;
		}
		if (animator->myAnimationIndex != index)
		{
			index = -1;
			break;
		}
	}

	if (!PLAYMODE)
	{
		std::shared_ptr<AnimationController> sharedController = myController;
		for (auto& animator : animatorList)
		{
			if (animator->myController != sharedController)
			{
				sharedController = nullptr;
				break;
			}
		}

		bool edit = false;
		sharedController = AssetRegistry::GetInstance()->AssetDropdownMenu<AnimationController>(sharedController.get(), "Controller", edit);
		if (edit)
		{
			for (auto& animator : animatorList)
			{
				animator->myController = sharedController->MakeInstance();
				animator->myCurrentAnimation = sharedController->GetCurrentAnimation();
			}
		}
	}

	if (myController && ImGui::Button("View Controller"))
	{
		Editor::GetInstance()->GetAnimationControllerEditor()->SetSelectedObject(myGameObject);
		Editor::GetInstance()->GetAnimationControllerEditor()->SetController(myController.get());
	}
}

void Animator::OnModelChanged()
{
	myCurrentAnimation = nullptr;
	myController = nullptr;

	if (myModel->GetSkeleton())
	{
		if (myModel->GetSkeleton()->sharedData)
		{
			if (!myModel->GetSkeleton()->sharedData->allAnimations.empty())
			{
				myCurrentAnimation = myModel->GetSkeleton()->sharedData->allAnimations[0];
			}
		}
	}
}

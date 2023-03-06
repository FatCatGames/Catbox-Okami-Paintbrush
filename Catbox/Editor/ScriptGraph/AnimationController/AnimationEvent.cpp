#include "stdafx.h"
#include "AnimationEvent.h"
#include "Animator.h"
#include "Assets\AnimationController.h"

void AnimationEvent::Execute(Animator* anActor)
{
	if (myType == AnimationEventType::Prefab && myPrefabData.prefab)
	{
		auto newObject = InstantiatePrefab(myPrefabData.prefab);
		if (myPrefabData.worldSpace)
		{
			newObject->GetTransform()->SetWorldPos(myPrefabData.position);
		}
		else
		{
			if (myPrefabData.bone)
			{
				auto& boneTransform = anActor->GetSkeleton()->boneTransforms[myPrefabData.bone->id];
				Vector3f x = boneTransform.right() * myPrefabData.position.x;
				Vector3f y = boneTransform.up() * myPrefabData.position.y;
				Vector3f z = boneTransform.forward() * myPrefabData.position.z;
				newObject->GetTransform()->SetWorldPos(boneTransform.worldPos() + x + y + z);
				newObject->GetTransform()->Translate(myPrefabData.position);
			}
			else
			{
				newObject->GetTransform()->SetWorldPos(anActor->GetTransform()->worldPos());
				newObject->GetTransform()->Translate(myPrefabData.position);
			}
		}
	}
	else if (myType == AnimationEventType::PlaySound)
	{

	}
}

static std::string searchTerm;

void AnimationEvent::RenderInProperties(AnimationController* aController)
{
	ImGui::Text(("Event " + std::to_string(myID)).c_str());
	ImGui::Spacing();
	ImGui::SetNextItemWidth(100);
	int frame = myFrame;
	if (Catbox::InputInt(("Frame##" + std::to_string(myID)).c_str(), &frame, 1, 10))
	{
		if (frame < 0) frame = 0;
		myFrame = frame;
	}

	const char* types[]
	{
		"Prefab",
		"Sound"
	};

	int typeIndex = static_cast<int>(myType);
	ImGui::SetNextItemWidth(100);
	if (ImGui::Combo(("Event Type##" + std::to_string(myID)).c_str(), &typeIndex, types, 2))
	{
		myType = (AnimationEventType)typeIndex;
	}


	ImGui::SetNextItemWidth(100);
	if (myType == AnimationEventType::Prefab)
	{
		bool edit = false;
		auto newPrefab = AssetRegistry::GetInstance()->AssetDropdownMenu<Prefab>(myPrefabData.prefab.get(), ("Prefab##" + std::to_string(myID)).c_str(), edit);
		if (edit)
		{
			myPrefabData.prefab = newPrefab;
		}

		Catbox::Checkbox(("World Space##" + std::to_string(myID)).c_str(), &myPrefabData.worldSpace);
		if (myPrefabData.worldSpace) 
		{
			Catbox::DragFloat3(("Position##" + std::to_string(myID)).c_str(), &myPrefabData.position);
		}
		else
		{
			Catbox::DragFloat3(("Offset##" + std::to_string(myID)).c_str(), &myPrefabData.position);
			
			auto& skeleton = aController->GetSkeleton();
			int index = 0;
			for (size_t i = 0; i < skeleton->bones.size(); i++)
			{
				if (myPrefabData.bone)
				{
					if (skeleton->bones[i].id == myPrefabData.bone->id) index = i+1;
				}
			}

			if (ImGui::BeginCombo(("Bone##" + std::to_string(myID)).c_str(), index == 0 ? "None" : skeleton->boneNames[index-1].c_str()))
			{
				if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
					ImGui::SetKeyboardFocusHere(0);
				ImGui::InputText("Search", &searchTerm);

				if (ImGui::Selectable("None", false))
				{
					myPrefabData.bone = nullptr;
				}

				for (int i = 0; i < skeleton->boneNames.size(); i++)
				{
					if (Catbox::ToLowerString(skeleton->boneNames[i]).find(Catbox::ToLowerString(searchTerm)) != std::string::npos)
					{
						if (ImGui::Selectable(skeleton->boneNames[i].c_str(), false))
						{
							myPrefabData.bone = &skeleton->bones[i];
						}
					}
				}

				ImGui::EndCombo();
			}
		}
	}
}

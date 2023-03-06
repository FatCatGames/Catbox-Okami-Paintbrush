#include "stdafx.h"
#include "SceneHierarchyWindow.h"
#include "Assets/Scene.h"
#include "Editor/Windows/PropertiesWindow.h"
#include "CommonUtilities\UtilityFunctions.hpp"
#include "Editor/Commands/EditorCommandHandler.h"
#include "Editor/Commands/CreateDeleteObjectCommand.h"
#include "CommonUtilities/InputHandler.h"
#include <ImGui/imgui_stdlib.h>
#include "Components/3D/ModelInstance.h"
#include "Assets\Model.h"

void SceneHierarchyWindow::ItemClickedCheck(GameObject* anObject)
{
	if (Editor::GetInstance()->GetGizmos().IsUsing()) return;
	if (ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen() && ((mySelectedObjects.empty() && ImGui::IsItemClicked()) || (!mySelectedObjects.empty() && ImGui::IsMouseReleased(0))))
	{
		if (Input::GetKeyHeld(KeyCode::SHIFT) && !mySelectedObjects.empty())
		{
			int endIndex = -1;
			for (int i = 0; i < myRenderedObjects.size(); i++)
			{
				if (endIndex == -1 && myRenderedObjects[i] == anObject)
				{
					endIndex = i;
				}


				if (mySelectedObjectIndex != -1 && endIndex != -1)
				{
					int startIndex = mySelectedObjectIndex;
					if (endIndex < startIndex)
					{
						std::swap(startIndex, endIndex);
					}

					ClearSelectedObjects();

					for (int n = startIndex; n <= endIndex; n++)
					{
#ifdef _DEBUG 
						if (n >= myRenderedObjects.size()) break;
#endif
						myRenderedObjects[n]->AddSelected();
						AddSelectedObject(myRenderedObjects[n]);
						Editor::GetInstance()->GetGizmos().AddSelectedObject(myRenderedObjects[n]);
					}
					return;
				}
			}

		}
		else if (anObject->IsSelected(0))
		{
			if (Input::GetKeyHeld(KeyCode::CTRL))
			{
				anObject->Deselect();
				RemoveSelectedObject(anObject);
				Editor::GetInstance()->GetGizmos().RemoveSelectedObject(anObject);
			}
			else
			{
				anObject->Select();
				ClearSelectedObjects();
				Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
				AddSelectedObject(anObject);
				Editor::GetInstance()->GetGizmos().AddSelectedObject(anObject);
			}
		}
		else
		{
			if (Input::GetKeyHeld(KeyCode::CTRL))
			{
				anObject->AddSelected();
			}
			else
			{
				anObject->Select();
				ClearSelectedObjects();
				Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
			}

			AddSelectedObject(anObject);
			Editor::GetInstance()->GetGizmos().AddSelectedObject(anObject);
		}
	}

	if (ImGui::IsItemClicked(1))
	{
		anObject->SetSelected(1, true);
		ImGui::OpenPopup("gameobject_menu");
	}


	if (anObject->IsSelected(1))
	{
		if (ImGui::BeginPopup("gameobject_menu"))
		{
			if (!myPopupRendered)
			{
				myPopupRendered = true;
				if (ImGui::MenuItem("Add GameObject"))
				{
					Instantiate(anObject->GetTransform());
				}
				if (ImGui::MenuItem("Duplicate"))
				{
					GameObject* copy = anObject->Duplicate();

					Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
					ClearSelectedObjects();
					RemoveSelectedObject(anObject);
					AddSelectedObject(copy);
					Editor::GetInstance()->GetGizmos().AddSelectedObject(copy);
					copy->Select();
				}
				if (ImGui::MenuItem("Delete"))
				{
					EditorCommandHandler::AddCommand(std::make_shared<CreateDeleteObjectCommand>(anObject, CreateDeleteObjectCommand::Operation::Delete));
					anObject->Destroy();
					Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
					ClearSelectedObjects();
				}
				if (ImGui::MenuItem("Make prefab"))
				{
					auto path = Catbox::SaveFile("Prefab(*.prefab)\0*.prefab\0");
					if (!path.empty())
					{
						GameObject temp;
						auto prefab = CreateAsset<Prefab>(path);
						anObject->SetPrefab(nullptr);

						temp.LoadObjectData(anObject->GetObjectData());
						temp.SaveAsset(path.c_str());
						anObject->SetPrefab(prefab);
					}
				}
			}
			ImGui::EndPopup();
		}
		else
		{
			anObject->SetSelected(1, false);
		}
	}
}

bool SceneHierarchyWindow::ScrollCheck(GameObject* anObject)
{
	if (anObject->GetObjectInstanceID() == myObjectIDToScrollTo)
	{
		ImGui::SetScrollHereY();
		myObjectIDToScrollTo = -1;
		anObject->myIsOpenedInHierarchy = true;
		Transform* parent = anObject->GetTransform()->GetParent();
		while (parent)
		{
			parent->GetGameObject()->myIsOpenedInHierarchy = true;
			parent = parent->GetParent();
		}
		return true;
	}

	bool done = false;
	for (auto& child : anObject->GetTransform()->GetChildren())
	{
		done |= (ScrollCheck(child->GetGameObject()));
		if (done) return true;
	}

	return false;
}

void SceneHierarchyWindow::RenderObject(GameObject* anObject)
{
	myRenderedObjects.push_back(anObject);
	if (anObject->GetObjectInstanceID() == myObjectIDToScrollTo)
	{
		ImGui::SetScrollHereY();
		myObjectIDToScrollTo = -1;
	}

	bool active = anObject->IsActive();
	bool selected = anObject->IsSelected(0) || anObject->IsSelected(1);
	if (selected)
	{
		int newIndex = static_cast<int>(myRenderedObjects.size() - 1);
		if (mySelectedObjectIndex < newIndex)
		{
			mySelectedObjectIndex = newIndex;
		}
	}
	bool leaf = anObject->GetTransform()->GetChildren().size() == 0 || !mySearchString.empty();

	if (!selected && !active) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

	ImGuiTreeNodeFlags flags =
		(selected ? ImGuiTreeNodeFlags_Selected : 0) |
		(leaf ? ImGuiTreeNodeFlags_Leaf : 0) |
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanFullWidth;

	ImGui::SetNextItemOpen(anObject->myIsOpenedInHierarchy);

	auto rootObject = anObject->GetTransform()->GetRootParent()->GetGameObject();
	bool popColor = false;
	auto prefab = rootObject->GetPrefab();
	bool shouldOpen = false;

	if (rootObject->GetPrefabMissing() || anObject->GetPrefabMissing())
	{
		popColor = true;
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		std::string name = anObject->GetName() + " (missing prefab)" + "##" + std::to_string(anObject->GetObjectInstanceID());
		shouldOpen = ImGui::TreeNodeEx(name.c_str(), flags);
	}
	else
	{
		if (prefab || anObject->GetPrefab())
		{
			popColor = true;
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.6f, 1, 1));
		}

		std::string name = anObject->GetName() + "##" + std::to_string(anObject->GetObjectInstanceID());
		shouldOpen = ImGui::TreeNodeEx(name.c_str(), flags);
	}

	if (popColor)
	{
		ImGui::PopStyleColor();
	}

	if (mySearchString.empty())
	{
		anObject->myIsOpenedInHierarchy = shouldOpen;
	}

	if (!selected && !active) ImGui::PopStyleColor();

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		if (mySelectedObjects.size() > 1)
		{
			myDraggedObjects = mySelectedObjects;
		}
		else if (mySelectedObjects.size() == 1 && mySelectedObjects[0] == anObject)
		{
			myDraggedObjects.push_back(anObject);
		}
		if (!anObject->IsSelected(0))
		{
			myDraggedObjects.push_back(anObject);
		}
		ImGui::SetDragDropPayload("OBJECT_DRAG", &anObject->GetObjectInstanceID(), sizeof(int));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_DRAG"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int));
			//int payload_n = *(const int*)payload->Data;

			for (auto& object : myDraggedObjects)
			{
				bool failed = false;
				Transform* parent = anObject->GetTransform()->GetParent();
				if (anObject->GetPrefab()) failed = true;

				while (parent != nullptr && !failed)
				{
					if (parent == object->GetTransform()) failed = true;
					if (parent->GetGameObject()->GetPrefab()) failed = true;
					parent = parent->GetParent();
				}

				if (!failed)
				{
					bool openPopup = false;
					ModelInstance* model = anObject->GetComponent<ModelInstance>();
					if (model)
					{
						std::shared_ptr<SkeletonInstance> skeleton = model->GetSkeleton();
						if (skeleton)
						{
							openPopup = true;
							mySelectedParent = anObject;
						}
					}
					if (openPopup)
					{
						myRenderParentingPopup = true;

					}
					else
					{
						for (auto& object : myDraggedObjects)
						{
							object->GetTransform()->SetParent(anObject->GetTransform());
						}
						myDraggedObjects.clear();
						myDragIndex = 0;
					}
				}
			}
		}



		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CREATE_OBJECT"))
		{
			std::filesystem::path aPath = *(std::filesystem::path*)payload->Data;
			auto newObject = AssetRegistry::GetInstance()->AutoMakeGameObject(std::filesystem::path(aPath));
			if (newObject)
			{
				newObject->GetTransform()->SetParent(anObject->GetTransform());
			}
		}

		ImGui::EndDragDropTarget();
	}
	else
	{
		ItemClickedCheck(anObject);
	}

	if (anObject->myIsOpenedInHierarchy)
	{
		if (mySearchString.empty())
		{
			for (auto& child : anObject->GetTransform()->GetChildren())
			{
				RenderObject(child->GetGameObject());
			}
		}
		ImGui::TreePop();
	}
	else if (!anObject->myIsOpenedInHierarchy && !mySearchString.empty())
	{
		ImGui::TreePop();
	}
	else if (myObjectIDToScrollTo != -1)
	{
		ScrollCheck(anObject);
	}
}

void SceneHierarchyWindow::Render()
{
	if (!BeginWindow("Hierarchy", false)) return;

	if (myScene)
	{
		myPopupRendered = false;
		myRenderedObjects.clear();
		ImGui::InputText("Search", &mySearchString);
		if (ImGui::IsItemEdited())
		{
			myShouldRenderObjects.clear();
			for (auto& object : myScene->myRootObjects)
			{
				SearchChildren(myShouldRenderObjects, object);
			}
		}
		if (mySearchString.empty())
		{
			myShouldRenderObjects = myScene->myRootObjects;
		}
		for (auto& object : myShouldRenderObjects)
		{
			RenderObject(object);
		}

		if (!myDraggedObjects.empty())
		{
			ImGui::Button("Clear parent");
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_DRAG"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					//int payload_n = *(const int*)payload->Data;
					for (auto& object : myDraggedObjects)
					{
						object->GetTransform()->SetParent(nullptr);
					}
					myDraggedObjects.clear();
					myDragIndex = 0;
				}


				ImGui::EndDragDropTarget();
			}
		}

		if (!myPopupRendered)
		{
			if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered())
			{
				ImGui::OpenPopup("gameobject_menu");
				if (ImGui::BeginPopup("gameobject_menu"))
				{
					ImGui::EndPopup();
				}
			}

			if (ImGui::BeginPopupContextWindow(0, 1))
			{
				if (ImGui::MenuItem("Add gameobject"))
				{
					GameObject* newObject = Instantiate();
					EditorCommandHandler::AddCommand(std::make_shared<CreateDeleteObjectCommand>(newObject, CreateDeleteObjectCommand::Operation::Create));
				}
				ImGui::EndPopup();
			}
		}

		if (myRenderParentingPopup)
		{
			ImGui::OpenPopup("setparent_menu", ImGuiPopupFlags_AnyPopupLevel);
			ImGui::BeginPopup("setparent_menu");

			bool hovered = false;
			bool parentToObject = false;
			ModelInstance* model = mySelectedParent->GetComponent<ModelInstance>();
			std::shared_ptr<SkeletonInstance> skeleton = model->GetSkeleton();
			if (ImGui::MenuItem(mySelectedParent->GetName().c_str()))
			{
				parentToObject = true;
				myRenderParentingPopup = false;
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)) hovered = true;
			}
			for (size_t i = 0; i < skeleton->sharedData->bones.size(); i++)
			{
				if (ImGui::MenuItem(skeleton->sharedData->boneNames[i].c_str()))
				{
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)) hovered = true;

					for (auto& draggedObject : myDraggedObjects)
					{
						if (draggedObject == mySelectedParent) continue;
						auto objTransform = draggedObject->GetTransform();

						objTransform->SetParent(&skeleton->boneTransforms[i]);
						mySelectedParent->GetTransform()->AddChild(*objTransform);
						objTransform->myBoneParentId = skeleton->sharedData->bones[i].parentBoneIndex;
						objTransform->Reset();
					}
					mySelectedParent = nullptr;
					myDraggedObjects.clear();
					myDragIndex = 0;
					myRenderParentingPopup = false;
					break;
				}
			}
			ImGui::EndPopup();

			/*
			if (ImGui::IsMouseClicked(0) && !hovered)
				{
					myRenderParentingPopup = false;
				}
			*/

			if (parentToObject)
			{
				for (auto& object : myDraggedObjects)
				{
					object->GetTransform()->SetParent(mySelectedParent->GetTransform());
				}
				myDraggedObjects.clear();
				mySelectedParent = nullptr;
				myDragIndex = 0;
			}
		}

	}
	ImGui::End();
}

void SceneHierarchyWindow::SetScene(Scene* aScene)
{
	myScene = aScene;
	Reset();
	ClearSelectedObjects();
}

void SceneHierarchyWindow::ClearSelectedObjects()
{
	mySelectedObjects.clear();
	Editor::GetInstance()->GetGizmos().ClearSelectedObjects();
	mySelectedObjectIndex = -1;
	Editor::GetInstance()->SetCanDeleteObjects(true);
}

void SceneHierarchyWindow::AddSelectedObject(GameObject* anObject)
{
	mySelectedObjects.push_back(anObject);
	Editor::GetInstance()->SetCanDeleteObjects(true);
}

void SceneHierarchyWindow::RemoveSelectedObject(GameObject* anObject)
{
	for (size_t i = 0; i < mySelectedObjects.size(); i++)
	{
		if (mySelectedObjects[i] == anObject)
		{
			mySelectedObjects.erase(mySelectedObjects.begin() + i);
		}
	}

	mySelectedObjectIndex = -1;
}

void SceneHierarchyWindow::FocusOnObject(GameObject* anObject)
{
	myObjectIDToScrollTo = anObject->GetObjectInstanceID();
}

void SceneHierarchyWindow::Reset()
{
	myShouldRenderObjects.clear();
}

void SceneHierarchyWindow::SearchChildren(std::vector<GameObject*>& aGameObjectRefrence, GameObject* aObject)
{
	if (Catbox::ToLowerString(aObject->GetName()).find(Catbox::ToLowerString(mySearchString)) != std::string::npos)
	{
		aGameObjectRefrence.push_back(aObject);
	}
	for (auto& object : aObject->GetTransform()->GetChildren())
	{
		SearchChildren(aGameObjectRefrence, object->GetGameObject());
	}
}

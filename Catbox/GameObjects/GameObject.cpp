#include "stdafx.h"
#include "GameObject.h"
#include "../Components/Physics/Collisions/Collider.h"
#include "ComponentHandler.h"
#include "ComponentTools/SceneManager.h"
#include "Editor/Commands/EditorCommandHandler.h"
#include "Editor/Commands/CreateDeleteObjectCommand.h"
#include <Editor\Commands\TransformCommand.h>
#include <ImGui/imgui_stdlib.h>
#include "3D/Camera.h"
#include "Graphics\Animations\SkeletonData.h"
#include "Assets\Prefab.h"

GameObject::GameObject()
{
	myTransform.myGameObject = this;
}

GameObject::~GameObject()
{
	for (auto component : myComponents)
	{
		if (myInstanceID != -1)
		{
			component->OnDestroy();
		}
		delete component;
		component = nullptr;
	}
	myComponents.clear();
}

void GameObject::DeleteObject()
{
	Engine::GetInstance()->RemoveGameObject(myInstanceID);
	if (!myTransform.GetParent())
	{
		Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->RemoveRootObject(this);
	}

	auto& children = myTransform.GetChildren();
	for (int i = children.size(); i > 0; i--)
	{
		children[i - 1]->GetGameObject()->DeleteObject();
	}

	myTransform.myChildren.clear();

	if (EDITORMODE)
	{
		Deselect();
	}

	delete this;
}

void GameObject::Destroy()
{
	myShouldDestroy = true;
}

#define OnCollisionEvent(F) void GameObject::F(Collider* someColission){for (Component* c : myComponents){if (!c->myIsEnabled) continue;c->F(someColission);}}

OnCollisionEvent(OnCollisionEnter)
OnCollisionEvent(OnTriggerEnter)
OnCollisionEvent(OnCollisionStay)
OnCollisionEvent(OnCollisionExit)
OnCollisionEvent(OnOverlapBegin)
OnCollisionEvent(OnOverlap)
OnCollisionEvent(OnOverlapEnd)


void GameObject::SetObjectInstanceID(int anID)
{
	myInstanceID = anID;
	myNamePlusId = myName + '#' + std::to_string(myInstanceID);
}

const int& GameObject::GetObjectInstanceID()
{
	return myInstanceID;
}

Transform* GameObject::GetTransform()
{
	return &myTransform;
}

bool GameObject::operator==(GameObject& aRhs)
{
	return aRhs.GetObjectInstanceID() == myInstanceID;
}


bool GameObject::IsActive()
{
	bool active = myIsActive;
	Transform* parent = myTransform.GetParent();
	while (active && parent != nullptr)
	{
		active = parent->GetGameObject()->IsActive();
		parent = parent->GetParent();
	}

	return active;
}

void GameObject::SetActive(bool aIsActiveFlag)
{
	myIsActive = aIsActiveFlag;
	if (!myHasStarted) Start();
	SetComponentsEnabled(aIsActiveFlag);
}


void GameObject::SetTag(const char* aTag)
{
	myTag = aTag;
}

void GameObject::SetName(const char* aName)
{
	myName = aName;
	myNamePlusId = myName + '#' + std::to_string(myInstanceID);
}

const std::string& GameObject::GetTag()
{
	return myTag;
}

const std::string& GameObject::GetName()
{
	return myName;
}

void GameObject::RemoveComponent(Component* aComponent)
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i] == aComponent)
		{
			myComponents[i]->OnDestroy();
			delete myComponents[i];
			myComponents.erase(myComponents.begin() + i);
			return;
		}
	}
}

GameObject* GameObject::Duplicate()
{
	GameObject* copy = Instantiate();
	copy->GetTransform()->SetParent(myTransform.GetParent());
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.CreateDocument();
	std::string objectDataStr = SaveObjectData();
	copy->LoadObjectData(objectDataStr);
	EditorCommandHandler::AddCommand(std::make_shared<CreateDeleteObjectCommand>(copy, CreateDeleteObjectCommand::Operation::Create));
	return copy;
}

std::string GameObject::GetObjectData()
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.CreateDocument();
	return SaveObjectData();
}



void GameObject::RenderInProperties(std::vector<Asset*>& anAssetList)
{
	std::vector<GameObject*> objectList;
	std::vector<Transform*> transformList;
	for (Asset* asset : anAssetList)
	{
		GameObject* obj = dynamic_cast<GameObject*>(asset);
		objectList.push_back(obj);
		transformList.push_back(obj->GetTransform());
	}

	bool active = objectList[0]->myIsActive;
	if (Catbox::Checkbox("##Active", &active))
	{
		for (GameObject* object : objectList)
		{
			object->SetActive(active);
		}
	}
	

	ImGui::SameLine();

	char nameInput[64] = "-";
	if (objectList.size() == 1)
	{
		for (size_t i = 0; i < objectList[0]->myName.size(); i++)
		{
			nameInput[i] = objectList[0]->myName[i];
		}
	}

	std::string emptyText = "##NameInput" + std::to_string(myInstanceID);
	if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	{
		for (GameObject* object : objectList)
		{
			object->myName = nameInput;
		}
	}

	if (Catbox::Checkbox("Cull", &myShouldCull))
	{
		for (GameObject* object : objectList)
		{
			object->myShouldCull = myShouldCull;
		}
	}

	ImGui::Separator();

	Transform::RenderInProperties(transformList);

	GameObject* prefabHolder = GetPrefab() ? this : nullptr;
	if (myTransform.GetRootParent()->GetGameObject()->GetPrefab())
	{
		prefabHolder = myTransform.GetRootParent()->GetGameObject();
	}
	if (prefabHolder)
	{
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("Cannot edit components of prefab-linked object.");
		if (ImGui::Button("Unlink prefab"))
		{
			prefabHolder->SetPrefab(nullptr);
		}
	}
	else
	{
		std::vector<Component*> sharedComponents = objectList[0]->myComponents;

		for (GameObject* object : objectList)
		{
			for (size_t i = 0; i < sharedComponents.size();)
			{
				bool found = false;
				for (size_t n = 0; n < object->myComponents.size(); n++)
				{
					if (typeid(*object->myComponents[n]) == typeid(*sharedComponents[i]))
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					sharedComponents.erase(sharedComponents.begin() + i);
				}
				else ++i;
			}
		}


		for (auto& component : sharedComponents)
		{
			std::string name = component->GetName() + "##" + std::to_string(myInstanceID);

			ImGui::Spacing();
			ImGui::Separator();

			bool expanded = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen);
			if (ImGui::IsItemClicked(1))
			{
				ImGui::OpenPopup("component_menu");
			}

			if (ImGui::BeginPopup("component_menu"))
			{
				if (ImGui::MenuItem("Remove component"))
				{
					auto& componentType = typeid(*component);
					for (GameObject* object : objectList)
					{
						for (size_t i = 0; i < object->myComponents.size();)
						{
							if (typeid(*object->myComponents[i]) == componentType)
							{
								object->RemoveComponent(object->myComponents[i]);
								break;
							}
							else ++i;
						}
					}
					ImGui::EndPopup();
					if (expanded) ImGui::TreePop();
					continue;
				}
				ImGui::EndPopup();
			}

			if (expanded)
			{
				ImGui::Spacing();

				auto& componentType = typeid(*component);
				std::vector<Component*> componentsOfType;

				for (GameObject* object : objectList)
				{
					for (size_t i = 0; i < object->myComponents.size(); i++)
					{
						if (typeid(*object->myComponents[i]) == componentType)
						{
							componentsOfType.push_back(object->myComponents[i]);
							break;
						}
					}
				}

				component->RenderInProperties(componentsOfType);
				ImGui::Spacing();
				ImGui::TreePop();
			}
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			ImGui::InputText("Search", &mySearchComponent);
			for (auto& [key, AddCompFunction] : ComponentHandler::GetAddComponentFunctions())
			{
				if (Catbox::ToLowerString(key).find(Catbox::ToLowerString(mySearchComponent)) != std::string::npos)
				{
					if (ImGui::Button(key.c_str()))
					{
						for (GameObject* object : objectList)
						{
							AddCompFunction(object);
						}

						ImGui::CloseCurrentPopup();
					}
				}
			}
			ImGui::EndPopup();
		}
	}
}

void GameObject::SaveAsset(const char* aPath)
{
	if (!EDITORMODE) return;
	myPath = aPath;
	myPathStr = myPath.string();
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& document = wrapper.CreateDocument();
	std::string objectDataStr = SaveObjectData();
	auto& objectData = wrapper.StringToJson(objectDataStr);
	document.AddMember("Data", objectData, document.GetAllocator());
	wrapper.SaveDocument(myPath.string().c_str());
}

void GameObject::LoadFromPath(const char* aPath)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& doc = wrapper.LoadDocument(aPath);
	LoadObjectData(doc["Data"].GetObj());
}

void GameObject::SetSkeleton(std::shared_ptr<SkeletonInstance> aSkeleton)
{
	mySkeleton = aSkeleton;
}

Transform* GameObject::GetBone(const std::string& aBoneName)
{
	if (!mySkeleton) return nullptr;
	if (!mySkeleton->sharedData) return nullptr;
	if (mySkeleton->sharedData->boneNameToIndex.count(aBoneName) != 0)
	{
		return &mySkeleton->boneTransforms[mySkeleton->sharedData->boneNameToIndex[aBoneName]];
	}
	printerror("GetBone() failed: " + aBoneName + " not found on " + myName);
	return nullptr;
}


void GameObject::SetPrefab(std::shared_ptr<Prefab> aPrefab)
{
	myPrefab = aPrefab;
	myPrefabMissing = false;
	myPrefabName = aPrefab ? aPrefab->GetName() : "";
}


void GameObject::SetComponentsEnabled(bool aIsEnabled)
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		aIsEnabled ? myComponents[i]->OnEnable() : myComponents[i]->OnDisable();
	}

	for (auto& child : myTransform.GetChildren())
	{
		child->myGameObject->SetComponentsEnabled(aIsEnabled);
	}
}

std::string GameObject::SaveObjectData()
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = RapidJsonWrapper::GetInstance()->GetDocument().GetAllocator();
	auto& array = *wrapper.GetTarget();
	auto object = wrapper.CreateObject();
	wrapper.SetTarget(object);
	wrapper.SaveString("Name", myName.c_str());
	wrapper.SaveValue<DataType::Bool>("Active", myIsActive);
	wrapper.SaveValue<DataType::Bool>("Opened", myIsOpenedInHierarchy);
	wrapper.SaveValue<DataType::Bool>("Cull", myShouldCull);
	myTransform.Save();

	if (myPrefab || myPrefabMissing)
	{
		wrapper.SaveString("Prefab", myPrefabName.c_str());
	}
	else
	{
		auto components = wrapper.CreateArray();
		components.SetArray();
		for (auto& component : myComponents)
		{
			wrapper.SetTarget(components);

			auto componentObj = wrapper.CreateObject();
			wrapper.SetTarget(componentObj);

			const char* typeidName = typeid(*component).name();
			const char* componentName = ComponentHandler::GetComponentName(typeidName).c_str();
			wrapper.SaveString("ComponentType", componentName);

			component->Save(componentObj);
			components.PushBack(componentObj, alloc);
		}

		object.AddMember("Components", components, alloc);
	}

	if (array.IsArray())
	{
		array.PushBack(object, alloc);
	}

	wrapper.SetTarget(object);
	return wrapper.JsonToString(object);
}

void GameObject::LoadComponents(rapidjson::Value& anObject)
{
	auto componentsArray = anObject["Components"].GetArray();
	for (auto& componentData : componentsArray)
	{
		string componentName = componentData["ComponentType"].GetString();
		Component* newComponent = ComponentHandler::AddComponentToObjectByName(this, componentName);
		if (newComponent)
		{
			newComponent->Load(componentData);
		}
		else
		{
			printerror(myName + " tried to load componet " + componentName + " but it has been deleted or renamed");
		}
	}
}

void GameObject::LoadObjectData(rapidjson::Value& anObject)
{
	myHasFinishedLoading = false;
	for (auto component : myComponents)
	{
		RemoveComponent(component);
	}

	myComponents.clear();
	myName = anObject["Name"].GetString();


	if (anObject.HasMember("Active")) myIsActive = anObject["Active"].GetBool();
	if (anObject.HasMember("Opened")) myIsOpenedInHierarchy = anObject["Opened"].GetBool();
	if (anObject.HasMember("Cull")) myShouldCull = anObject["Cull"].GetBool();

	if (anObject.HasMember("Prefab")) //Object has a link to a prefab
	{
		myPrefabName = anObject["Prefab"].GetString();
		auto prefab = AssetRegistry::GetInstance()->GetAsset<Prefab>(myPrefabName.c_str());

		if (prefab)
		{
			myPrefab = prefab;
			auto& prefabJson = RapidJsonWrapper::GetInstance()->LoadDocument(prefab->GetPath().string().c_str());
			myTransform.Load(anObject["Transform"].GetObj());
			myTransform.LoadChildren(prefabJson["Data"]["Transform"].GetObj());
			LoadComponents(prefabJson["Data"]);


		}
		else
		{
			myPrefabMissing = true;
			printerror(myName + " missing prefab link!");
		}
	}
	else //Load raw object data
	{
		myTransform.Load(anObject["Transform"].GetObj());
		myTransform.LoadChildren(anObject["Transform"].GetObj());
		LoadComponents(anObject);
	}

	if (mySkeleton)
	{
		for (size_t i = 0; i < myChildrenToBones.size(); i++)
		{
			myChildrenToBones[i]->myParent = &mySkeleton->boneTransforms[myChildrenToBones[i]->myBoneParentId];
			mySkeleton->boneTransforms[myChildrenToBones[i]->myBoneParentId].AddChild(*myChildrenToBones[i]);
			//myTransform.AddChild(*myChildrenToBones[i]);
		}

		myChildrenToBones.clear();
	}
	myHasFinishedLoading = true;

	if (PLAYMODE)
	{
		for (auto component : myComponents)
		{
			component->Awake();
		}
	}
	for (auto component : myComponents)
	{
		component->OnObjectFinishedLoading();
	}
}

void GameObject::LoadObjectData(std::string anObject)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& newDoc = wrapper.StringToJson(anObject);
	LoadObjectData(newDoc);
}


void GameObject::Update()
{
	bool shouldBeCulled = false;
	myAliveTime += Engine::GetInstance()->GetDeltaTime();


	if (!myIsActive) return;

	if (myShouldCull) 
	{
		if (EDITORMODE)
		{
			if (PLAYMODE)
			{
				shouldBeCulled = !Engine::GetInstance()->GetMainCamera()->IsInsideFrustum(&myTransform);
			}
			else
			{
				shouldBeCulled = !Editor::GetInstance()->GetEditorCamera().GetCamera()->IsInsideFrustum(&myTransform);
			}
		}
		else
		{
			if (Engine::GetInstance()->GetMainCamera())
			{
				shouldBeCulled = !Engine::GetInstance()->GetMainCamera()->IsInsideFrustum(&myTransform);
			}
		}
	}

	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i]->myIsEnabled)
		{
			if (!myComponents[i]->myShouldCull)
			{
				if (PLAYMODE) myComponents[i]->Update();
				else myComponents[i]->RunInEditor();
			}
			else
			{
				if (!shouldBeCulled)
				{
					if (PLAYMODE) myComponents[i]->Update();
					else myComponents[i]->RunInEditor();
				}
			}
		}
	}

	auto& children = myTransform.GetChildren();
	for (int i = 0; i < children.size();)
	{
		if (children[i]->myGameObject->myShouldDestroy)
		{
			children[i]->myGameObject->DeleteObject();
			continue;
		}
		children[i++]->myGameObject->Update();
	}
}

void GameObject::Start()
{
	if (myHasStarted || !myIsActive) return;
	for (size_t i = 0; i < myTransform.myChildren.size(); i++)
	{
		myTransform.myChildren[i]->myGameObject->Start();
	}

	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i]->myHasStarted) continue;
		myComponents[i]->Awake();
		myComponents[i]->myHasStarted = true;
	}
	myHasStarted = true;
}



void GameObject::FixedUpdate()
{
	bool shouldBeCulled = false;
	myAliveTime += Engine::GetInstance()->GetDeltaTime();


	if (!myIsActive) return;

	if (EDITORMODE)
	{
		if (PLAYMODE)
		{
			if (Engine::GetInstance()->GetMainCamera())
			{
				shouldBeCulled = !Engine::GetInstance()->GetMainCamera()->IsInsideFrustum(&myTransform);
			}
		}
		else
		{
			if (Editor::GetInstance()->GetEditorCamera().GetCamera())
			{
				shouldBeCulled = !Editor::GetInstance()->GetEditorCamera().GetCamera()->IsInsideFrustum(&myTransform);
			}
		}
	}
	else
	{
		if (Engine::GetInstance()->GetMainCamera())
		{
			shouldBeCulled = !Engine::GetInstance()->GetMainCamera()->IsInsideFrustum(&myTransform);
		}
	}
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i]->myIsEnabled)
		{
			if (!myComponents[i]->myShouldCull)
			{
				if (PLAYMODE) myComponents[i]->FixedUpdate();
			}
			else
			{
				if (!shouldBeCulled)
				{
					if (PLAYMODE) myComponents[i]->FixedUpdate();
				}
			}
		}
	}

	auto& children = myTransform.GetChildren();
	for (int i = 0; i < children.size();)
	{
		if (children[i]->myGameObject->myShouldDestroy)
		{
			children[i]->myGameObject->DeleteObject();
			continue;
		}
		children[i++]->myGameObject->FixedUpdate();
	}
}

void GameObject::UpdateTransform()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i]->myIsEnabled)
		{
			myComponents[i]->OnTransformChanged();
		}
	}
}
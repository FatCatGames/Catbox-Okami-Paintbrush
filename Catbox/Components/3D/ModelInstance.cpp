#include "stdafx.h"
#include <fstream>
#include "Assets/Material.h"
#include "../Components/Physics/Collisions/SphereCollider.h"
#include "ModelInstance.h"
#include "Assets/Model.h"
#include "Graphics/Animations/SkeletonData.h"
#include "Animator.h"



ModelInstance::~ModelInstance()
{
	Engine::GetInstance()->GetGraphicsEngine()->RemoveFromRenderQueue(this);
}

void ModelInstance::Update()
{
	Render();
}

void ModelInstance::Init()
{
	myShouldCull = true;
}
void ModelInstance::Awake()
{
	if (myShouldBeInstanceMaterial)
	{
		for (int i = 0; i < myMaterials.size(); i++)
		{
			myMaterials[i] = myMaterials[i]->MakeInstanceOfMaterial();
		}
	}
}

void ModelInstance::RunInEditor()
{
	Render();
}

void ModelInstance::Render()
{
	if (myIsEnabled && myModel != nullptr)
	{
		Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(this, myRendererToUse);
		for (int i = 0; i < myInstanceCount; i++)
		{
			myModel->RenderInstance(&myInstances[i]);
		}
	}
}


void ModelInstance::RenderInProperties(std::vector<Component*>& aComponentList)
{
	std::vector<ModelInstance*> modelInstanceList;
	for (Component* component : aComponentList)
	{
		ModelInstance* modelInstance = dynamic_cast<ModelInstance*>(component);
		modelInstanceList.push_back(modelInstance);
	}

	auto modelList = AssetRegistry::GetInstance()->GetAssets<Model>();

	bool sharedModel = true;
	for (ModelInstance* modelInstance : modelInstanceList)
	{
		if (modelInstance->myModel != myModel)
		{
			sharedModel = false;
			break;
		}
	}

	bool edit;
	auto newModel = AssetRegistry::GetInstance()->AssetDropdownMenu<Model>(sharedModel ? myModel.get() : nullptr, "Model", edit);
	if (edit)
	{
		for (ModelInstance* modelInstance : modelInstanceList)
		{
			modelInstance->SetModel(newModel);
		}
	}

	ImGui::Text("Materials");

	std::vector<int> indices;
	indices.resize(myMaterials.size());

	for (ModelInstance* modelInstance : modelInstanceList)
	{
		if (modelInstance->myMaterials.size() < indices.size())
		{
			indices.resize(modelInstance->myMaterials.size());
		}
	}


	for (int i = 0; i < static_cast<int>(myMaterials.size()); i++)
	{
		auto mat = AssetRegistry::GetInstance()->AssetDropdownMenu<Material>(myMaterials[i].get(), std::to_string(i), edit);
		if (edit)
		{
			for (ModelInstance* modelInstance : modelInstanceList)
			{
				modelInstance->myMaterials[i] = mat;
			}
		}
	}

	if (Catbox::Checkbox("Used for navigation", &myUsedForNavMesh))
	{
		for (ModelInstance* modelInstance : modelInstanceList)
		{
			modelInstance->myUsedForNavMesh = myUsedForNavMesh;
		}
	}
	bool myShouldInstance = myShouldBeInstanceMaterial;
	if (Catbox::Checkbox("Instance Material", &myShouldInstance))
	{
		for (ModelInstance* modelInstance : modelInstanceList)
		{
			modelInstance->myShouldBeInstanceMaterial = myShouldInstance;
		}
	}

	if (Catbox::Checkbox("Cast shadows", &myShouldCastShadows))
	{
		for (ModelInstance* modelInstance : modelInstanceList)
		{
			modelInstance->myShouldCastShadows = myShouldCastShadows;
		}
	}

	int rendererIndex = (int)myRendererToUse;
	const char* items[] = { "Deferred", "Forward" };
	if (ImGui::Combo("Renderer", &rendererIndex, items, IM_ARRAYSIZE(items)))
	{
		for (ModelInstance* modelInstance : modelInstanceList)
		{
			modelInstance->myRendererToUse = (Renderer)rendererIndex;
		}
	}

	int oldInstanceCount = static_cast<int>(myInstances.size());
	myInstanceCount = oldInstanceCount;
	if (ImGui::InputInt("Instances", &myInstanceCount, 0, 1000))
	{
		myInstances.resize(myInstanceCount);
		myModel->SetMaxInstances(myInstanceCount);

		for (size_t i = oldInstanceCount; i < myInstanceCount; i++)
		{
			myInstances[i].SetWorldPos(myTransform->worldPos() + Vector3f::up() * static_cast<float>(i - (oldInstanceCount - 1)));
		}
	}
}

void ModelInstance::ClearModel()
{
	myModel = nullptr;

	if (mySkeletonInstance)
	{
		mySkeletonInstance->sharedData = nullptr;
	}

	Animator* animator = myGameObject->GetComponent<Animator>();
	if (animator) animator->OnModelChanged();
}

void ModelInstance::OnTransformChanged()
{
	if (myModel != nullptr)
	{
		Vector3f tempCenter = myModel->GetCenter();
		Vector4f myModelsWorld = Vector4f(tempCenter.x, tempCenter.y, tempCenter.z, 0.0f) * myTransform->GetWorldTransformMatrix();
		myTransform->SetCenter(Vector3f(myModelsWorld.x, myModelsWorld.y, myModelsWorld.z) + myTransform->worldPos());
	}
}

void ModelInstance::SetModel(std::shared_ptr<Model> aModel)
{
	myModel = aModel;
	int prev = static_cast<int>(myMaterials.size());
	int newPartCount = aModel ? static_cast<int>(aModel->GetModelParts().size()) : 0;
	int diff = newPartCount - prev;
	myMaterials.resize(newPartCount);

	for (int i = prev; i < diff; i++)
	{
		if (prev == 0) myMaterials[i] = AssetRegistry::GetInstance()->GetAsset<Material>("DefaultMat");
		else myMaterials[i] = myMaterials[static_cast<int>(prev - 1)];
	}

	if (aModel && aModel->GetSkeleton())
	{
		if (!mySkeletonInstance)
		{
			mySkeletonInstance = std::make_shared<SkeletonInstance>();
		}

		myGameObject->SetSkeleton(mySkeletonInstance);

		for (size_t i = 0; i < mySkeletonInstance->boneTransforms.size(); i++)
		{
			myTransform->RemoveChild(&mySkeletonInstance->boneTransforms[i]);
		}

		mySkeletonInstance->SetSharedData(aModel->GetSkeleton());
		for (size_t i = 0; i < mySkeletonInstance->boneTransforms.size(); i++)
		{
			mySkeletonInstance->boneTransforms[i].myIsBone = true;
			mySkeletonInstance->boneTransforms[i].myGameObject = myGameObject;
			int parentId = mySkeletonInstance->sharedData->bones[i].parentBoneIndex;
			mySkeletonInstance->boneTransforms[i].myBoneParentId = parentId;
			if (parentId != -1)
			{
				mySkeletonInstance->boneTransforms[i].myParent = &mySkeletonInstance->boneTransforms[parentId];
			}
			else mySkeletonInstance->boneTransforms[i].myParent = myTransform;
		}
	}
	else
	{
		if (mySkeletonInstance) mySkeletonInstance->sharedData = nullptr;
		myGameObject->SetSkeleton(nullptr);
	}

	Animator* animator = myGameObject->GetComponent<Animator>();
	if (animator)
	{
		animator->SetModelInstance(this);
	}
	Vector3f tempCenter = myModel ? myModel->GetCenter() : Vector3f::zero();
	Vector4f myModelsWorld = Vector4f(tempCenter.x, tempCenter.y, tempCenter.z, 0.0f) * myTransform->GetWorldTransformMatrix();
	myTransform->SetCenter(Vector3f(myModelsWorld.x, myModelsWorld.y, myModelsWorld.z) + myTransform->worldPos());
	myTransform->SetBoundsRadius(myModel ? myModel->GetBoundsRadius() : 0);
}


std::shared_ptr<Material> ModelInstance::GetMaterial(int anIndex)
{
	if (anIndex > static_cast<int>(myMaterials.size()) - 1)
	{
		return nullptr;
	}
	return myMaterials[anIndex];
}

void ModelInstance::SetMaterial(std::shared_ptr<Material> aMaterial, int anIndex, bool isInstance)
{
	if (isInstance && aMaterial)
	{
		myMaterials[anIndex] = aMaterial->MakeInstanceOfMaterial();
	}
	else
	{
		myMaterials[anIndex] = aMaterial;
	}
}

bool ModelInstance::ShouldDrawOutline()
{
	return myGameObject->IsSelected(0);
}

void ModelInstance::Save(rapidjson::Value& aComponentData)
{
	int version = 6;

	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);

	const char* modelName = "Empty \0";
	if (myModel)
	{
		modelName = myModel->GetName().c_str();
	}

	wrapper.SaveString("Model", modelName);

	auto materialsArray = wrapper.CreateArray();
	materialsArray.SetArray();
	wrapper.SetTarget(materialsArray);
	for (size_t i = 0; i < myMaterials.size(); i++)
	{
		const char* matName = "";
		if (myMaterials[i]) matName = myMaterials[i]->GetName().c_str();
		auto ref = rapidjson::GenericStringRef(matName);
		materialsArray.PushBack(ref, wrapper.GetDocument().GetAllocator());
	}

	wrapper.SetTarget(aComponentData);
	wrapper.SaveObject("Materials", materialsArray);
	wrapper.SaveValue<DataType::Bool>("UsedForNav", myUsedForNavMesh);
	wrapper.SaveValue<DataType::Int>("Renderer", myRendererToUse);
	wrapper.SaveValue<DataType::Bool>("Material Instance", myShouldBeInstanceMaterial);
	wrapper.SaveValue<DataType::Bool>("CastShadows", myShouldCastShadows);
	wrapper.SaveValue<DataType::Int>("InstanceCount", myInstanceCount);
}

void ModelInstance::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();
	string modelName = aComponentData["Model"].GetString();
	if (!modelName.empty())
	{
		std::shared_ptr<Model> model = AssetRegistry::GetInstance()->GetAsset<Model>(modelName);
		if (!model)
		{
			printerror("Model " + modelName + " could not be loaded!!");
			return;
		}
		SetModel(model);
	}

	auto materials = aComponentData["Materials"].GetArray();
	if (materials.Size() > myMaterials.size()) myMaterials.resize(static_cast<size_t>(materials.Size()));
	for (unsigned int i = 0; i < myMaterials.size(); i++)
	{
		if (i >= materials.Size())
		{
			if (i != 0)
			{
				myMaterials[i] = myMaterials[i - 1];
			}
			else
			{
				myMaterials[i] = AssetRegistry::GetInstance()->GetAsset<Material>("DefaultMat");
			}
		}
		else
		{
			string matName = materials[i].GetString();
			if (!matName.empty())
			{
				myMaterials[i] = AssetRegistry::GetInstance()->GetAsset<Material>(matName);
				if (!myMaterials[i])
				{
					myMaterials[i] = AssetRegistry::GetInstance()->GetAsset<Material>("DefaultMat");
				}
			}
			else
			{
				myMaterials[i] = AssetRegistry::GetInstance()->GetAsset<Material>("DefaultMat");
			}
		}
	}

	if (version >= 1)
	{
		myUsedForNavMesh = aComponentData["UsedForNav"].GetBool();
	}
	if (version >= 2 && version < 4)
	{
		myRendererToUse = (Renderer)(aComponentData["ForwardRender"].GetBool());
	}
	if (version >= 3)
	{
		if (aComponentData.HasMember(("Material Instance")))
		{
			myShouldBeInstanceMaterial = aComponentData[("Material Instance")].GetBool();
		}
		if (version >= 4)
		{
			myRendererToUse = (Renderer)(aComponentData["Renderer"].GetInt());

			if (version >= 5)
			{
				myShouldCastShadows = aComponentData["CastShadows"].GetBool();

				if (version >= 6)
				{
					myInstanceCount = aComponentData["InstanceCount"].GetInt();
					myInstances.resize(myInstanceCount);
					myModel->SetMaxInstances(myInstanceCount);

					for (size_t i = 0; i < myInstanceCount; i++)
					{
						myInstances[i].SetWorldPos(myTransform->worldPos() + Vector3f::up() * static_cast<float>(i + 1));
					}
				}
			}
		}
	}
}
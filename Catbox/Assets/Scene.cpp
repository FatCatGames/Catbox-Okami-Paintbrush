#include "stdafx.h"
#include "Scene.h"
#include "../GameObjects/GameObjectFactory.h"
#include "../Components/3D/Camera.h"
#include "3D/Light.h"
#include "CommonUtilities\RapidJsonWrapper.h"
#include "Editor\Commands\EditorCommandHandler.h"
#include "ComponentTools\SceneManager.h"

Scene::~Scene()
{
	myRootObjects.clear();
}


void Scene::Update()
{
	for (int i = 0; i < myRootObjects.size();)
	{
		if (myRootObjects[i]->myShouldDestroy)
		{
			myRootObjects[i]->DeleteObject();
			continue;
		}
		myRootObjects[i++]->Update();
	}
}

void Scene::FixedUpdate()
{
	for (int i = 0; i < myRootObjects.size();)
	{
		myRootObjects[i++]->FixedUpdate();
	}
}

void Scene::ImportScene()
{
	GameObject* aParent = Instantiate();
	aParent->SetName(myName.c_str());

	auto wrapper = RapidJsonWrapper::GetInstance();
	string path = myPath.string();

	auto& document = wrapper->LoadDocument(path.c_str());
	for (auto& objectData : document["GameObjects"].GetArray())
	{
		auto newObject = Instantiate();
		newObject->LoadObjectData(objectData);
		newObject->GetTransform()->SetParent(aParent->GetTransform());
	}
}

void Scene::LoadScene()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	string path = myPath.string();
	if (myShouldTempSave) path = "Temp/" + myName + ".scene";

	auto& document = wrapper->LoadDocument(path.c_str());
	for (auto& objectData : document["GameObjects"].GetArray())
	{
		auto newObject = Instantiate();
		newObject->LoadObjectData(objectData);
	}

	Engine::GetInstance()->GetGraphicsEngine()->SetLights(&myLightData);
}

void Scene::Unload()
{
	myRootObjects.clear();
	myLightData.ClearLightData();
	if (!EDITORMODE)
	{
		Engine::GetInstance()->GetGraphicsEngine()->SetCamera(nullptr);
	}
}


void Scene::AddRootObject(GameObject* anObject)
{
	if (!anObject) return;
	myRootObjects.push_back(anObject);
}

void Scene::RemoveRootObject(GameObject* anObject)
{
	if (!anObject) return;
	for (size_t i = 0; i < myRootObjects.size(); i++)
	{
		if (myRootObjects[i] == anObject)
		{
			myRootObjects.erase(myRootObjects.begin() + i);
			return;
		}
	}
}

void Scene::SaveAsset(const char* aPath)
{
	print("Saved scene!");
	Editor::GetInstance()->ResetTimeSinceSave();
	EditorCommandHandler::ClearCommandSinceSaveCount();

	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& document = wrapper.CreateDocument();
	auto objectsArray = wrapper.CreateArray();
	objectsArray.SetArray();

	for (auto& object : myRootObjects)
	{
		wrapper.SetTarget(objectsArray);
		object->SaveObjectData();
	}

	document.SetObject();
	document.AddMember("GameObjects", objectsArray, document.GetAllocator());

	wrapper.SaveDocument(aPath);
}


void Scene::LoadFromPath(const char* /*aPath*/)
{
	LoadScene();
}

void Scene::AddLight(Light* aLight)
{
	myLights.push_back(aLight);
	myLightData.AddLight(&aLight->GetLightData());
	if (aLight->GetLightData().type == 3)
	{
		myEnvLight = aLight;
		Engine::GetInstance()->GetGraphicsEngine()->SetEnvironmentLight(myEnvLight);
	}
}

void Scene::RemoveLight(Light* aLight)
{
	Light* newEnvLight = nullptr;
	for (size_t i = 0; i < myLights.size(); i++)
	{
		if (myLights[i] == aLight)
		{
			myLights.erase(myLights.begin() + i);
			myLightData.RemoveLight(&aLight->GetLightData());
			if (aLight->GetLightData().type != 3) break;
			--i;
		}
		else if (myLights[i]->GetLightData().type == 3)
		{
			newEnvLight = myLights[i];
		}
	}

	if (aLight->GetLightData().type == 3)
	{
		myEnvLight = newEnvLight;
		Engine::GetInstance()->GetGraphicsEngine()->SetEnvironmentLight(newEnvLight);
	}
}

void Scene::ChangeLightType(Light* aLight, int aNewType)
{
	myLightData.RemoveLight(&aLight->GetLightData());
	if (aLight->GetLightData().type == 3)
	{
		myEnvLight = nullptr;
		Engine::GetInstance()->GetGraphicsEngine()->SetEnvironmentLight(nullptr);
	}

	aLight->GetLightData().type = aNewType;
	if (aNewType == 3)
	{
		myEnvLight = aLight;
		Engine::GetInstance()->GetGraphicsEngine()->SetEnvironmentLight(myEnvLight);
	}

	myLightData.AddLight(&aLight->GetLightData());
}

void Scene::StartGameObjects()
{
	for (size_t i = 0; i < myRootObjects.size(); i++)
	{
		myRootObjects[i]->Start();
	}
}

void Scene::TempSave()
{
	myShouldTempSave = true;

	if (!std::filesystem::exists("Temp"))
	{
		CreateDirectoryA("Temp", 0);
	}

	SaveAsset(("Temp/" + myName + ".scene").c_str());
	myShouldTempSave = false;
}

void Scene::TempLoad()
{
	myShouldTempSave = true;
	LoadScene();
	myShouldTempSave = false;
}

void Scene::DoubleClickAsset()
{
	Engine::GetInstance()->GetSceneManager()->LoadScene(std::static_pointer_cast<Scene>(shared_from_this()));
}

GameObject* FindObject(GameObject* anObject, const std::string& aName)
{
	if (anObject->GetName() == aName)
	{
		return anObject;
	}

	for (auto child : anObject->GetTransform()->GetChildren())
	{
		if (FindObject(child->GetGameObject(), aName))
		{
			return child->GetGameObject();
		}
	}

	return nullptr;
}

GameObject* Scene::FindGameObject(const std::string& aName)
{
	for (size_t i = 0; i < myRootObjects.size(); i++)
	{
		GameObject* obj = FindObject(myRootObjects[i], aName);
		if (obj) return obj;
	}

	return nullptr;
}

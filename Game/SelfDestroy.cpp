#include "Game.pch.h"
#include "SelfDestroy.h"

void SelfDestroy::Update()
{
	myLifeTime += deltaTime;

	if(myLifeTime > myDestroyTime)
	{
		myGameObject->Destroy();
	}
}

void SelfDestroy::RenderInProperties(std::vector<Component*>& aComponentList)
{
	Catbox::InputFloat("Destroy time", &myDestroyTime);
}

void SelfDestroy::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Float>("Time", myDestroyTime);
}

void SelfDestroy::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Time")) 
	{
		myDestroyTime = aComponentData["Time"].GetFloat();
	}
}

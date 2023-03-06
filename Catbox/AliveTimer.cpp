#include "stdafx.h"
#include "AliveTimer.h"

void AliveTimer::SetAliveTimer(float aAliveTimer)
{
	myAliveTime = aAliveTimer;
}

void AliveTimer::Awake()
{
}

void AliveTimer::RenderInProperties()
{
	ImGui::InputFloat("Alive Time", &myAliveTime);
}

void AliveTimer::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Float>("Alive Time", myAliveTime);
}

void AliveTimer::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Alive Time")) 
	{
		myAliveTime = aComponentData["Alive Time"].GetFloat();
	}
}

void AliveTimer::Update()
{
	myAliveTime -= Engine::GetInstance()->GetDeltaTime();
	if (myAliveTime <= 0) 
	{
		myAliveTime = 0;
		myGameObject->Destroy();
	}
}

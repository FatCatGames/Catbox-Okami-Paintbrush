#include "stdafx.h"
#include "FollowTarget.h"

void FollowTarget::Update()
{
	if (!myTarget) return;

	myTransform->SetWorldPos(myTarget->worldPos() + myOffset);
}

void FollowTarget::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto t = ComponentVectorCast<FollowTarget>(aComponentList);

	Catbox::DragFloat3("Offset", &myOffset);
}

void FollowTarget::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Float>("x", myOffset.x);
	wrapper.SaveValue<DataType::Float>("y", myOffset.y);
	wrapper.SaveValue<DataType::Float>("z", myOffset.z);
}

void FollowTarget::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("x")) 
	{
		myOffset.x = aComponentData["x"].GetFloat();
		myOffset.y = aComponentData["y"].GetFloat();
		myOffset.z = aComponentData["z"].GetFloat();
	}
}

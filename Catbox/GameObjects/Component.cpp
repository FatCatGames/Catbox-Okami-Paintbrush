#include "stdafx.h"
#include "../Components/Physics/Collisions/CollisionData.h"
#include "ComponentHandler.h"

unsigned int Component::globalComponentCount = 0;

void Component::RenderInProperties(std::vector<Component*>& /*aComponentList*/)
{
	ImGui::Text("RenderInProperties() not implemented for component");
}

void Component::Destroy()
{
	myGameObject->RemoveComponent(this);
}

Transform* Component::GetTransform()
{
	return myTransform;
}

GameObject& Component::GetGameObject()
{
	return *myGameObject;
}

void Component::SetEnabled(bool anIsEnabled)
{
	myIsEnabled = anIsEnabled;
	if (myIsEnabled) OnEnable();
	else OnDisable();
}

const std::string& Component::GetName()
{
	return ComponentHandler::GetComponentName(typeid(*this).name());
}

void Component::Save(rapidjson::Value& /*aComponentData*/)
{
	std::cout << "Saving not implemented for " << GetName() << std::endl;
}

void Component::Load(rapidjson::Value& /*aComponentData*/)
{
	std::cout << "Loading not implemented for " << GetName() << std::endl;
}

std::string Component::GetComponentData()
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& document = wrapper.CreateDocument();
	Save(document);
	document.AddMember("Data", *wrapper.GetTarget(), document.GetAllocator());
	return wrapper.JsonToString(document);
}

void Component::SetGameObject(GameObject* aGameObject)
{
	myGameObject = aGameObject;
	myTransform = aGameObject->GetTransform();
}

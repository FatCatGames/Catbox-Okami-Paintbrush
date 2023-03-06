#pragma once
#include <map>
#include <string>
#include <functional>
#include "GameObjects\Component.h"

class Component;
class GameObject;


class ComponentHandler
{
public:

	template<typename T>
	static void RegisterComponent();

	template<typename T>
	static Component* AddComponentToObject(GameObject* anObject);

	static Component* AddComponentToObjectByName(GameObject* anObject, std::string aComponentType);

	static const std::string& GetComponentName(const char* aTypeIDName);

	static inline const std::map<std::string, std::function<Component* (GameObject*)>>& GetAddComponentFunctions() { return AddComponentFunc; }

	static void RenderComponentProperties(Component* aComponent);

	static void ManuallyRegisterComponents();
	
private:
	friend class Editor;
	static std::map<std::string, std::function<Component* (GameObject*)>> AddComponentFunc;
	static std::map<const char*, std::string> ComponentNames;
	static std::string ErrorStr;
};


inline const std::string& ComponentHandler::GetComponentName(const char* aTypeIDName)
{
	if (ComponentNames.find(aTypeIDName) != ComponentNames.end()) return ComponentNames.at(aTypeIDName);
	return ErrorStr;
}

template<typename T>
inline void ComponentHandler::RegisterComponent()
{
	std::string name = std::string(typeid(T).name());
	for (size_t i = 1; i < name.length(); i++)
	{
		if (isupper(name[i])) 
		{
			name.insert(name.begin() + i, ' ');
			++i;
		}
	}

	name.erase(0, 7);
	Component* aComponent = new Component;
	aComponent->RegisterComponentNodes();
	delete aComponent;
	AddComponentFunc.insert({name, AddComponentToObject<T> });
	ComponentNames.insert({ typeid(T).name(), name });
}

template<typename T>
inline Component* ComponentHandler::AddComponentToObject(GameObject* anObject)
{
	return anObject->AddComponent<T>();
}


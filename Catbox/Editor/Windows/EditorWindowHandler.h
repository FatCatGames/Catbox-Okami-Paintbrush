#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include "EditorWindow.h"


class EditorWindowHandler
{
public:
	EditorWindowHandler() = default;
	~EditorWindowHandler() = default;


	void Render();

	std::shared_ptr<EditorWindow> OpenWindow(WindowType aWindowType);

	template<class T>
	std::vector<T*> GetAllWindowsOfType(WindowType aWindowType);

	template<class T>
	T* GetWindowOfType(WindowType aWindowType);

	void CloseWindow(EditorWindow* aWindow);

	std::vector<std::shared_ptr<EditorWindow>>& GetActiveWindows() { return myActiveWindows; };

	void BringToFront(EditorWindow* aWindow);


private:
	std::vector<std::shared_ptr<EditorWindow>> myActiveWindows;
	std::unordered_map<WindowType, std::vector<std::shared_ptr<EditorWindow>>> myWindows;
};

template<class T>
std::vector<T*> EditorWindowHandler::GetAllWindowsOfType(WindowType aWindowType)
{
	auto& windows = myWindows[aWindowType];
	std::vector<T*> castedWindows;
	castedWindows.resize(windows.size());
	
	for (size_t i = 0; i < windows.size(); i++)
	{
		castedWindows[i] = dynamic_cast<T*>(windows[i].get());
	}

	return castedWindows;
}

template<class T>
inline T* EditorWindowHandler::GetWindowOfType(WindowType aWindowType)
{
	if(myWindows[aWindowType].empty()) return nullptr;
	return dynamic_cast<T*>(myWindows[aWindowType][0].get());
}

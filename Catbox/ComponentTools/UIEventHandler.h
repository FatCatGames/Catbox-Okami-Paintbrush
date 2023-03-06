#pragma once

class UIElement;
class UIEventHandler 
{
public:
	static void Init();
	static inline void AddUIElement(UIElement* aUIElement) { myUIElements.push_back(aUIElement); }
	static void RemoveUIElement(UIElement* aUIElement);
	static void Update();

	static void SetNextLevel(std::string aNextLevel);

private:
	static void ResizeUIElements();
	static std::vector<UIElement*> myUIElements;
	static std::string myNextLevel;
	static bool myShouldChangeLevel;
};
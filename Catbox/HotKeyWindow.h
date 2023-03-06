#pragma once
#include "Editor/Windows/EditorWindow.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"
class HotKeyWindow : public EditorWindow
{
public:

private:
	void OrderNames(const std::unordered_map<std::string, KeyCode>& aMap);
	void Render() override;
	std::string myCurrentEditHotkey = "";
	std::string myAddKey = "";
	bool myAddButtonClicked = false;
	bool myFirstRun = false;
	std::vector<std::string> myOrderedNames;
};


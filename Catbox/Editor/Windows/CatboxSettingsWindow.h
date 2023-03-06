#pragma once
#include "EditorWindow.h"
#include "UserPreferences.h"

class CatboxSettingsWindow : public EditorWindow
{
public:
	CatboxSettingsWindow();

private:

	void Render() override;
	UserPreferences* myPrefs;
};


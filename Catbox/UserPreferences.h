#pragma once

class UserPreferences
{
public:

	void SaveUserPreferences();
	void LoadUserPreferences();


	const inline Color& GetOutlineColor() { return mySelectionOutlineColor; }

	enum class Theme
	{
		Night,
		Light
	};

	Theme myTheme = Theme::Night;
	Color myClearColor = { 0.1f, 0.1f, 0.1f, 1 };
	Color mySelectionOutlineColor = { 1, 0.5f, 0, 1 };
	float myCamMoveSpeed = 10;
	float myCamScrollSpeed = 30;
	float myCamRotateSpeed = 20;
	float myMoveSnap[3] = { 1,1,1 };
	float myRotateSnap[3] = { 45.f, 45.f, 45.f };
	float myScaleSnap[3] = { 0.5f, 0.5f, 0.5f };
	bool mySnapToggle = false;
	bool myDebugDrawerToggle = false;
	std::string myDefaultIP = "127.0.0.1";
	std::string myUsername = "";
	std::string myChatColor = "FFFFFF";
};
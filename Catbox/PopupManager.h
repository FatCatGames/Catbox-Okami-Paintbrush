#pragma once
struct Popup
{
	float aliveTime = 0;
	float maxAliveTime;
	std::string name;
	std::string content;
	Vector2f size = {100,100};
	Vector2f pos = { 0,0 };
};
class PopupManager
{
public:
	static void CreatePopup(std::string aName, float aTime = 1, Vector2f aSize = Vector2f(50, 50), std::string aContent = std::string(""), Vector2f aPos = { Engine::GetInstance()->GetWindowSize().x * 0.5f, Engine::GetInstance()->GetWindowSize().y * 0.5f });
	static void Render();
	static void Update();
private:
	static std::vector<Popup> myPopups;
};


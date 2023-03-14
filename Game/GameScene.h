#pragma once

class GameScene : public Component
{
public:
	static GameScene* GetInstance() { return Instance; }
	GameScene();
	void Update() override;
	void PerformAction(const std::string& anAction, Vector2i& aPosition);

private:
	static GameScene* Instance;

	//Actions
	float myLerpTimer = 2;
	float myLerpTarget = 2;
	bool myIsDay = true;
	Color myDaytimeColor;
	Color myNighttimeColor;
	Color myCurrentColor;
};
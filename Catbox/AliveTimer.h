#pragma once
class AliveTimer : public Component
{
public:
	void SetAliveTimer(float aAliveTimer);
	void Awake();
	void RenderInProperties();
	void Save(rapidjson::Value& aComponentData);
	void Load(rapidjson::Value& aComponentData);
	void Update();
private:
	float myAliveTime;
};


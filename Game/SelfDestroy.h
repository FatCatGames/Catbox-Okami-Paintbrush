#pragma once

class SelfDestroy : public Component
{
public:
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

private:
	float myLifeTime = 0;
	float myDestroyTime = 1;
};
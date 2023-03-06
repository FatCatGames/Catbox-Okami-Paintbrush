#pragma once

class FollowTarget : public Component
{
public:

	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;

private:

	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	Transform* myTarget;
	Vector3f myOffset;
};
#pragma once
#include "GameObjects\Component.h"

class ComponentTemplate : public Component
{
public:
	void Awake() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;

private:
	void Update() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	//void OnCollisionEnter(Collider* aCollider) override;
	//void OnCollisionStay(Collider* aCollider) override;
	//void OnCollisionExit(Collider* aCollider) override;
};
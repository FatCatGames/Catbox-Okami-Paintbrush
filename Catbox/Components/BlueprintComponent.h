#pragma once
#include "GameObjects\Component.h"
class ScriptGraph;
class Blueprint;
class BlueprintComponent : public Component
{
public:
	void Awake() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	std::shared_ptr<Blueprint> GetBlueprint() { return myBlueprint; }

private:
	void Update() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	void OnCollisionEnter(Collider* aCollider) override;
	void OnCollisionStay(Collider* aCollider) override;
	void OnCollisionExit(Collider* aCollider) override;
	void OnOverlapBegin(Collider* aCollider) override;
	void OnOverlap(Collider* aCollider) override;
	void OnOverlapEnd(Collider* aCollider) override;

	std::shared_ptr<Blueprint> myBlueprint;
};
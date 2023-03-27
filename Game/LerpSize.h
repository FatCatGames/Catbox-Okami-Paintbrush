#pragma once
#include "CommonUtilities\Curve.h"

class LerpSize : public Component
{
public:
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

private:
	Curve mySizeCurve;
	float myCurrentLifeTime = 0;
	float myTargetLifetime = 2;
};
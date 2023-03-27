#include "Game.pch.h"
#include "LerpSize.h"

void LerpSize::Awake()
{
	mySizeCurve.AddCoord({ 0.f, 0.f });
	mySizeCurve.AddCoord({ 0.3f, 1.f });
	mySizeCurve.AddCoord({ 0.7f, 1.f });
	mySizeCurve.AddCoord({ 1.f, 0.f });
}

void LerpSize::Update()
{
	myCurrentLifeTime += deltaTime;
	myTransform->SetWorldScale(Vector3f::one() * mySizeCurve.Evaluate(myCurrentLifeTime / myTargetLifetime));

	if(myCurrentLifeTime > myTargetLifetime)
	{
		myGameObject->Destroy();
	}
}

void LerpSize::RenderInProperties(std::vector<Component*>& aComponentList)
{
	Catbox::InputFloat("Lifetime", &myTargetLifetime);
}

void LerpSize::Save(rapidjson::Value& aComponentData)
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	wrapper->SaveValue<DataType::Float>("Lifetime", myTargetLifetime);
}

void LerpSize::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Lifetime"))
	{
		myTargetLifetime = aComponentData["Lifetime"].GetFloat();
	}
}
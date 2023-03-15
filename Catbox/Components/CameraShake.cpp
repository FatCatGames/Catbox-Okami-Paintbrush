#include "stdafx.h"
#include "CameraShake.h"
#include "CameraController.h"

void CameraShake::Awake()
{
	if (!myGameObject->GetComponent<Camera>())
	{
		printerror("Camera Shake needs to be attached to a camera");
	}
	myTempShakeDuration = myShakeDuration;
	myTempShakeIntensity = myCameraShakeIntensity;
}

void CameraShake::Init()
{
	myShakeIntensityMultiplier.AddCoord({ 0, 0 });
	myShakeIntensityMultiplier.AddCoord({ 0.25f, 0.15f });
	myShakeIntensityMultiplier.AddCoord({ 0.5f, 1 });
	myShakeIntensityMultiplier.AddCoord({ 0.75f, 0.15f });
	myShakeIntensityMultiplier.AddCoord({ 1, 0 });
	myCameraShakeActive = false;
}

void CameraShake::Update()
{
	if (Engine::GetInstance()->IsGamePaused())
	{
		myCameraShakeActive = false;
		return;
	}

	if (myCameraShakeActive == true)
	{
		myTotalShakeTime += deltaTime;

		//float myIntensityMultiplier = (0.5f - (abs((myTotalShakeTime / myTempShakeDuration) - 0.5f))) * 2.f;

		myTimeSinceShake += deltaTime;

		if (myTotalShakeTime >= myTempShakeDuration)
		{
			Stop();
			return;
		}
		if (myTimeSinceShake > myTimeBetweenShakes)
		{
			myTimeSinceShake = 0;

			myCameraOrigin.offset = myGameObject->GetTransform()->worldPos();
			myCameraOrigin.rotation = myGameObject->GetTransform()->worldRot();
			myCameraOrigin.fov = myGameObject->GetComponent<Camera>()->GetFoV();

			Vector3f dir = { Catbox::GetRandom(-1.0f, 1.0f), 0, Catbox::GetRandom(-1.0f, 1.0f) };
			Vector3f shakeDistance = myShakeIntensityMultiplier.Evaluate(myTotalShakeTime / myTempShakeDuration) * myTempShakeIntensity * dir;

			myTemporaryCamera->GetTransform()->SetWorldPos(myCameraOrigin.offset + shakeDistance);
			myTemporaryCamera->GetTransform()->SetWorldRot(myCameraOrigin.rotation);
			myTemporaryCamera->GetComponent<Camera>()->SetFoV(myCameraOrigin.fov);
		}
	}
}

void CameraShake::Start()
{
	Stop();

	myTemporaryCamera = Instantiate();
	myTemporaryCamera->AddComponent<Camera>();
	myTemporaryCamera->GetComponent<Camera>()->SetFoV(myGameObject->GetComponent<Camera>()->GetFoV());
	myTemporaryCamera->GetTransform()->SetWorldPos(myGameObject->GetComponent<Camera>()->GetTransform()->worldPos());
	myTemporaryCamera->GetTransform()->SetWorldRot(myGameObject->GetComponent<Camera>()->GetTransform()->worldRot());
	myTemporaryCamera->SetName("Temporary camera (DON'T CLICK)");

	Engine::GetInstance()->SetActiveCamera(myTemporaryCamera->GetComponent<Camera>());

	myCameraShakeActive = true;
}

void CameraShake::Pause()
{
	myCameraShakeActive = false;
}

void CameraShake::Stop()
{
	if (myTemporaryCamera != nullptr)
	{
		Engine::GetInstance()->SetActiveCamera(myGameObject->GetComponent<Camera>());
		//Engine::GetInstance()->GetCameraController()->RemoveCamera(myTemporaryCamera->GetComponent<Camera>());
		myTemporaryCamera->Destroy();
	}

	myTotalShakeTime = 0.0f;
	myTempShakeDuration = myShakeDuration;
	myTempShakeIntensity = myCameraShakeIntensity;
	myCameraShakeActive = false;
}

void CameraShake::RenderInProperties(std::vector<Component*>& aComponentList)
{
	if (ImGui::Button("Play"))
	{
		Start();
	}
	if (ImGui::Button("Pause"))
	{
		Pause();
	}
	if (ImGui::Button("Stop"))
	{
		Stop();
	}

	ImGui::DragFloat("Intensity", &myCameraShakeIntensity, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Duration", &myShakeDuration, 0.1f, 0.0f, 780.0f);
}

void CameraShake::Save(rapidjson::Value& aComponentData)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Float>("Intensity", myCameraShakeIntensity);
	wrapper.SaveValue<DataType::Float>("Duration", myShakeDuration);
}

void CameraShake::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Intensity"))
	{
		myCameraShakeIntensity = aComponentData["Intensity"].GetFloat();
	}
	if (aComponentData.HasMember("Duration"))
	{
		myShakeDuration = aComponentData["Duration"].GetFloat();
	}
}
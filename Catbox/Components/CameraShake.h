#pragma once
#include "GameObjects/Component.h"
#include "CommonUtilities\Curve.h"
class CameraShake : public Component
{
public:

	CameraShake() = default;
	~CameraShake() = default;
	void Awake() override;
	void Init() override;
	void Update() override;
	void Start();
	void Pause();
	void Stop();
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	bool GetCameraShakeActive() { return myCameraShakeActive; };
	float GetCameraShakeIntesity() { return myTempShakeIntensity; };
	void SetCameraShakeIntensity(float anIntensity) { myTempShakeIntensity = anIntensity; };

	float GetShakeDuration() { return myTempShakeDuration; };
	void SetShakeDuration(float aDuration) { myTempShakeDuration = aDuration; };

private:
	float myTotalShakeTime;
	Camera::CameraSettingsTemplate myCameraOrigin;
	GameObject* myTemporaryCamera = nullptr;
	Curve myShakeIntensityMultiplier;
	bool myCameraShakeActive;
	float myCameraShakeIntensity;
	float myShakeDuration;
	float myTempShakeIntensity;
	float myTempShakeDuration;
	float myTimeBetweenShakes = 0.02f;
	float myTimeSinceShake;
};


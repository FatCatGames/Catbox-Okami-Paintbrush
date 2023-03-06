#pragma once
#include "3D/CinematicCamera.h"

class PlayerCamera;

class CameraController
{
public:
	CameraController();
	~CameraController();

	void AddCamera(Camera* aCamera);
	std::vector<Camera*>& GetCameras();
	void RemoveCamera(Camera* aCamera);
	void SetPlayerCamera(PlayerCamera* aCamera);
	PlayerCamera* GetPlayerCamera();

	void Update();
	void Clear();

	void TransitionTo(Camera* aCameraTo);
	void SmoothTransitionTo(Camera* aCameraTo, float someSeconds);
	bool IsTransitioning() { return myIsTransitioning; }
private:
	float LerpFoV();
	float LerpPosition();
	float LerpRotation();

	std::vector<Camera*> myCameras;
	Camera* myCurrentCamera;
	PlayerCamera* myPlayerCamera = nullptr;
	GameObject* myTemporaryCamera;
	Camera::CameraSettingsTemplate myTemporaryCameraOrigin;
	float myTempStartTime;
	float myDelayStartTime;
	float mySecondsToTransition;

	bool myIsTransitioning;
};
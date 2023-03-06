#pragma once
#include "Camera.h"

class CinematicCamera : public Camera
{
public:
	CinematicCamera();
	~CinematicCamera();


	inline int GetIsInstant() { return myIsInstant; }
	inline int GetNextCamera() { return myNextCamera; }
	inline float GetDelay() { return myDelay; }
	inline float GetSpeed() { return mySpeed; }

	void RenderInProperties(std::vector<Component*>& aComponentList) override;

private:

	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	int myNextCamera = -1;
	bool myIsInstant = true;
	float myDelay = 0.0f;
	float mySpeed = 1.0f;
};
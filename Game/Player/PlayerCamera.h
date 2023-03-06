#pragma once
#include "Components/3D/Camera.h"
#include "Physics/PhysicsEngine.h"



class PlayerCamera : public Camera
{
public:
	PlayerCamera();
	~PlayerCamera();

	void Update() override;
	void Init() override;
	void Awake() override;

	void SetFollowTarget(Transform* aTarget);
	Vector3f GetFollowOffset();
	void SetFollowOffset(Vector3f anOffset);

	void RotateAroundObject(float someDegreesX, float someDegreesY);

	void RenderInProperties(std::vector<Component*>& aComponentList) override;

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	Transform* myFollowTarget = nullptr;
	Vector3f myFollowOffset;
	Vector3f myCurrentFollowOffset;
	Vector3f myTargetPosition;

	physx::PxQuat myTargetRotation;

	float myYFollowOffset = 1.5f;
	float mySpeed;
	float myMaxAngleRadian;
	float myMinAngleRadian;
};


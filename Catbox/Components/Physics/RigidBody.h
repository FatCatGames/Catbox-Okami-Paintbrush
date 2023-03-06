#pragma once
#include "stdafx.h"
#include "physX/PxRigidActor.h"

enum Shape;

class RigidBody: public Component
{
public:
	struct RigidBodyData 
	{
		float mass = 5;
		float gravity = 9.81f;
		float drag = 50.0f;
		bool useGravity = true;
		bool isStatic = true;
	};

	enum class Axis
	{
		x,
		y,
		z
	};

	RigidBody() = default;
	~RigidBody();
	RigidBody(physx::PxRigidActor* aActor);

	void Awake() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void SetRigidShape(Shape aShape);

	void Update() override;
	void FixedUpdate() override;
	RigidBodyData& GetData();
	Vector3f& GetVelocity();
	void SetActorPosition(const Vector3f& aPos);
	void SetActorRotation(const Vector3f& aRot);
	void SetVelocity(Vector3f aDirection, float aForce);
	void SetAngularVelocity(Vector3f aDirection, float aForce);
	Vector3f AddForce(Vector3f aDirection, float aForce);
	Vector3f AddForceAtPos(const float aForce, const Vector3f aPosition);
	void ScheduleForce(const Vector3f aPosition);
	void ChangeGravityScale(bool aGravityScale);
	void ChangeMass(float aMass);
	void SetTranslationAxisLock(bool x, bool y, bool z, bool shouldWakeUp = true);
	void SetRotationAxisLock(bool x, bool y, bool z, bool shouldWakeUp = true);
	bool GetTranslationAxisLock(Axis anAxis);
	bool GetRotationAxisLock(Axis anAxis);
private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	physx::PxRigidActor* myActor;
	RigidBodyData myRbData;
	Vector3f myVelocity;
	Vector3f myScheduledForce;
	bool myDebugMode = false;
	float myMass = 1;
	bool myGravity = true;
	bool myLockTranslations[3] = { false, false, false };
	bool myLockRotations[3] = { false, false, false };
};
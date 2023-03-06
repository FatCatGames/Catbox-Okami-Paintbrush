#pragma once
#include "Physics/PhysicsEngine.h"

enum CollisionDirection
{
	PxCF_Down,
	PxCF_Top,
	PxCF_Side,
	PxCF_None
};

class CharacterController : public Component
{
public:
	~CharacterController();
	void Awake() override;
	void Move(Vector3f aDirection);
	void Rotate(Vector3f aDirection);
	void SetMovementSpeed(float aMovementSpeed);
	void SetStepHeight(float aHeight);
	void SetContactOffset(float aHeight);
	void SetDensity(float aDensity);
	void SetFootPosition(Vector3f aPosition);
	void SetUpDirection(Vector3f aDirection);
	void SetGravity(float aGravity);
	void SetSlopeOffset(float aOffset);
	void SetVelocity(Vector3f aVelocity) { myVelocity = aVelocity; }
	void Slerp(Vector3f aAngles, float t);
	bool CheckCollisionDirection(CollisionDirection aDirection);
	CollisionDirection GetCollisionDirection();

	void SetPosition(Vector3f aPos) { myCharacterController->setPosition(physx::PxExtendedVec3(aPos.x, aPos.y, aPos.z)); };

	Vector3f GetVelocity() { return myVelocity; }
	Vector3f GetMoveDirection() { return myDirection; }

	void AddForce(const Vector3f& aDirection, float aForce);
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void OnCollisionStay(Collider* aCollider) override;

	void StopNavPathFinding();

private:
	void Update();
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	physx::PxController* myCharacterController = nullptr;
	//Character Controller
	bool myIsGrounded = false;
	float myMovmentSpeed = 1.0f;
	float myStepHeight = 1.0f;
	float myGravity = 9.81f;
	Vector3f myVelocity = {0,0,0};
	physx::PxQuat aTestQuat;
	float myContactOffset = 1.0f;
	float mySlopeOffset = 1.0f;
	bool myUsingNavAgent = false;

	//Character Controller Desc
	float myDensity = 1.0f;

	physx::PxControllerCollisionFlags myCollisionFlags;
	physx::PxFilterData myControllerFilters;

	physx::PxQuat myTargetRotation;

	Vector3f myFootPosition = { 0,0,0 };
	Vector3f myUpDirection = { 0,1,0 };
	Vector3f myDirection = Vector3f(0, 0, 0);
};
#pragma once
class RigidBody;
class Bomb : public Component
{
public:
	void Awake() override;
	void OnTransformChanged() override;
	void Update() override;
	void FixedUpdate() override;
	void OnCollisionStay(Collider* aCollider) override;

private:
	RigidBody* myRb;
	Vector3f mySpawnPos;
	Vector3f myPushForce;
	float mySpeed = 0;
	float myAliveTime = 0;
	bool myHasSavedPos = false;
	bool myHasLoadedPos = false;
};
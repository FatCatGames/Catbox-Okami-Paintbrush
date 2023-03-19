#pragma once
class RigidBody;
class Tree : public Component
{
public:
	void Slash(const Vector3f& anIntersectionPos, const Vector3f& anIntersectionDir);
	void Update() override;

private:
	Transform* myTreeTop;
	bool myIsLerping = false;
	float myLerpTimer = 0;
	float myLerpTarget = 3;
	float myTargetAngleX;
	float myTargetAngleZ;
	Vector3f mySlashDir;
	Vector3f myStartRot;
};
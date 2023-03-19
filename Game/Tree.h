#pragma once
#include "CommonUtilities\Curve.h"

class RigidBody;
class Tree : public Component
{
public:
	void Slash(const Vector3f& anIntersectionPos, const Vector3f& anIntersectionDir);
	void Update() override;

private:
	Transform* myTreeTop;
	Material* myTreeTopMat;
	Material* myTreeTrunkMat;
	bool myIsLerping = false;
	float myLerpTimer = 0;
	float myLerpTarget = 1.5f;
	float myDeadTime = 0;
	float myTargetAngleX;
	float myTargetAngleZ;
	Vector3f mySlashDir;
	Vector3f myStartRot;
	Curve myLerpCurve;
};
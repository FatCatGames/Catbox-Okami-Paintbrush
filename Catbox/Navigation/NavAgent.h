#pragma once
#include "NavMesh.h"

//class NavMesh;
class NavAgent : public Component
{
public:
	void Init() override;
	void Awake() override;
	void Update() override;
	void RunInEditor() override;
	bool FindPath(const Vector3f& aTarget, bool forcePathFinding = false);
	void ChangeSpeed(float aMovmentSpeed);
	float GetSpeed() { return mySpeed; }
	void PausePathfinding();
	void StopPathFinding();
	void ResumePathfinding();
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	bool ReachedTarget() const { return myTargetReached; }
	Vector3f GetCurrentDirection() const { if (!myTargetReached) { return myDir; } return Vector3f::zero(); }
	std::shared_ptr<NavMesh> GetNavMesh() const;
	void SetNavMesh(std::shared_ptr<NavMesh> aNavMesh) { myNavMeshToUse = aNavMesh; }
	void SetUsingCharacterController(bool aFlag) { myUsingCharacterController = aFlag; }
private:

	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	float mySpeed = 5;

	std::shared_ptr<NavMesh> myNavMeshToUse = nullptr;
	std::vector<Vector3f> myPath;

	std::vector<NavMesh::Connection*> path;
	NavMesh::Connection end;
	Vector3f myStartPoint;
	std::vector<Vector3f> myLeftFunnelPoints;
	std::vector<Vector3f> myRightFunnelPoints;
	Vector3f portalLeft;
	Vector3f portalApex;
	Vector3f portalRight;
	bool drawRightNext = true;
	int myFunnelPointIndex = 0;

	dtQueryFilter myDtFilter;

	const int myMaxPathSize = 255;
	bool myUsingCharacterController = false;

	int myPathIndex = 0;
	Vector3f myDir;
	Vector3f myTargetPosition = Vector3f(INT_MAX, INT_MAX, INT_MAX);
	bool myTargetReached = true;
	bool myPathFailed = false;
	float myAngleY = 0;
};
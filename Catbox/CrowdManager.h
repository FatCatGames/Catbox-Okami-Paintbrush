#pragma once
#include "Recast & Detour/DetourCrowd/DetourCrowd.h"
#include "Navigation/NavMesh.h"
class CrowdManager
{
public:
	void AddCrowd(int aId, dtNavMesh* aNavmesh);
	int AddAgentToCrowd(int aCrowdId, NavMesh* aNavmesh, const Vector3f& aPos, const Vector3f& aSize, dtQueryFilter* aFilter);
	const dtCrowdAgent* GetAgent(int aCrowdId, int aAgentId);

	void Update();
private:
	std::vector<dtCrowd*> myCrowds;
	int myCrowdIdCounter = 0;
};


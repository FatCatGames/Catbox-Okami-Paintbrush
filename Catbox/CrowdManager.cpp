#include "stdafx.h"
#include "CrowdManager.h"
#include "Navigation/NavMeshExporter.h"

void CrowdManager::AddCrowd(int aId, dtNavMesh* aNavmesh)
{
	dtCrowd* newCrowd = dtAllocCrowd();
	newCrowd->init(20, 5, aNavmesh);

	// Setup local avoidance params to different qualities.
	dtObstacleAvoidanceParams params;
	// Use mostly default settings, copy from dtCrowd.
	memcpy(&params, newCrowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

	// Low (11)
	params.velBias = 0.5f;
	params.adaptiveDivs = 5;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 1;
	newCrowd->setObstacleAvoidanceParams(0, &params);

	// Medium (22)
	params.velBias = 0.5f;
	params.adaptiveDivs = 5;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 2;
	newCrowd->setObstacleAvoidanceParams(1, &params);

	// Good (45)
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 3;
	newCrowd->setObstacleAvoidanceParams(2, &params);

	// High (66)
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 3;
	params.adaptiveDepth = 3;
	newCrowd->setObstacleAvoidanceParams(3, &params);
}

int CrowdManager::AddAgentToCrowd(int aCrowdId, NavMesh* aNavmesh, const Vector3f& aPos, const Vector3f& aSize, dtQueryFilter* aFilter)
{
	if (aCrowdId < myCrowds.size()) 
	{
		auto settings = aNavmesh->GetBakingSettings();
		dtCrowdAgentParams ap;
		memset(&ap, 0, sizeof(ap));
		ap.radius = settings.agentRadius;
		ap.height = settings.agentHeight;
		ap.maxAcceleration = 8.0f;
		ap.maxSpeed = 3.5f;
		ap.collisionQueryRange = ap.radius * 12.0f;
		ap.pathOptimizationRange = ap.radius * 30.0f;
		ap.updateFlags = 0;
		//if (m_toolParams.m_anticipateTurns)
		ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
		//if (m_toolParams.m_optimizeVis)
		ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
		//if (m_toolParams.m_optimizeTopo)
		ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
		//if (m_toolParams.m_obstacleAvoidance)
		ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
		//if (m_toolParams.m_separation)
		ap.updateFlags |= DT_CROWD_SEPARATION;

		ap.obstacleAvoidanceType = (unsigned char)(1);
		ap.separationWeight = 5.0f;

		float pos[3];
		pos[0] = aPos.x;
		pos[1] = aPos.y;
		pos[2] = aPos.z;

		int idx = myCrowds[aCrowdId]->addAgent(pos, &ap);
		if (idx != -1)
		{
			float size[3];
			size[0] = aSize.x;
			size[1] = aSize.y;
			size[2] = aSize.z;

			dtPolyRef nearest;
			
			float tarPos[3];

			auto targetRef = aNavmesh->GetDtNavMeshQuery()->findNearestPoly(pos, size, aFilter, &nearest, tarPos);
			if (targetRef)
			{
				myCrowds[aCrowdId]->requestMoveTarget(idx, nearest, tarPos);
			}				
			return idx;
		}
	}
	return -1;
}

const dtCrowdAgent* CrowdManager::GetAgent(int aCrowdId, int aAgentId)
{
	return myCrowds[aCrowdId]->getAgent(aAgentId);
}

void CrowdManager::Update()
{
	for (auto& crowd : myCrowds)
	{
		if (crowd) 
		{
			crowd->update(deltaTime, nullptr);
		}
	}
}

#include "stdafx.h"
#include "Pathfinding.h"
#include "StringPulling.h"

std::vector<NavMesh::Connection*> RetracePath(NavMesh::Node* aStart, NavMesh::Node* anEnd)
{
	std::vector<NavMesh::Connection*> path;
	NavMesh::Connection* c = anEnd->parent;
	while (c->from != aStart)
	{
		path.push_back(c);
		c = c->from->parent;
	}
	path.push_back(c);

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<Vector3f> Navigation::FindPath(const Vector3f& aStart, const Vector3f& aTarget, NavMesh& aNavMesh, AStarPath& path)
{
	path.clear();
	bool pathSuccess = false;

	auto& nodes = aNavMesh.GetNodes();

	std::vector<NavMesh::Node*> openSet;
	std::vector<NavMesh::Node*> closedSet;

	float shortestDistStart = FLT_MAX;
	float shortestDistEnd = FLT_MAX;
	int shortestDistIndexStart = -1;
	int shortestDistIndexEnd = -1;
	for (int i = 0; i < nodes.size(); i++)
	{
		float distSqrStart = (nodes[i].center - aStart).LengthSqr();
		if (distSqrStart < shortestDistStart)
		{
			shortestDistStart = distSqrStart;
			shortestDistIndexStart = i;
		}
		float distSqrEnd = (nodes[i].center - aTarget).LengthSqr();
		if (distSqrEnd < shortestDistEnd)
		{
			shortestDistEnd = distSqrEnd;
			shortestDistIndexEnd = i;
		}
	}

	auto startPoint = &nodes[shortestDistIndexStart];
	openSet.push_back(startPoint);
	auto targetPoint = &nodes[shortestDistIndexEnd];
	startPoint->gCost = 0;

	while (!openSet.empty())
	{
		auto& currentPoint = *openSet.front();
		openSet.erase(openSet.begin());
		closedSet.push_back(&currentPoint);

		if (currentPoint.index == targetPoint->index)
		{
			targetPoint = &currentPoint;
			pathSuccess = true;
			break;
		}

		for (auto& c : currentPoint.connections)
		{
			if (std::find(closedSet.begin(), closedSet.end(), c.to) != closedSet.end()) continue;

			int newMoveCostToNeighbor = currentPoint.gCost + c.cost;
			bool isInOpenSet = std::find(openSet.begin(), openSet.end(), c.to) != openSet.end();
			if (newMoveCostToNeighbor < c.to->gCost || !isInOpenSet)
			{
				c.to->gCost = newMoveCostToNeighbor;
				c.to->hCost = NavMesh::GetDistance(c.to->center, aTarget);
				c.to->parent = &c;

				if (!isInOpenSet)
				{
					openSet.push_back(c.to);
				}
			}
			isInOpenSet = isInOpenSet;
		}
	}

	std::vector<Vector3f> waypoints;
	if (pathSuccess)
	{
		print("Path found!");
		if (startPoint != targetPoint)
		{
			path = RetracePath(startPoint, targetPoint);
			//waypoints = stringPull(path);
		}

		waypoints.push_back(aTarget);
	}
	else
	{
		std::string start = "(" + std::to_string(aStart.x) + ", " + std::to_string(aStart.z) + ")";
		std::string end = "(" + std::to_string(aTarget.x) + ", " + std::to_string(aTarget.z) + ")";
		print("Failed to find path from " + start + " to " + end);
	}

	return waypoints;
}


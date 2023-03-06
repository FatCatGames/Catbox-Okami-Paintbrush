#pragma once
#include "NavMesh.h"

#define AStarPath std::vector<NavMesh::Connection*>
class Navigation
{
public:
	static std::vector<Vector3f> FindPath(const Vector3f& aStart, const Vector3f& aTarget, NavMesh& aNavMesh, AStarPath& path);
};
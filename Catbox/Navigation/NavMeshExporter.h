#pragma once
#include "Assets\MeshData.h"

enum Partitioning
{
	WaterShed = 0,
	Monotone,
	Layers
};

enum Filters
{
	LowHangingObstacles = 0,
	LedgeSpans,
	WalkableLowHeightSpans
};

struct NavmeshBakingSettings
{
	//Rasterization
	float cellSize = 0; // 0.1f - 1.0f
	float cellHeight = 0; // 0.1f - 1.0f

	//Agent
	float agentHeight = 0; // 0.1f - 5.0f
	float agentRadius = 0; // 0.0f - 5.0f
	float agentMaxClimb = 0; // 0.1f - 5.0f
	float agentMaxSlope = 0; // 0 - 90

	//Region
	float minRegionSize = 0; // 0 - 150
	float mergeRegionSize = 0; // 0 - 150

	Partitioning partitioning = WaterShed;

	std::array<bool, 3> myFilters{ false, false, false };

	//Polygonization
	float maxEdgeLength = 0; // 0 - 50
	float maxEdgeError = 0; // 0.1f - 3.0f 
	float vertsPerPoly = 3; // DONT CHANGE THIS

	//Detail Mesh
	float sampleDistance = 0; // 0 - 16
	float maxSampleError = 0; // 0 - 16


};

class GameObject;
class NavMesh;
class ModelInstance;
class NavMeshExporter
{
public:
	NavMeshExporter() = default;
	~NavMeshExporter() = default;
	static MeshData ExportScene(NavmeshBakingSettings aNavmeshSetting);
	static std::shared_ptr<NavMesh> BakeNavMesh(NavmeshBakingSettings aNavmeshSetting);

private:
	static void AddObjectToNavmesh(GameObject* anObject);
	static std::vector<ModelInstance*> myObjectsToExport;
	static std::vector<std::shared_ptr<Model>> myModels;
};
#pragma once
#include "Assets\MeshData.h"
#include "GameObjects\Component.h"
#include "..\Detour\DetourNavMesh.h"
#include "..\Detour\DetourNavMeshQuery.h"
#include "Navigation/NavMeshExporter.h"

enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};
enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};

class NavMesh : public Asset
{
public:

	static int GetDistance(const Vector3f& aPointA, const Vector3f& aPointB)
	{
		float dstX = abs(aPointA.x - aPointB.x);
		float dstZ = abs(aPointA.z - aPointB.z);

		if (dstX > dstZ)
			return static_cast<int>(roundf(14 * dstZ + 10 * (dstX - dstZ)));
		return static_cast<int>(roundf(14 * dstX + 10 * (dstZ - dstX)));
	}
	/*struct NavPoint
	{
		Vector3f position;
		int gCost = INT_MAX;
		int hCost = INT_MAX;
		std::vector<NavPoint*> neighbors;
		NavPoint* parent = nullptr;
		int index = -1;
	};*/


	struct NavMeshTileHeader
	{
		dtTileRef tileRef;
		int dataSize;
	};

	struct Node;
	struct Connection
	{
		Node* from = nullptr;
		Node* to = nullptr;
		Vector3f left;
		Vector3f right;
		int leftIndex;
		int rightIndex;
		float cost;
	};

	struct Node
	{
		int index = -1;
		Vector3f center;
		std::vector<Vector3f> vertices;
		std::vector<int> indices;
		Connection* parent = nullptr;
		std::vector<Connection> connections;
		int gCost = INT_MAX;
		int hCost = INT_MAX;
	};


	struct NavMeshData
	{
		std::shared_ptr<MeshData> meshData;
		std::vector<Vector3f> vertices;
		std::vector<Node> nodes;
	};

	NavMesh() = default;
	~NavMesh() = default;
	inline std::shared_ptr<NavMeshData> GetData() { return myData; }
	//inline std::vector<NavPoint>& GetPoints() { return myData->points; }
	inline std::vector<Node>& GetNodes() { return myData->nodes; }
	inline void SetMesh(std::shared_ptr<NavMeshData> aData) { myData = aData; }
	inline void SetMesh(std::shared_ptr<MeshData> aData) { myData->meshData = aData; }
	static std::shared_ptr<NavMeshData> CreateNavMesh(std::shared_ptr<MeshData> aMeshData);
	bool SamplePoint(Vector3f aPoint);
	bool SamplePoints(std::vector<Vector3f> aPoints);
	Vector3f GetClosestPoint(Vector3f aPoint);
	bool SampleBarycentricPoint(Vector3f aPoint);
	Vector3f GetNearestVertex(Vector3f aPoint);
	const NavmeshBakingSettings& GetBakingSettings() { return myBakingSettings; }
	
	void SetDtNavMesh(const dtNavMesh* aDtNavMesh) { myDtNavMesh = aDtNavMesh; }
	void SetDtNavMeshQuery(dtNavMeshQuery* aDtNavMeshQuery) { myDtNavMeshQuery = aDtNavMeshQuery; }
	void SetBakingSettings(const NavmeshBakingSettings& aBakingSettings) { myBakingSettings = aBakingSettings; }

	const dtNavMesh* GetDtNavMesh() { return myDtNavMesh; }
	dtNavMeshQuery* GetDtNavMeshQuery() { return myDtNavMeshQuery; }

private:
	friend class NavigationWindow;
	std::vector<std::string> myStrings;

	void SaveAsset(const char* aPath) override;
	void LoadFromPath(const char* aPath) override;
	std::shared_ptr<NavMeshData> myData;
	const dtNavMesh* myDtNavMesh = nullptr;
	dtNavMeshQuery* myDtNavMeshQuery = nullptr;
	NavmeshBakingSettings myBakingSettings;
};
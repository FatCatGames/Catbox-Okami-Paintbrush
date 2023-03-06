#pragma once
#include "EditorWindow.h"
#include "Recast.h"
#include "Navigation\NavMeshExporter.h"

class NavMesh;
class NavigationWindow : public EditorWindow
{
public:


private:
	void Render() override;
	std::shared_ptr<MeshData> mySelectedMeshData = nullptr;
	std::shared_ptr<NavMesh> mySelectedNavMesh = nullptr;
	std::shared_ptr<MeshData> mySavedMeshData = nullptr;
	MeshData myRCNavmeshData;

	std::vector<std::string> myPartitioningStyles = { "Watershed", "Monotone", "Layers" };

	NavmeshBakingSettings mySettings;

};
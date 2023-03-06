#include "stdafx.h"
#include "NavigationWindow.h"
#include "Navigation\NavMesh.h"

void NavigationWindow::Render()
{
	if (!BeginWindow("Navigation", true)) return;

	bool edit;

	if (mySelectedNavMesh != nullptr) 
	{
		if (ImGui::Button("Bake Navmesh"))
		{
			auto navMesh = NavMeshExporter::BakeNavMesh(mySettings);
			mySelectedNavMesh->SetDtNavMesh(navMesh->GetDtNavMesh());
			mySelectedNavMesh->SetDtNavMeshQuery(navMesh->GetDtNavMeshQuery());
			mySelectedNavMesh->SetMesh(navMesh->myData->meshData);
			//mySelectedNavMesh = std::make_shared<NavMesh>();
			//std::shared_ptr<NavMesh::NavMeshData> navmeshData = std::make_shared<NavMesh::NavMeshData>();
			//navmeshData->meshData = std::make_shared<MeshData>(myRCNavmeshData);
			//mySelectedNavMesh->SetMesh(navmeshData);
			if (!mySelectedNavMesh)
			{
				printerror("Navmesh Baking Failed");
			}
		}

		ImGui::Text("Rasterization");
		Catbox::DragFloat("Cell Size", &mySettings.cellSize, 0.005f, 0.0f, 1.0f, "%.2f");
		if (Catbox::DragFloat("Cell Height", &mySettings.cellHeight, 0.01f, 0.0f, 0.0f, "%.2f"))
		{
			if (mySettings.cellHeight > 1.0f)
			{
				mySettings.cellHeight = 1.0f;
			}
			else if (mySettings.cellHeight < 0.0f)
			{
				mySettings.cellHeight = 0.0f;
			}
		}

		ImGui::Text("Agent");
		if (Catbox::DragFloat("Height", &mySettings.agentHeight, 0.02f, 0.0f, 0.0f, "%.2f"))
		{
			if (mySettings.agentHeight > 5.0f)
			{
				mySettings.agentHeight = 5.0f;
			}
			else if (mySettings.agentHeight < 0.0f)
			{
				mySettings.agentHeight = 0.0f;
			}
		}
		if (Catbox::DragFloat("Radius", &mySettings.agentRadius, 0.02f, 0.0f, 0.0f, "%.2f"))
		{
			if (mySettings.agentRadius > 5.0f)
			{
				mySettings.agentRadius = 5.0f;
			}
			else if (mySettings.agentRadius < 0.0f)
			{
				mySettings.agentRadius = 0.0f;
			}
		}
		if (Catbox::DragFloat("Max Climb", &mySettings.agentMaxClimb, 0.02f, 0.0f, 0.0f, "%.2f"))
		{
			if (mySettings.agentMaxClimb > 5.0f)
			{
				mySettings.agentMaxClimb = 5.0f;
			}
			else if (mySettings.agentMaxClimb < 0.0f)
			{
				mySettings.agentMaxClimb = 0.0f;
			}
		}
		if (Catbox::DragFloat("Max Slope", &mySettings.agentMaxSlope, 0.5f, 0.0f, 0.0f, "%.0f"))
		{
			mySettings.agentMaxSlope = static_cast<int>(mySettings.agentMaxSlope);
			if (mySettings.agentMaxSlope > 90)
			{
				mySettings.agentMaxSlope = 90;
			}
			else if (mySettings.agentMaxSlope < 0)
			{
				mySettings.agentMaxSlope = 0;
			}
		}

		ImGui::Text("Region");
		if (Catbox::DragFloat("Min Region Size", &mySettings.minRegionSize, 1.0f, 0.0f, 0.0f, "%.0f"))
		{
			mySettings.minRegionSize = static_cast<int>(mySettings.minRegionSize);
			if (mySettings.minRegionSize > 150)
			{
				mySettings.minRegionSize = 150;
			}
			else if (mySettings.minRegionSize < 0)
			{
				mySettings.minRegionSize = 0;
			}
		}

		if (Catbox::DragFloat("Merged Region Size", &mySettings.mergeRegionSize, 1.0f, 0.0f, 0.0f, "%.0f"))
		{
			mySettings.mergeRegionSize = static_cast<int>(mySettings.mergeRegionSize);
			if (mySettings.mergeRegionSize > 150)
			{
				mySettings.mergeRegionSize = 150;
			}
			else if (mySettings.mergeRegionSize < 0)
			{
				mySettings.mergeRegionSize = 0;
			}
		}

		if (ImGui::BeginCombo("Partitioning", myPartitioningStyles[mySettings.partitioning].c_str()))
		{
			for (int i = 0; i < myPartitioningStyles.size(); i++)
			{
				bool selected = false;
				if (i == mySettings.partitioning)
				{
					selected = true;
				}
				if (ImGui::Selectable(myPartitioningStyles[i].c_str(), selected))
				{
					mySettings.partitioning = (Partitioning)i;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Text("Filters");
		ImGui::Checkbox("Low Hanging Obstacles", &mySettings.myFilters[0]);
		ImGui::Checkbox("Ledge Spans", &mySettings.myFilters[1]);
		ImGui::Checkbox("Walkable Low Height Spans", &mySettings.myFilters[2]);

		ImGui::Text("Polygonization");
		if (Catbox::DragFloat("Max Edge Length", &mySettings.maxEdgeLength, 0.3f, 0.0f, 0.0f, "%.0f"))
		{
			mySettings.maxEdgeLength = static_cast<int>(mySettings.maxEdgeLength);
			if (mySettings.maxEdgeLength > 50)
			{
				mySettings.maxEdgeLength = 50;
			}
			else if (mySettings.maxEdgeLength < 0)
			{
				mySettings.maxEdgeLength = 0;
			}
		}
		if (Catbox::DragFloat("Max Edge Error", &mySettings.maxEdgeError, 0.017f, 0.0f, 0.0f, "%.01f"))
		{
			if (mySettings.maxEdgeError > 3.0f)
			{
				mySettings.maxEdgeError = 3.0f;
			}
			else if (mySettings.maxEdgeError < 0.1f)
			{
				mySettings.maxEdgeError = 0.1f;
			}
		}

		ImGui::Text("Detail Mesh");
		if (Catbox::DragFloat("Sample Distance", &mySettings.sampleDistance, 0.08f, 0.0f, 0.0f, "%.0f"))
		{
			mySettings.sampleDistance = static_cast<int>(mySettings.sampleDistance);
			if (mySettings.sampleDistance > 16.0f)
			{
				mySettings.sampleDistance = 16.0f;
			}
			else if (mySettings.sampleDistance < 0.0f)
			{
				mySettings.sampleDistance = 0.0f;
			}
		}
		if (Catbox::DragFloat("Max Sample Error", &mySettings.maxSampleError, 0.08f, 0.0f, 0.0f, "%.0f"))
		{
			mySettings.maxSampleError = static_cast<int>(mySettings.maxSampleError);
			if (mySettings.maxSampleError > 16.0f)
			{
				mySettings.maxSampleError = 16.0f;
			}
			else if (mySettings.maxSampleError < 0.0f)
			{
				mySettings.maxSampleError = 0.0f;
			}
		}
	}

	ImGui::Separator();
	
	auto newNavMesh = AssetRegistry::GetInstance()->AssetDropdownMenu(mySelectedNavMesh.get(), "NavMesh to edit", edit);
	if (edit)
	{
		mySelectedNavMesh = newNavMesh;
		if (mySelectedNavMesh) 
		{
			mySavedMeshData = mySelectedMeshData = mySelectedNavMesh->GetData()->meshData;
		}
	}

	if (mySelectedNavMesh)
	{
		auto newMesh = AssetRegistry::GetInstance()->AssetDropdownMenu(mySelectedMeshData.get(), "Mesh", edit);

		if (edit)
		{
			mySelectedMeshData = newMesh;
			mySelectedNavMesh->SetMesh(NavMesh::CreateNavMesh(mySelectedMeshData));
		}

		bool popColor = false;
		if (mySelectedMeshData != mySavedMeshData)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.3f, 1 });
			ImGui::Text("Mesh not saved!");
			popColor = true;
		}

		if (ImGui::Button("Save"))
		{
			mySelectedNavMesh->SaveAsset(mySelectedNavMesh->GetPathStr().c_str());
			mySavedMeshData = mySelectedMeshData;
		}


		if (popColor) ImGui::PopStyleColor();
	}


	if (ImGui::Button("Create New"))
	{
		auto path = Catbox::SaveFile("NavMesh(*.nav)\0*.nav\0");

		if (!path.empty())
		{
			mySelectedNavMesh = CreateAsset<NavMesh>(path);
		}
	}

	if (mySelectedNavMesh)
	{
		Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(mySelectedNavMesh.get());
	}

	EndWindow();
}
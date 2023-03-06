#include "stdafx.h"
#include "NavMeshExporter.h"
#include "ComponentTools/SceneManager.h"
#include "Components/3D/ModelInstance.h"
#include "Assets\Model.h"
#include "Navigation\NavMesh.h"
#include "CommonUtilities\RapidJsonWrapper.h"
#include "Recast.h"
#include "RecastDebugDraw.h"
#include "RecastDump.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourDebugDraw.h"


std::vector<ModelInstance*> NavMeshExporter::myObjectsToExport;
std::vector<std::shared_ptr<Model>> NavMeshExporter::myModels;
MeshData NavMeshExporter::ExportScene(NavmeshBakingSettings aNavmeshSettings)
{
	myObjectsToExport.clear();

	auto& rootObjects = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetRootObjects();
	for (auto& obj : rootObjects) 
	{
		AddObjectToNavmesh(obj);
	}

	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& document = wrapper.GetDocument();
	auto& alloc = document.GetAllocator();
	auto objectsArray = wrapper.CreateArray();
	objectsArray.SetArray();

	std::vector<Vertex> navMeshVertices;
	std::vector<int> navMeshIndcies;
	std::vector<unsigned int> anotherNavMeshInd;
	std::vector<float> myVertexPositions;

	for (auto& mesh : myObjectsToExport)
	{
		if (!mesh->IsUsedForNavigation()) continue;
		auto model = mesh->GetModel();
		if (model)
		{
			//auto transform = mesh->GetTransform();
			//wrapper.SetTarget(document);
			//auto object = wrapper.CreateObject();
			//wrapper.SetTarget(object);

			//wrapper.SaveString("fbx", model->GetName().c_str());

			//auto positionArray = wrapper.CreateArray();
			//positionArray.SetArray();
			//Vector3f pos = transform->worldPos();
			//positionArray.PushBack(pos.x, alloc);
			//positionArray.PushBack(pos.y, alloc);
			//positionArray.PushBack(pos.z, alloc);
			//wrapper.SaveObject("Pos", positionArray);

			//auto rotationArray = wrapper.CreateArray();
			//rotationArray.SetArray();
			//Vector3f rot = transform->worldRot();
			//rotationArray.PushBack(rot.x, alloc);
			//rotationArray.PushBack(rot.y, alloc);
			//rotationArray.PushBack(rot.z, alloc);
			//wrapper.SaveObject("Rot", rotationArray);

			//auto scaleArray = wrapper.CreateArray();
			//scaleArray.SetArray();
			//Vector3f scale = transform->worldScale();
			//scaleArray.PushBack(scale.x, alloc);
			//scaleArray.PushBack(scale.y, alloc);
			//scaleArray.PushBack(scale.z, alloc);
			//wrapper.SaveObject("Size", scaleArray);

			//objectsArray.PushBack(object, alloc);

			std::unordered_map<unsigned int, size_t> navMeshIndciestemp;
			Vector3f tempRot = mesh->GetGameObject().GetTransform()->worldRot();

			Catbox::Matrix4x4<float> worldTransfrom = mesh->GetGameObject().GetTransform()->GetWorldTransformMatrix();
			Catbox::Matrix4x4<float> rotationMatrix = Catbox::Matrix4x4<float>();

			rotationMatrix = rotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(tempRot.z)) * rotationMatrix;
			rotationMatrix = rotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(tempRot.y)) * rotationMatrix;
			rotationMatrix = rotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(tempRot.x)) * rotationMatrix;

			worldTransfrom(4, 1) *= 100;
			worldTransfrom(4, 2) *= 100;
			worldTransfrom(4, 3) *= 100;

			for (int i = 0; i < model->GetModelParts().size(); i++)
			{
				for (int j = 0; j < model->GetModelPart(i)->myVertices.size(); j++)
				{
					model->GetModelPart(0);
					navMeshVertices.push_back(model->GetModelPart(i)->myVertices[j]); 
					navMeshVertices[navMeshVertices.size() - 1].position = ((navMeshVertices[navMeshVertices.size() - 1].position) * worldTransfrom) * 0.01f;
					myVertexPositions.push_back(navMeshVertices[navMeshVertices.size() - 1].position.x);
					myVertexPositions.push_back(navMeshVertices[navMeshVertices.size() - 1].position.y);
					myVertexPositions.push_back(navMeshVertices[navMeshVertices.size() - 1].position.z);
					navMeshIndciestemp.insert({ j,navMeshVertices.size() - 1 });
				}
				for (int j = 0; j < model->GetModelPart(i)->myIndices.size(); j += 3)
				{
					if (navMeshIndciestemp.find(model->GetModelPart(i)->myIndices[j]) != navMeshIndciestemp.end())
					{
						if (navMeshIndciestemp.find(model->GetModelPart(i)->myIndices[j + 1]) != navMeshIndciestemp.end())
						{
							if (navMeshIndciestemp.find(model->GetModelPart(i)->myIndices[j + 2]) != navMeshIndciestemp.end())
							{
								navMeshIndcies.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j]]);
								navMeshIndcies.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j + 1]]);
								navMeshIndcies.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j + 2]]);
								anotherNavMeshInd.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j]]);
								anotherNavMeshInd.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j + 1]]);
								anotherNavMeshInd.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j + 2]]);

							}
						}
					}
				}
			}
		}
		print("Done");
	}
	MeshData tempMeshdata;
	tempMeshdata.SetVertices(navMeshVertices);
	tempMeshdata.SetIndices(anotherNavMeshInd);
	myModels.push_back(std::make_shared<Model>(std::make_shared<MeshData>(tempMeshdata)));

	CreateAsset<Model>("NOTRC.fbx", myModels[myModels.size() - 1]);

	bool m_keepInterResults = true;
	float m_totalBuildTimeMs = 2.0f;

	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcPolyMeshDetail* m_dmesh;
	rcContext* m_ctx = new rcContext();
	
	unsigned int ntris = navMeshIndcies.size() / 3;

	rcConfig aConfig;
	std::memset(&aConfig, 0, sizeof(aConfig));
	aConfig.cs = aNavmeshSettings.cellSize;
	aConfig.ch = aNavmeshSettings.cellHeight;
	aConfig.walkableSlopeAngle = aNavmeshSettings.agentMaxSlope;
	aConfig.walkableHeight = (int)ceilf(aNavmeshSettings.agentHeight / aConfig.ch);
	aConfig.walkableClimb = (int)floorf(aNavmeshSettings.agentMaxClimb / aConfig.ch);
	aConfig.walkableRadius = (int)ceilf(aNavmeshSettings.agentRadius / aConfig.cs);
	aConfig.maxEdgeLen = (int)(aNavmeshSettings.maxEdgeLength / aConfig.cs);
	aConfig.maxSimplificationError = (aNavmeshSettings.maxEdgeError);
	aConfig.minRegionArea = (int)rcSqr(aNavmeshSettings.minRegionSize);		// Note: area = size*size
	aConfig.mergeRegionArea = (int)rcSqr(aNavmeshSettings.mergeRegionSize);	// Note: area = size*size
	aConfig.maxVertsPerPoly = (int)3;
	aConfig.detailSampleDist = aNavmeshSettings.sampleDistance /*m_detailSampleDist < 0.9f ? 0 : aConfig.cs * m_detailSampleDist;*/;
	aConfig.detailSampleMaxError = aConfig.ch * aNavmeshSettings.maxSampleError;
	aConfig.height = 200.0f;
	aConfig.width = 200.0;
	aConfig.bmin[0] = -100;
	aConfig.bmin[1] = -100;
	aConfig.bmin[2] = -100;
	aConfig.bmax[0] = 200;
	aConfig.bmax[1] = 200;
	aConfig.bmax[2] = 200;

	rcCalcGridSize(aConfig.bmin, aConfig.bmax, aConfig.cs, &aConfig.width, &aConfig.height);

	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		printerror("buildNavigation: Out of memory 'solid'.");
		//return false;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, aConfig.width, aConfig.height, aConfig.bmin, aConfig.bmax, aConfig.cs, aConfig.ch))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		printerror("buildNavigation: Could not create solid heightfield.");
		//return false;
	}

	m_triareas = new unsigned char[ntris];
	if (!m_triareas)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
		printerror("buildNavigation: Out of memory 'm_triareas' (%d).");
		//return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	std::memset(m_triareas, 0, (ntris) * sizeof(unsigned char));
	rcMarkWalkableTriangles(m_ctx, aConfig.walkableSlopeAngle, myVertexPositions.data(), navMeshVertices.size(), navMeshIndcies.data(), ntris, m_triareas);
	if (!rcRasterizeTriangles(m_ctx, myVertexPositions.data(), navMeshVertices.size(), navMeshIndcies.data(), m_triareas, ntris, *m_solid, aConfig.walkableClimb))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
		printerror("buildNavigation: Could not rasterize triangles.");
		//return false;
	}

	if (!m_keepInterResults)
	{
		delete[] m_triareas;
		m_triareas = 0;
	}

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	if (aNavmeshSettings.myFilters[0])
		rcFilterLowHangingWalkableObstacles(m_ctx, aConfig.walkableClimb, *m_solid);
	if (aNavmeshSettings.myFilters[1])
		rcFilterLedgeSpans(m_ctx, aConfig.walkableHeight, aConfig.walkableClimb, *m_solid);
	if (aNavmeshSettings.myFilters[2])
		rcFilterWalkableLowHeightSpans(m_ctx, aConfig.walkableHeight, *m_solid);


	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		printerror("buildNavigation: Out of memory 'chf'.");
		//return false;
	}
	if (!rcBuildCompactHeightfield(m_ctx, aConfig.walkableHeight, aConfig.walkableClimb, *m_solid, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		printerror("buildNavigation: Could not build compact data.");
		//return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, aConfig.walkableRadius, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		printerror("buildNavigation: Could not erode.");
		//return false;
	}

	// (Optional) Mark areas.
	//const ConvexVolume* vols = m_geom->getConvexVolumes();
	//for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
	//	rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);


	// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
	// There are 3 martitioning methods, each with some pros and cons:
	// 1) Watershed partitioning
	//   - the classic Recast partitioning
	//   - creates the nicest tessellation
	//   - usually slowest
	//   - partitions the heightfield into nice regions without holes or overlaps
	//   - the are some corner cases where this method creates produces holes and overlaps
	//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
	//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
	//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
	// 2) Monotone partioning
	//   - fastest
	//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
	//   - creates long thin polygons, which sometimes causes paths with detours
	//   * use this if you want fast navmesh generation
	// 3) Layer partitoining
	//   - quite fast
	//   - partitions the heighfield into non-overlapping regions
	//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
	//   - produces better triangles than monotone partitioning
	//   - does not have the corner cases of watershed partitioning
	//   - can be slow and create a bit ugly tessellation (still better than monotone)
	//     if you have large open areas with small obstacles (not a problem if you use tiles)
	//   * good choice to use for tiled navmesh with medium and small sized tiles

	if ((int)aNavmeshSettings.partitioning == 0)
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(m_ctx, *m_chf))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
			printerror("buildNavigation: Could not build distance field.");
			return MeshData();
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(m_ctx, *m_chf, 0, aConfig.minRegionArea, aConfig.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
			printerror("buildNavigation: Could not build watershed regions.");
			return MeshData();
		}
	}
	else if ((int)aNavmeshSettings.partitioning == 1)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, aConfig.minRegionArea, aConfig.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
			printerror("buildNavigation: Could not build monotone regions.");
			return MeshData();
		}
	}
	else // SAMPLE_PARTITION_LAYERS
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildLayerRegions(m_ctx, *m_chf, 0, aConfig.minRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
			printerror("buildNavigation: Could not build layer regions.");
			return MeshData();
		}
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		printerror("buildNavigation: Out of memory 'cset'.");
		return MeshData();
	}
	if (!rcBuildContours(m_ctx, *m_chf, aConfig.maxSimplificationError, aConfig.maxEdgeLen, *m_cset))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return MeshData();
		printerror("buildNavigation: Could not create contours.");
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return MeshData();
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, aConfig.maxVertsPerPoly, *m_pmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		printerror("buildNavigation: Could not triangulate contours.");
		return MeshData();
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
		printerror("buildNavigation: Out of memory 'pmdtl'.");
		return MeshData();
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, aConfig.detailSampleDist, aConfig.detailSampleMaxError, *m_dmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
		printerror("buildNavigation: Could not build detail mesh.");
		return MeshData();
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}

	std::vector<Vertex> myVertexVector;
	std::vector<unsigned int> myIndecies;

	auto dmesh = *m_dmesh;

	for (int i = 0; i < dmesh.nmeshes; ++i)
	{
		const unsigned int* m = &dmesh.meshes[i * 4];
		const unsigned int bverts = m[0];
		const unsigned int btris = m[2];
		const int ntris = (int)m[3];
		const float* verts = &dmesh.verts[bverts * 3];
		const unsigned char* tris = &dmesh.tris[btris * 4];

		for (int j = 0; j < ntris; ++j)
		{
			const float* ppos[3]{ &verts[tris[j * 4 + 0] * 3],&verts[tris[j * 4 + 1] * 3], &verts[tris[j * 4 + 2] * 3] };
			Vertex tempVertex;
			Vertex tempVertex1;
			Vertex tempVertex2;
			tempVertex.position = Vector4f{ *(ppos[0]),  *(ppos[0] + 1), *(ppos[0] + 2), 1 };
			tempVertex1.position = Vector4f{ *(ppos[1]),  *(ppos[1] + 1), *(ppos[1] + 2), 1 };
			tempVertex2.position = Vector4f{ *(ppos[2]),  *(ppos[2] + 1), *(ppos[2] + 2), 1 };

			tempVertex.normal = Vector3f::up();
			tempVertex1.normal = Vector3f::up();
			tempVertex2.normal = Vector3f::up();


			myVertexVector.push_back(tempVertex);
			myVertexVector.push_back(tempVertex1);
			myVertexVector.push_back(tempVertex2);

			myIndecies.push_back(myVertexVector.size() - 3);
			myIndecies.push_back(myVertexVector.size() - 2);
			myIndecies.push_back(myVertexVector.size() - 1);
			//GameObject* one = Instantiate();
			//one->GetTransform()->SetWorldPos({ tempVertex.position.x, tempVertex.position.y, tempVertex.position.z });
			//GameObject* two = Instantiate();
			//two->GetTransform()->SetWorldPos({ tempVertex1.position.x, tempVertex1.position.y, tempVertex1.position.z });
			//GameObject* three = Instantiate();
			//three->GetTransform()->SetWorldPos({ tempVertex2.position.x, tempVertex2.position.y, tempVertex2.position.z });
		}
	}

	MeshData navmeshMeshadata;
	if (myVertexVector.size() > 0) 
	{
		navmeshMeshadata.SetVertices(myVertexVector);
		navmeshMeshadata.SetIndices(myIndecies);
		auto navmesh = (std::make_shared<Model>(std::make_shared<MeshData>(navmeshMeshadata)));
		CreateAsset<Model>("markus_temp_navmesh.fbx", navmesh);
	}

 
	if (aConfig.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;

		// Update poly flags from areas.
		//for (int i = 0; i < m_pmesh->npolys; ++i)
		//{
		//	if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
		//		m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

		//	if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
		//		m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
		//		m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
		//	{
		//		m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
		//	}
		//	else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
		//	{
		//		m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
		//	}
		//	else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
		//	{
		//		m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
		//	}
		//}


		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;
		//params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
		//params.offMeshConRad = m_geom->getOffMeshConnectionRads();
		//params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
		//params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
		//params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
		//params.offMeshConUserID = m_geom->getOffMeshConnectionId();
		//params.offMeshConCount = m_geom->getOffMeshConnectionCount();
		params.walkableHeight = aConfig.walkableHeight;
		params.walkableRadius = aConfig.walkableRadius;
		params.walkableClimb = aConfig.walkableClimb;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = aConfig.cs;
		params.ch = aConfig.ch;
		params.buildBvTree = true;

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
			//return false;
		}

		dtNavMesh* m_navMesh = dtAllocNavMesh();
		dtNavMeshQuery* m_navQuery = dtAllocNavMeshQuery();
		if (!m_navMesh)
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
			//return false;
		}

		dtStatus status;

		status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
			//return false;
		}

		status = m_navQuery->init(m_navMesh, 2048);
		if (dtStatusFailed(status))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
			//return false;
		}
	}

	//document.AddMember("Models", objectsArray, document.GetAllocator());
	//wrapper.SaveDocument(("Temp/ExportedLevels/" + Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetName()+ ".json").c_str());
	return navmeshMeshadata;
}

std::shared_ptr<NavMesh> NavMeshExporter::BakeNavMesh(NavmeshBakingSettings aNavmeshSettings)
{
	myObjectsToExport.clear();
	std::shared_ptr<NavMesh> returnNavmesh = nullptr;
	auto& rootObjects = Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetRootObjects();
	for (auto& obj : rootObjects)
	{
		AddObjectToNavmesh(obj);
	}

	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& document = wrapper.GetDocument();
	auto& alloc = document.GetAllocator();
	auto objectsArray = wrapper.CreateArray();
	objectsArray.SetArray();

	std::vector<Vertex> navMeshVertices;
	std::vector<int> navMeshIndcies;
	std::vector<unsigned int> anotherNavMeshInd;
	std::vector<float> myVertexPositions;

	for (auto& mesh : myObjectsToExport)
	{
		if (!mesh->IsUsedForNavigation()) continue;
		auto model = mesh->GetModel();
		if (model)
		{
			//auto transform = mesh->GetTransform();
			//wrapper.SetTarget(document);
			//auto object = wrapper.CreateObject();
			//wrapper.SetTarget(object);

			//wrapper.SaveString("fbx", model->GetName().c_str());

			//auto positionArray = wrapper.CreateArray();
			//positionArray.SetArray();
			//Vector3f pos = transform->worldPos();
			//positionArray.PushBack(pos.x, alloc);
			//positionArray.PushBack(pos.y, alloc);
			//positionArray.PushBack(pos.z, alloc);
			//wrapper.SaveObject("Pos", positionArray);

			//auto rotationArray = wrapper.CreateArray();
			//rotationArray.SetArray();
			//Vector3f rot = transform->worldRot();
			//rotationArray.PushBack(rot.x, alloc);
			//rotationArray.PushBack(rot.y, alloc);
			//rotationArray.PushBack(rot.z, alloc);
			//wrapper.SaveObject("Rot", rotationArray);

			//auto scaleArray = wrapper.CreateArray();
			//scaleArray.SetArray();
			//Vector3f scale = transform->worldScale();
			//scaleArray.PushBack(scale.x, alloc);
			//scaleArray.PushBack(scale.y, alloc);
			//scaleArray.PushBack(scale.z, alloc);
			//wrapper.SaveObject("Size", scaleArray);

			//objectsArray.PushBack(object, alloc);

			std::unordered_map<unsigned int, size_t> navMeshIndciestemp;
			Vector3f tempRot = mesh->GetGameObject().GetTransform()->worldRot();

			Catbox::Matrix4x4<float> worldTransfrom = mesh->GetGameObject().GetTransform()->GetWorldTransformMatrix();
			Catbox::Matrix4x4<float> rotationMatrix = Catbox::Matrix4x4<float>();

			rotationMatrix = rotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(tempRot.z)) * rotationMatrix;
			rotationMatrix = rotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(tempRot.y)) * rotationMatrix;
			rotationMatrix = rotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(tempRot.x)) * rotationMatrix;

			worldTransfrom(4, 1) *= 100;
			worldTransfrom(4, 2) *= 100;
			worldTransfrom(4, 3) *= 100;

			for (int i = 0; i < model->GetModelParts().size(); i++)
			{
				for (int j = 0; j < model->GetModelPart(i)->myVertices.size(); j++)
				{
					model->GetModelPart(0);
					navMeshVertices.push_back(model->GetModelPart(i)->myVertices[j]);
					navMeshVertices[navMeshVertices.size() - 1].position = ((navMeshVertices[navMeshVertices.size() - 1].position) * worldTransfrom) * 0.01f;
					myVertexPositions.push_back(navMeshVertices[navMeshVertices.size() - 1].position.x);
					myVertexPositions.push_back(navMeshVertices[navMeshVertices.size() - 1].position.y);
					myVertexPositions.push_back(navMeshVertices[navMeshVertices.size() - 1].position.z);
					navMeshIndciestemp.insert({ j,navMeshVertices.size() - 1 });
				}
				for (int j = 0; j < model->GetModelPart(i)->myIndices.size(); j += 3)
				{
					if (navMeshIndciestemp.find(model->GetModelPart(i)->myIndices[j]) != navMeshIndciestemp.end())
					{
						if (navMeshIndciestemp.find(model->GetModelPart(i)->myIndices[j + 1]) != navMeshIndciestemp.end())
						{
							if (navMeshIndciestemp.find(model->GetModelPart(i)->myIndices[j + 2]) != navMeshIndciestemp.end())
							{
								navMeshIndcies.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j]]);
								navMeshIndcies.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j + 1]]);
								navMeshIndcies.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j + 2]]);
								anotherNavMeshInd.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j]]);
								anotherNavMeshInd.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j + 1]]);
								anotherNavMeshInd.push_back(navMeshIndciestemp[model->GetModelPart(i)->myIndices[j + 2]]);

							}
						}
					}
				}
			}
		}
		print("Done");
	}
	MeshData tempMeshdata;
	tempMeshdata.SetVertices(navMeshVertices);
	tempMeshdata.SetIndices(anotherNavMeshInd);
	myModels.push_back(std::make_shared<Model>(std::make_shared<MeshData>(tempMeshdata)));

	CreateAsset<Model>("NOTRC.fbx", myModels[myModels.size() - 1]);

	bool m_keepInterResults = true;
	float m_totalBuildTimeMs = 2.0f;

	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcPolyMeshDetail* m_dmesh;
	rcContext* m_ctx = new rcContext();

	unsigned int ntris = navMeshIndcies.size() / 3;

	rcConfig aConfig;
	std::memset(&aConfig, 0, sizeof(aConfig));
	aConfig.cs = aNavmeshSettings.cellSize;
	aConfig.ch = aNavmeshSettings.cellHeight;
	aConfig.walkableSlopeAngle = aNavmeshSettings.agentMaxSlope;
	aConfig.walkableHeight = (int)ceilf(aNavmeshSettings.agentHeight / aConfig.ch);
	aConfig.walkableClimb = (int)floorf(aNavmeshSettings.agentMaxClimb / aConfig.ch);
	aConfig.walkableRadius = (int)ceilf(aNavmeshSettings.agentRadius / aConfig.cs);
	aConfig.maxEdgeLen = (int)(aNavmeshSettings.maxEdgeLength / aConfig.cs);
	aConfig.maxSimplificationError = (aNavmeshSettings.maxEdgeError);
	aConfig.minRegionArea = (int)rcSqr(aNavmeshSettings.minRegionSize);		// Note: area = size*size
	aConfig.mergeRegionArea = (int)rcSqr(aNavmeshSettings.mergeRegionSize);	// Note: area = size*size
	aConfig.maxVertsPerPoly = (int)3;
	aConfig.detailSampleDist = aNavmeshSettings.sampleDistance /*m_detailSampleDist < 0.9f ? 0 : aConfig.cs * m_detailSampleDist;*/;
	aConfig.detailSampleMaxError = aConfig.ch * aNavmeshSettings.maxSampleError;
	aConfig.height = 200.0f;
	aConfig.width = 200.0;
	aConfig.bmin[0] = -200;
	aConfig.bmin[1] = -200;
	aConfig.bmin[2] = -200;
	aConfig.bmax[0] = 200;
	aConfig.bmax[1] = 200;
	aConfig.bmax[2] = 200;

	rcCalcGridSize(aConfig.bmin, aConfig.bmax, aConfig.cs, &aConfig.width, &aConfig.height);

	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		printerror("buildNavigation: Out of memory 'solid'.");
		//return false;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, aConfig.width, aConfig.height, aConfig.bmin, aConfig.bmax, aConfig.cs, aConfig.ch))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		printerror("buildNavigation: Could not create solid heightfield.");
		//return false;
	}

	m_triareas = new unsigned char[ntris];
	if (!m_triareas)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
		printerror("buildNavigation: Out of memory 'm_triareas' (%d).");
		//return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	std::memset(m_triareas, 0, (ntris) * sizeof(unsigned char));
	rcMarkWalkableTriangles(m_ctx, aConfig.walkableSlopeAngle, myVertexPositions.data(), navMeshVertices.size(), navMeshIndcies.data(), ntris, m_triareas);
	if (!rcRasterizeTriangles(m_ctx, myVertexPositions.data(), navMeshVertices.size(), navMeshIndcies.data(), m_triareas, ntris, *m_solid, aConfig.walkableClimb))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
		printerror("buildNavigation: Could not rasterize triangles.");
		//return false;
	}

	if (!m_keepInterResults)
	{
		delete[] m_triareas;
		m_triareas = 0;
	}

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	if (aNavmeshSettings.myFilters[0])
		rcFilterLowHangingWalkableObstacles(m_ctx, aConfig.walkableClimb, *m_solid);
	if (aNavmeshSettings.myFilters[1])
		rcFilterLedgeSpans(m_ctx, aConfig.walkableHeight, aConfig.walkableClimb, *m_solid);
	if (aNavmeshSettings.myFilters[2])
		rcFilterWalkableLowHeightSpans(m_ctx, aConfig.walkableHeight, *m_solid);


	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		printerror("buildNavigation: Out of memory 'chf'.");
		//return false;
	}
	if (!rcBuildCompactHeightfield(m_ctx, aConfig.walkableHeight, aConfig.walkableClimb, *m_solid, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		printerror("buildNavigation: Could not build compact data.");
		//return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, aConfig.walkableRadius, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		printerror("buildNavigation: Could not erode.");
		//return false;
	}

	// (Optional) Mark areas.
	//const ConvexVolume* vols = m_geom->getConvexVolumes();
	//for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
	//	rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);


	// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
	// There are 3 martitioning methods, each with some pros and cons:
	// 1) Watershed partitioning
	//   - the classic Recast partitioning
	//   - creates the nicest tessellation
	//   - usually slowest
	//   - partitions the heightfield into nice regions without holes or overlaps
	//   - the are some corner cases where this method creates produces holes and overlaps
	//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
	//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
	//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
	// 2) Monotone partioning
	//   - fastest
	//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
	//   - creates long thin polygons, which sometimes causes paths with detours
	//   * use this if you want fast navmesh generation
	// 3) Layer partitoining
	//   - quite fast
	//   - partitions the heighfield into non-overlapping regions
	//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
	//   - produces better triangles than monotone partitioning
	//   - does not have the corner cases of watershed partitioning
	//   - can be slow and create a bit ugly tessellation (still better than monotone)
	//     if you have large open areas with small obstacles (not a problem if you use tiles)
	//   * good choice to use for tiled navmesh with medium and small sized tiles

	if ((int)aNavmeshSettings.partitioning == 0)
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(m_ctx, *m_chf))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
			printerror("buildNavigation: Could not build distance field.");
			return nullptr;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(m_ctx, *m_chf, 0, aConfig.minRegionArea, aConfig.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
			printerror("buildNavigation: Could not build watershed regions.");
			return nullptr;
		}
	}
	else if ((int)aNavmeshSettings.partitioning == 1)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, aConfig.minRegionArea, aConfig.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
			printerror("buildNavigation: Could not build monotone regions.");
			return nullptr;
		}
	}
	else // SAMPLE_PARTITION_LAYERS
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildLayerRegions(m_ctx, *m_chf, 0, aConfig.minRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
			printerror("buildNavigation: Could not build layer regions.");
			return nullptr;
		}
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		printerror("buildNavigation: Out of memory 'cset'.");
		return nullptr;
	}
	if (!rcBuildContours(m_ctx, *m_chf, aConfig.maxSimplificationError, aConfig.maxEdgeLen, *m_cset))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return nullptr;
		printerror("buildNavigation: Could not create contours.");
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return nullptr;
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, aConfig.maxVertsPerPoly, *m_pmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		printerror("buildNavigation: Could not triangulate contours.");
		return nullptr;
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
		printerror("buildNavigation: Out of memory 'pmdtl'.");
		return nullptr;
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, aConfig.detailSampleDist, aConfig.detailSampleMaxError, *m_dmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
		printerror("buildNavigation: Could not build detail mesh.");
		return nullptr;
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}

	std::vector<Vertex> myVertexVector;
	std::vector<unsigned int> myIndecies;

	auto dmesh = *m_dmesh;

	for (int i = 0; i < dmesh.nmeshes; ++i)
	{
		const unsigned int* m = &dmesh.meshes[i * 4];
		const unsigned int bverts = m[0];
		const unsigned int btris = m[2];
		const int ntris = (int)m[3];
		const float* verts = &dmesh.verts[bverts * 3];
		const unsigned char* tris = &dmesh.tris[btris * 4];

		for (int j = 0; j < ntris; ++j)
		{
			const float* ppos[3]{ &verts[tris[j * 4 + 0] * 3],&verts[tris[j * 4 + 1] * 3], &verts[tris[j * 4 + 2] * 3] };
			Vertex tempVertex;
			Vertex tempVertex1;
			Vertex tempVertex2;
			tempVertex.position = Vector4f{ *(ppos[0]),  *(ppos[0] + 1), *(ppos[0] + 2), 1 };
			tempVertex1.position = Vector4f{ *(ppos[1]),  *(ppos[1] + 1), *(ppos[1] + 2), 1 };
			tempVertex2.position = Vector4f{ *(ppos[2]),  *(ppos[2] + 1), *(ppos[2] + 2), 1 };

			tempVertex.normal = Vector3f::up();
			tempVertex1.normal = Vector3f::up();
			tempVertex2.normal = Vector3f::up();


			myVertexVector.push_back(tempVertex);
			myVertexVector.push_back(tempVertex1);
			myVertexVector.push_back(tempVertex2);

			myIndecies.push_back(myVertexVector.size() - 3);
			myIndecies.push_back(myVertexVector.size() - 2);
			myIndecies.push_back(myVertexVector.size() - 1);
			//GameObject* one = Instantiate();
			//one->GetTransform()->SetWorldPos({ tempVertex.position.x, tempVertex.position.y, tempVertex.position.z });
			//GameObject* two = Instantiate();
			//two->GetTransform()->SetWorldPos({ tempVertex1.position.x, tempVertex1.position.y, tempVertex1.position.z });
			//GameObject* three = Instantiate();
			//three->GetTransform()->SetWorldPos({ tempVertex2.position.x, tempVertex2.position.y, tempVertex2.position.z });
		}
	}

	MeshData navmeshMeshadata;
	if (myVertexVector.size() > 0)
	{
		navmeshMeshadata.SetVertices(myVertexVector);
		navmeshMeshadata.SetIndices(myIndecies);
		auto navmesh = (std::make_shared<Model>(std::make_shared<MeshData>(navmeshMeshadata)));
		CreateAsset<Model>("markus_temp_navmesh.fbx", navmesh);
	}


	if (aConfig.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;

		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
				m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

			if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}


		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;
		//params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
		//params.offMeshConRad = m_geom->getOffMeshConnectionRads();
		//params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
		//params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
		//params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
		//params.offMeshConUserID = m_geom->getOffMeshConnectionId();
		//params.offMeshConCount = m_geom->getOffMeshConnectionCount();
		params.walkableHeight = aConfig.walkableHeight;
		params.walkableRadius = aConfig.walkableRadius;
		params.walkableClimb = aConfig.walkableClimb;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = aConfig.cs;
		params.ch = aConfig.ch;
		params.buildBvTree = true;

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
			return nullptr;
		}

		dtNavMesh* m_navMesh = dtAllocNavMesh();
		dtNavMeshQuery* m_navQuery = dtAllocNavMeshQuery();
		if (!m_navMesh)
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
			return nullptr;
		}

		dtStatus status;

		status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
			return nullptr;
		}

		status = m_navQuery->init(m_navMesh, 2048);
		if (dtStatusFailed(status))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
			return nullptr;
		}
		std::shared_ptr<NavMesh::NavMeshData> myNavMeshData = std::make_shared<NavMesh::NavMeshData>();
		returnNavmesh = CreateAsset<NavMesh>("Markus_Temp.nav");
		returnNavmesh->SetDtNavMesh(m_navMesh);
		returnNavmesh->SetDtNavMeshQuery(m_navQuery);
		myNavMeshData->meshData = std::make_shared<MeshData>(navmeshMeshadata);
		returnNavmesh->SetMesh(myNavMeshData);
		returnNavmesh->SetBakingSettings(aNavmeshSettings);
	}

	//document.AddMember("Models", objectsArray, document.GetAllocator());
	//wrapper.SaveDocument(("Temp/ExportedLevels/" + Engine::GetInstance()->GetSceneManager()->GetCurrentScene()->GetName()+ ".json").c_str());
	return returnNavmesh;
}

void NavMeshExporter::AddObjectToNavmesh(GameObject* anObject)
{
	for (auto& child : anObject->GetTransform()->GetChildren())
	{
		AddObjectToNavmesh(child->GetGameObject());
	}

	auto model = anObject->GetComponent<ModelInstance>();
	if (model) 
	{
		myObjectsToExport.push_back(model);
	}
}
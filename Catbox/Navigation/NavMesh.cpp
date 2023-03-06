#include "stdafx.h"
#include "NavMesh.h"
#include "NavMeshExporter.h"
#include "Recast.h"
#include "DetourNavMesh.h"




std::shared_ptr<NavMesh::NavMeshData> NavMesh::CreateNavMesh(std::shared_ptr<MeshData> aMeshData)
{
	std::shared_ptr<NavMeshData> data = std::make_shared<NavMeshData>();
	data->meshData = aMeshData;
	data->nodes.clear();

	//Create nodes
	auto& vertices = data->meshData->myVertices;
	auto& indices = data->meshData->myIndices;

	//for (size_t i = 0; i < vertices.size(); i++)
	//{
	//	data->vertices.push_back({ vertices[i].position.x, vertices[i].position.y, vertices[i].position.z });
	//}

	//int nodeIndex = 0;
	//for (size_t i = 0; i < indices.size(); i += 3)
	//{
	//	Node node;
	//	node.index = nodeIndex++;

	//	int i1 = indices[i];
	//	int i2 = indices[i + 1];
	//	int i3 = indices[i + 2];

	//	node.indices.push_back(i1);
	//	node.indices.push_back(i2);
	//	node.indices.push_back(i3);
	//	node.vertices.push_back(data->vertices[i1]);
	//	node.vertices.push_back(data->vertices[i2]);
	//	node.vertices.push_back(data->vertices[i3]);
	//	node.center = (data->vertices[i1] + data->vertices[i2] + data->vertices[i3]) / 3.f;
	//	data->nodes.push_back(node);
	//}

	////Link together nodes
	//for (size_t i = 0; i < data->nodes.size(); i++)
	//{
	//	for (size_t j = i + 1; j < data->nodes.size(); j++)
	//	{
	//		std::vector<int> sharedIndices;
	//		bool startWithLeft = true;

	//		for (size_t n1 = 0; n1 < data->nodes[i].indices.size(); n1++)
	//		{
	//			int index1 = data->nodes[i].indices[n1];
	//			for (size_t n2 = 0; n2 < data->nodes[j].indices.size(); n2++)
	//			{
	//				if (index1 == data->nodes[j].indices[n2])
	//				{
	//					sharedIndices.push_back(index1);
	//					break;
	//				}
	//			}
	//		}

	//		if (sharedIndices.size() >= 2)
	//		{
	//			Connection connectionA;
	//			Connection connectionB;
	//			connectionB.to = connectionA.from = &data->nodes[i];
	//			connectionA.to = connectionB.from = &data->nodes[j];

	//			Vector3f vert1 = data->vertices[sharedIndices[0]];
	//			/*Vector3f delta = (vert1 - connectionA.from->center).GetNormalized();
	//			Vector3f forward = (connectionA.to->center - connectionA.from->center).GetNormalized();
	//			bool startWithLeft = delta.Cross(forward).y < 0;*/

	//			bool startWithLeft = ((connectionA.to->center.x - connectionA.from->center.x) *
	//				(vert1.z - connectionA.from->center.z) -
	//				(connectionA.to->center.z - connectionA.from->center.z) *
	//				(vert1.x - connectionA.from->center.x)) > 0;

	//			int indexR = 0 + 0;
	//			int indexL = 1 - 0;

	//			connectionA.left = connectionB.left = data->vertices[sharedIndices[indexL]];
	//			connectionA.right = connectionB.right = data->vertices[sharedIndices[indexR]];

	//			connectionA.leftIndex = connectionB.leftIndex = indexL;
	//			connectionA.rightIndex = connectionB.rightIndex = indexR;

	//			connectionA.cost = connectionB.cost = NavMesh::GetDistance(connectionA.from->center, connectionA.to->center);

	//			data->nodes[i].connections.push_back(connectionA);
	//			data->nodes[j].connections.push_back(connectionB);
	//		}
	//	}
	//}

	return data;
}

void NavMesh::SaveAsset(const char* aPath)
{
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& document = wrapper.CreateDocument();
	auto& alloc = document.GetAllocator();


	// VERSION 1
	/*auto vertexArray = wrapper.CreateArray();
	vertexArray.SetArray();
	wrapper.SetTarget(vertexArray);
	for (auto& v : myData->meshData->myVertices)
	{
		vertexArray.PushBack(v.position.x, alloc);
		vertexArray.PushBack(v.position.y, alloc);
		vertexArray.PushBack(v.position.z, alloc);
	}
	document.AddMember("Vertices", vertexArray, alloc);

	auto indexArray = wrapper.CreateArray();
	indexArray.SetArray();
	wrapper.SetTarget(indexArray);
	for (int i = 0; i < myData->meshData->myIndices.size(); i++)
	{
		indexArray.PushBack(myData->meshData->myIndices[i], alloc);
	}
	document.AddMember("Indices", indexArray, alloc);


	auto nodeArray = wrapper.CreateArray();
	nodeArray.SetArray();
	for (auto& point : myData->nodes)
	{
		wrapper.SetTarget(nodeArray);

		auto componentObj = wrapper.CreateObject();
		wrapper.SetTarget(componentObj);


		auto neighborsArray = wrapper.CreateArray();
		neighborsArray.SetArray();

		for (auto& c : point.connections)
		{
			neighborsArray.PushBack(c.to->index, alloc);
		}

		componentObj.AddMember("Neighbors", neighborsArray, alloc);
		nodeArray.PushBack(componentObj, alloc);
	}

	document.AddMember("Nodes", nodeArray, alloc);*/


	//VERSION 2

	auto vertexArray = wrapper.CreateArray();
	vertexArray.SetArray();
	wrapper.SetTarget(vertexArray);
	for (auto& v : myData->meshData->myVertices)
	{
		vertexArray.PushBack(v.position.x, alloc);
		vertexArray.PushBack(v.position.y, alloc);
		vertexArray.PushBack(v.position.z, alloc);
	}
	document.AddMember("Vertices", vertexArray, alloc);

	auto indexArray = wrapper.CreateArray();
	indexArray.SetArray();
	wrapper.SetTarget(indexArray);
	for (int i = 0; i < myData->meshData->myIndices.size(); i++)
	{
		indexArray.PushBack(myData->meshData->myIndices[i], alloc);
	}
	document.AddMember("Indices", indexArray, alloc);

	int numTiles = 0;

	if (myDtNavMesh) 
	{
		for (int i = 0; i < myDtNavMesh->getMaxTiles(); ++i)
		{
			const dtMeshTile* tile = myDtNavMesh->getTile(i);
			if (!tile || !tile->header || !tile->dataSize) continue;
			numTiles++;
		}

		document.AddMember("Number of Tiles", numTiles, alloc);

		auto params = myDtNavMesh->getParams();
		document.AddMember("Max Polys", params->maxPolys, alloc);
		document.AddMember("Max Tiles", params->maxTiles, alloc);

		auto originPos = wrapper.CreateArray();
		originPos.SetArray();
		originPos.PushBack(params->orig[0], alloc);
		originPos.PushBack(params->orig[1], alloc);
		originPos.PushBack(params->orig[2], alloc);
		document.AddMember("Origin", originPos, alloc);

		document.AddMember("Tile Height", params->tileHeight, alloc);
		document.AddMember("Tile Width", params->tileWidth, alloc);

		// Store tiles.
		auto tiles = wrapper.CreateArray();
		tiles.SetArray();
		for (int i = 0; i < myDtNavMesh->getMaxTiles(); ++i)
		{
			const dtMeshTile* tile = myDtNavMesh->getTile(i);
			if (!tile || !tile->header || !tile->dataSize) continue;

			NavMeshTileHeader tileHeader;
			tileHeader.tileRef = myDtNavMesh->getTileRef(tile);
			tileHeader.dataSize = tile->dataSize;
			tiles.PushBack(tileHeader.tileRef, alloc);
			tiles.PushBack(tileHeader.dataSize, alloc);
			std::string tempString;
			for (int j = 0; j < tileHeader.dataSize; j++)
			{
				tempString.push_back(tile->data[j]);
			}
			myStrings.push_back(tempString);
			auto tempValue = wrapper.CreateObject();
			tempValue.SetString(rapidjson::StringRef(myStrings[0].c_str(), tile->dataSize));
			tiles.PushBack(tempValue, alloc);

		}
		document.AddMember("Tiles", tiles, alloc);

	}
	//auto connectionsArray = wrapper.CreateArray();
	//connectionsArray.SetArray();
	//for (auto& node : myData->nodes)
	//{
	//	wrapper.SetTarget(connectionsArray);

	//	for (auto& c : node.connections)
	//	{
	//		auto componentObj = wrapper.CreateObject();
	//		wrapper.SetTarget(componentObj);
	//		wrapper.SaveValue<DataType::Int>("n1", c.from->index);
	//		wrapper.SaveValue<DataType::Int>("n2", c.to->index);
	//		wrapper.SaveValue<DataType::Int>("l", c.leftIndex);
	//		wrapper.SaveValue<DataType::Int>("r", c.rightIndex);
	//		connectionsArray.PushBack(componentObj, alloc);
	//	}
	//}

	//document.AddMember("Connections", connectionsArray, alloc);

	wrapper.SaveDocumentCompressed(aPath);
}

void NavMesh::LoadFromPath(const char* aPath)
{
	//myData = std::make_shared<NavMeshData>();
	//std::vector<std::vector<int>> neighbors;

	//auto wrapper = RapidJsonWrapper::GetInstance();
	//auto& document = wrapper->LoadDocument(aPath);

	//myData->meshData = std::make_shared<MeshData>();

	//std::vector<Vertex> vertices;
	//std::vector<unsigned int> indices;

	//if (document.HasMember("Vertices"))
	//{
	//	auto& verticesArray = document["Vertices"].GetArray();
	//	for (int i = 0; i < verticesArray.Size(); i += 3)
	//	{
	//		Vertex v;
	//		v.position = { verticesArray[i].GetFloat(), verticesArray[i + 1].GetFloat(), verticesArray[i + 2].GetFloat(), 1 };
	//		vertices.push_back(v);
	//	}
	//}


	//if (document.HasMember("Indices"))
	//{
	//	for (auto& i : document["Indices"].GetArray())
	//	{
	//		indices.push_back(i.GetInt());
	//	}
	//}

	//myData->meshData->SetVertices(vertices);
	//myData->meshData->SetIndices(indices);

	//int i = 0;
	//for (auto& node : document["Nodes"].GetArray())
	//{
	//	neighbors.emplace_back();
	//	myData->nodes.emplace_back();
	//	myData->nodes.back().index = i;

	//	if (node.HasMember("Neighbors"))
	//	{
	//		for (auto& n : node["Neighbors"].GetArray())
	//		{
	//			neighbors.back().push_back(n.GetInt());
	//		}
	//	}

	//	if (document.HasMember("Vertices"))
	//	{
	//		Vector3f pos1 = { vertices[indices[i * 3]].position.x, vertices[indices[i * 3]].position.y, vertices[indices[i * 3]].position.z };
	//		Vector3f pos2 = { vertices[indices[(i * 3) + 1]].position.x, vertices[indices[(i * 3) + 1]].position.y, vertices[indices[(i * 3) + 1]].position.z };
	//		Vector3f pos3 = { vertices[indices[(i * 3) + 2]].position.x, vertices[indices[(i * 3) + 2]].position.y, vertices[indices[(i * 3) + 2]].position.z };
	//		myData->nodes.back().vertices.push_back(pos1);
	//		myData->nodes.back().vertices.push_back(pos2);
	//		myData->nodes.back().vertices.push_back(pos3);
	//		myData->nodes.back().center = (pos1 + pos2 + pos3) / 3.f;
	//	}
	//	++i;
	//}

	//for (int i = 0; i < neighbors.size(); i++)
	//{
	//	for (int n : neighbors[i])
	//	{
	//		Connection c;
	//		c.from = &myData->nodes[i];
	//		c.to = &myData->nodes[n];

	//		if (document.HasMember("Vertices"))
	//		{
	//			std::vector<Vector4f> sharedVertices;
	//			for (int n1 = 0; n1 < 3; n1++)
	//			{
	//				for (int n2 = 0; n2 < 3; n2++)
	//				{
	//					int index1 = indices[myData->nodes[i].index * 3 + n1];
	//					int index2 = indices[myData->nodes[n].index * 3 + n2];
	//					if (index1 == index2)
	//					{
	//						sharedVertices.push_back(vertices[indices[myData->nodes[i].index * 3 + n1]].position);
	//					}
	//				}
	//			}

	//			if (sharedVertices.size() >= 2)
	//			{
	//				Vector3f vertexPos = { sharedVertices[0].x, sharedVertices[0].y, sharedVertices[0].z };
	//				//Vector3f delta = (vertexPos - c.from->center).GetNormalized();
	//				//Vector3f forward = (c.to->center - c.from->center).GetNormalized();
	//				//bool startWithLeft = delta.Cross(forward).y < 0;
	//				bool startWithLeft = ((c.to->center.x - c.from->center.x) *
	//					(vertexPos.z - c.from->center.z) -
	//					(c.to->center.z - c.from->center.z) *
	//					(vertexPos.x - c.from->center.x)) > 0;

	//				int indexR = 0 + 1;
	//				int indexL = 1 - 1;
	//				c.left = { sharedVertices[indexL].x, sharedVertices[indexL].y, sharedVertices[indexL].z };
	//				c.right = { sharedVertices[indexR].x, sharedVertices[indexR].y, sharedVertices[indexR].z };
	//			}
	//		}

	//		myData->nodes[i].connections.push_back(c);
	//	}
	//}



	//VER 2
	myData = std::make_shared<NavMeshData>();

	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& document = wrapper->LoadDocument(aPath);

	myData->meshData = std::make_shared<MeshData>();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	if (document.HasMember("Vertices"))
	{
		auto& verticesArray = document["Vertices"].GetArray();
		for (int i = 0; i < verticesArray.Size(); i += 3)
		{
			Vertex v;
			v.position = { verticesArray[i].GetFloat(), verticesArray[i + 1].GetFloat(), verticesArray[i + 2].GetFloat(), 1 };
			vertices.push_back(v);
		}
	}


	if (document.HasMember("Indices"))
	{
		for (auto& i : document["Indices"].GetArray())
		{
			indices.push_back(i.GetInt());
		}
	}

	myData->meshData->SetVertices(vertices);
	myData->meshData->SetIndices(indices);
	if (document.HasMember("Number of Tiles"))
	{
		dtNavMeshParams params;
		params.maxPolys = document["Max Polys"].GetInt();
		params.maxTiles = document["Max Tiles"].GetInt();
		params.tileHeight = document["Tile Height"].GetFloat();
		params.tileWidth = document["Tile Width"].GetFloat();

		float pos[3];
		auto position = document["Origin"].GetArray();
		for (int i = 0; i < position.Size(); i++)
		{
			pos[i] = position[i].GetFloat();
		}
		params.orig[0] = pos[0];
		params.orig[1] = pos[1];
		params.orig[2] = pos[2];

		dtNavMesh* mesh = dtAllocNavMesh();
		mesh->init(&params);

		int numTiles = document["Number of Tiles"].GetInt();
		auto tiles = document["Tiles"].GetArray();
		for (int i = 0; i < numTiles; ++i)
		{
			NavMeshTileHeader tileHeader;
			tileHeader.tileRef = tiles[i].GetInt();
			tileHeader.dataSize = tiles[i + 1].GetUint();

			unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
			if (!data) break;
			memcpy_s(data, tileHeader.dataSize, tiles[i + 2].GetString(), tileHeader.dataSize);
			mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
		}

		myDtNavMesh = mesh;

		if (!myDtNavMeshQuery)
		{
			myDtNavMeshQuery = dtAllocNavMeshQuery();
		}

		myDtNavMeshQuery->init(myDtNavMesh, 2048);
	}

	//if (document.HasMember("Vertices"))
	//{
	//	for (int i = 0; i < indices.size(); i += 3)
	//	{
	//		auto& n = myData->nodes.emplace_back();
	//		myData->nodes.back().index = i;
	//		Vector3f pos1 = { vertices[indices[i]].position.x, vertices[indices[i]].position.y, vertices[indices[i]].position.z };
	//		Vector3f pos2 = { vertices[indices[i + 1]].position.x, vertices[indices[i + 1]].position.y, vertices[indices[i + 1]].position.z };
	//		Vector3f pos3 = { vertices[indices[i + 2]].position.x, vertices[indices[i + 2]].position.y, vertices[indices[i + 2]].position.z };
	//		n.indices.push_back(indices[i]);
	//		n.indices.push_back(indices[i + 1]);
	//		n.indices.push_back(indices[i + 2]);
	//		n.center = (pos1 + pos2 + pos3) / 3.f;
	//		myData->vertices.push_back(pos1);
	//		myData->vertices.push_back(pos2);
	//		myData->vertices.push_back(pos3);
	//		n.vertices.push_back(pos1);
	//		n.vertices.push_back(pos2);
	//		n.vertices.push_back(pos3);
	//	}
	//}
	/*for (auto& connection : document["Connections"].GetArray())
	{
		int n1 = connection["n1"].GetInt();
		int n2 = connection["n2"].GetInt();
		int l = connection["l"].GetInt();
		int r = connection["r"].GetInt();
		if (n1 < myData->nodes.size() && n2 < myData->nodes.size())
		{
			Connection c1;
			Connection c2;
			c2.to = c1.from = &myData->nodes[n1];
			c2.from = c1.to = &myData->nodes[n2];
			c1.left = c2.left = myData->nodes[n1].vertices[l];
			c1.right = c2.right = myData->nodes[n1].vertices[r];

			c1.cost = c2.cost = GetDistance(c1.from->center, c1.to->center);

			myData->nodes[n1].connections.push_back(c1);
			myData->nodes[n2].connections.push_back(c2);
		}
	}*/
}

bool NavMesh::SamplePoint(Vector3f aPoint)
{
	Node tempNode = myData->nodes[0];
	float distance = FLT_MAX;
	for (int i = 1; i < myData->nodes.size(); i++)
	{
		if (distance > myData->nodes[i].center.Distance(aPoint)) 
		{
			tempNode = myData->nodes[i];
			distance = myData->nodes[i].center.Distance(aPoint);
		}
	}
	return Catbox::PointInTriangle(aPoint, tempNode.vertices[0], tempNode.vertices[1], tempNode.vertices[2]);
}

bool NavMesh::SamplePoints(std::vector<Vector3f> aPointVector)
{
	std::vector<Node> nodes(aPointVector.size(), myData->nodes[0]);
	std::vector<float> distances(aPointVector.size(), FLT_MAX);

	for (int i = 1; i < myData->nodes.size(); i++)
	{
		for (int j = 0; j < distances.size(); j++)
		{
			if (distances[j] > myData->nodes[i].center.Distance(aPointVector[j]))
			{
				nodes[j] = myData->nodes[i];
				distances[j] = myData->nodes[i].center.Distance(aPointVector[j]);
			}
		}
	}
	for (int i = 0; i < nodes.size(); i++)
	{
		if (!Catbox::PointInTriangle(aPointVector[i], nodes[i].vertices[0], nodes[i].vertices[1], nodes[i].vertices[2]))
		{
			return false;
		}
	}
	return true;
}

Vector3f NavMesh::GetClosestPoint(Vector3f aPoint)
{
	Node tempNode = myData->nodes[0];
	float distance = FLT_MAX;
	for (int i = 1; i < myData->nodes.size(); i++)
	{
		if (distance > myData->nodes[i].center.Distance(aPoint))
		{
			tempNode = myData->nodes[i];
			distance = myData->nodes[i].center.Distance(aPoint);
		}
	}
	Vector3f returnPos = tempNode.vertices[0];
	for (int i = 1; i < 3; i++)
	{
		if (aPoint.Distance(tempNode.vertices[i]) < aPoint.Distance(returnPos)) 
		{
			returnPos = tempNode.vertices[i];
		}
	}
	return returnPos;
}

bool NavMesh::SampleBarycentricPoint(Vector3f aPoint)
{
	Node tempNode = myData->nodes[0];
	float distance = FLT_MAX;
	for (int i = 1; i < myData->nodes.size(); i++)
	{
		if (distance > myData->nodes[i].center.Distance(aPoint))
		{
			tempNode = myData->nodes[i];
			distance = myData->nodes[i].center.Distance(aPoint);
		}
	}
	if (Catbox::PointInTriangleBarycentric(aPoint, tempNode.vertices[0], tempNode.vertices[1], tempNode.vertices[2], 0.3f)) 
	{
		return true;
	}
	else 
	{
		for (int i = 0; i < tempNode.connections.size(); i++)
		{	
			Node aNode = *tempNode.connections[i].to;
			if (Catbox::PointInTriangleBarycentric(aPoint, aNode.vertices[0], aNode.vertices[1], aNode.vertices[2], 0.3f))
			{
				return true;
			}
		}
	}
	return false;
}

Vector3f NavMesh::GetNearestVertex(Vector3f aPoint)
{
	Node tempNode = myData->nodes[0];
	float distance = FLT_MAX;
	for (int i = 1; i < myData->nodes.size(); i++)
	{
		if (distance > myData->nodes[i].center.Distance(aPoint))
		{
			tempNode = myData->nodes[i];
			distance = myData->nodes[i].center.Distance(aPoint);
		}
	}

	Vector3f aNode1 = tempNode.vertices[0];
	Vector3f aNode2 = tempNode.vertices[1];
	Vector3f aNode3 = tempNode.vertices[2];

	float dx1 = aPoint.x - aNode1.x;
	float dy1 = aPoint.z - aNode1.z;
	float dx2 = aNode2.x - aNode1.x;
	float dy2 = aNode2.z - aNode1.z;
	float dx3 = aNode3.x - aNode1.x;
	float dy3 = aNode3.z - aNode1.z;

	return
	{
		aPoint.x,
		aNode1.y + ((dy1 * dx3 - dx1 * dy3) * (aNode2.y - aNode1.y) + (dx1 * dy2 - dy1 * dx2) * (aNode3.y - aNode1.y)) / (dx3 * dy2 - dx2 * dy3),
		aPoint.z
	};
}

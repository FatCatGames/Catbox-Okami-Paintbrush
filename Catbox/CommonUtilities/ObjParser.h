#pragma once
#include "ObjParser.h"
#include "Assets\MeshData.h"
#include "Assets\AssetRegistry.h"
//#include <strstream>
#include <sstream>

static std::shared_ptr<MeshData> ParseObj(const char* aPath)
{
	std::ifstream in(aPath, std::ios::in);

	if (!in)
	{
		MessageHandler::debugMessages.errors.push_back("Cannot find " + std::string(aPath));
		return nullptr;
	}

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::string line;
	while (std::getline(in, line))
	{
		//check v for vertices
		if (line.substr(0, 2) == "v ") 
		{
			std::istringstream v(line.substr(2));
			Vertex vert;
			float x, y, z;
			v >> x; v >> y; v >> z;
			vert.position = Vector4f(x, y, z, 1);
			vertices.push_back(vert);
		}

		//check for faces
		else if (line.substr(0, 2) == "f ") 
		{
			int a, b, c;
			int A, B, C;
			const char* chh = line.c_str();
			// Unity format
			//sscanf_s(chh, "f %i//%i %i//%i %i//%i", &a, &A, &c, &C, &b, &B);
			

			//Blender format
			sscanf_s(chh, "f %i/%i/%i %i/%i/%i %i/%i/%i", &a, &A, &A, &b, &B, &B, &c, &C, &C);

			indices.push_back(a-1);
			indices.push_back(b-1);
			indices.push_back(c-1);
		}
	}

	auto meshData = CreateAsset<MeshData>(aPath);
	meshData->SetVertices(vertices);
	meshData->SetIndices(indices);
	return meshData;
}
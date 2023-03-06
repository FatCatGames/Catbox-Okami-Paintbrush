#include "stdafx.h"
#include "ProjectSettings.h"
#include "Physics/Collisions/CollisionManager.h"

#include "rapidjson\document.h"
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>

void ProjectSettings::SaveProjectSettings()
{
	int version = 2;
	rapidjson::Document output;
	output.SetObject();
	auto& alloc = output.GetAllocator();
	rapidjson::Value value;

	value.SetInt(version);
	output.AddMember("Version", value, alloc);

	value.SetBool(Engine::GetInstance()->GetGraphicsEngine()->GetGammaCorrectionEnabled());
	output.AddMember("GammaCorrection", value, alloc);

	rapidjson::Value collisionMatrix(rapidjson::kArrayType);
	collisionMatrix.SetArray();

	auto& matrix = Engine::GetInstance()->GetCollisionManager()->GetCollisionLayerMatrix();

	for (int lay1 = 0; lay1 < matrix.size(); lay1++)
	{
		rapidjson::Value layer(rapidjson::kArrayType);
		for (size_t lay2 = 0; lay2 < matrix[lay1].size(); lay2++)
		{
			value.SetBool(matrix[lay1][lay2]);
			layer.PushBack(value, alloc);
		}
		collisionMatrix.PushBack(layer, alloc);
	}
	output.AddMember("Collision Matrix", collisionMatrix, alloc);

	rapidjson::Value physicsLayers(rapidjson::kArrayType);
	physicsLayers.SetArray();

	auto& physLay = Engine::GetInstance()->GetCollisionManager()->GetPhysicsCollisionLayers();

	for (int lay1 = 0; lay1 < physLay.size(); lay1++)
	{
		rapidjson::Value layer(rapidjson::kArrayType);
		for (size_t lay2 = 0; lay2 < physLay[lay1].size(); lay2++)
		{
			value.SetBool(physLay[lay1][lay2]);
			layer.PushBack(value, alloc);
		}
		physicsLayers.PushBack(layer, alloc);
	}
	output.AddMember("Physics Layers", physicsLayers, alloc);

	std::ofstream ofs("ProjectSettings.json");
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	output.Accept(writer);
}


void ProjectSettings::LoadProjectSettings()
{
	FILE* fp;
	bool error = fopen_s(&fp, "ProjectSettings.json", "rb");
	if (error) //no project settings file found
	{
		return;
	}

	rapidjson::Document document;
	char readbuffer[65536]{};
	rapidjson::FileReadStream is(fp, readbuffer, sizeof(readbuffer));
	document.ParseStream(is);
	fclose(fp);

	auto& collisionMatrix = Engine::GetInstance()->GetCollisionManager()->GetCollisionLayerMatrix();
	auto& physicsLayers = Engine::GetInstance()->GetCollisionManager()->GetPhysicsCollisionLayers();
	int version = document["Version"].GetInt();

	if (version >= 1)
	{
		auto matrix = document["Collision Matrix"].GetArray();
		for (int lay1 = 0; lay1 < static_cast<int>(matrix.Size()); lay1++)
		{
			if (lay1 >= collisionMatrix.size()) break;
			for (int lay2 = 0; lay2 < static_cast<int>(matrix[lay1].Size()); lay2++)
			{
				if (lay2 >= collisionMatrix[lay1].size()) break;
				collisionMatrix[lay1][lay2] = matrix[lay1][lay2].GetBool();
			}
		}

		if (version >= 2)
		{
			auto physicsMatrix = document["Physics Layers"].GetArray();
			for (int lay1 = 0; lay1 < static_cast<int>(physicsMatrix.Size()); lay1++)
			{
				if (lay1 >= physicsLayers.size()) break;
				for (int lay2 = 0; lay2 < static_cast<int>(physicsMatrix[lay1].Size()); lay2++)
				{
					if (lay2 >= physicsLayers[lay1].size()) break;
					physicsLayers[lay1][lay2] = physicsMatrix[lay1][lay2].GetBool();
				}
			}
		}
	}
}



void ProjectSettings::SaveKeyBinds()
{
	rapidjson::Document outputKeyBinds;
	rapidjson::Document::AllocatorType& aAllocator = outputKeyBinds.GetAllocator();
	outputKeyBinds.SetArray();

	for (auto& keys : Input::GetControllerScheme())
	{
		outputKeyBinds.PushBack(rapidjson::StringRef(keys.first.c_str(), keys.first.length()), aAllocator);
		outputKeyBinds.PushBack(static_cast<int>(keys.second), aAllocator);
	}

	std::ofstream secondOfs("GameKeyBinds.json");
	rapidjson::OStreamWrapper secondOsw(secondOfs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> secondWriter(secondOsw);
	outputKeyBinds.Accept(secondWriter);

}



void ProjectSettings::LoadKeyBinds()
{
	FILE* tempKeyBinds;
	fopen_s(&tempKeyBinds, "GameKeyBinds.json", "rb");
	if (tempKeyBinds)
	{
		rapidjson::Document document;
		char readbuffer[65536]{};
		rapidjson::FileReadStream is(tempKeyBinds, readbuffer, sizeof(readbuffer));
		document.ParseStream(is);
		fclose(tempKeyBinds);
		int size = document.GetArray().Size() - 1;
		for (int i = 0; i < size; i += 2)
		{
			if (!document[i].IsNull())
			{
				Input::AddKeyBind(document[i].GetString(), KeyCode(document[i + 1].GetInt()));
			}
		}
	}
}

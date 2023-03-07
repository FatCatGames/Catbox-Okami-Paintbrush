#include "stdafx.h"
#include "AssetRegistry.h"
#include <cassert>
#include "MeshData.h"
#include "Shader.h"
#include "Model.h"
#include <fstream>
#include "AssetLoader.h"
#include "Texture.h"
#include "3D\ModelInstance.h"
#include "Graphics/Particles/ParticleEmitterSettings.h"
#include "Animator.h"
#include "ParticleSystem.h"
#include "Scene.h"

#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/document.h>

#include <iostream>
#include <cstdio>

#include "Shaders\FlipbookShader.h"
#include "Shaders\UVPannerShader.h"
#include "Shaders\ScalingShader.h"
#include "Shaders\FadeoutShader.h"
#include "Shaders\ParticleAtlasWalkShader.h"
#include "Shaders/Environment/WaterDisplaceMentVS.h"

AssetRegistry* AssetRegistry::Instance = nullptr;
AssetRegistry::AssetRegistry()
{
	Instance = this;
}


void AssetRegistry::LoadAssets()
{
	CreateShaders();
	CreateMaterials();
	CreateMeshes();
	CreateModels();
	CreateTextures();

}

void AssetRegistry::RenameAsset(Asset* anAsset, const std::string& aNewName)
{
	std::string oldPath = anAsset->myPath.string();
	std::string oldName = Catbox::GetNameFromPath(oldPath);
	std::string extension = Catbox::GetExtensionFromPath(oldPath);
	std::string newPath = oldPath.substr(0, oldPath.size() - oldName.size() - extension.size());
	newPath += aNewName + extension;
	if (rename(oldPath.c_str(), newPath.c_str()) != 0)
	{
		printerror("Failed to rename " + oldName + " to " + aNewName);
	}
	else
	{
		anAsset->SetName(aNewName);
		anAsset->myPath = newPath;
		SortAll();
	}
}

void AssetRegistry::SaveAssetPaths()
{
	rapidjson::Document document;
	document.SetObject();

	rapidjson::Value paths;
	paths.SetObject();

	unordered_map<type_index, vector<shared_ptr<Asset>>>::iterator it;
	for (it = myAssets.begin(); it != myAssets.end(); it++)
	{
		for (auto& asset : it->second)
		{
			auto path = asset->GetPath().string();

			rapidjson::Value pathObj;
			pathObj.SetString(path.c_str(), rapidjson::SizeType(path.length()), document.GetAllocator());
			rapidjson::Value n(asset->GetName().c_str(), document.GetAllocator());
			paths.AddMember(n, pathObj, document.GetAllocator());
		}
	}

	document.AddMember("Paths", paths, document.GetAllocator());
	std::ofstream ofs("AssetPaths.json");
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	document.Accept(writer);
}


GameObject* AssetRegistry::AutoMakeGameObject(const std::filesystem::path& aPath)
{
	std::string pathStr = aPath.string();
	std::string assetName = Catbox::GetNameFromPath(pathStr);
	auto extension = aPath.extension();
	GameObject* tempGameObject = nullptr;

	if (extension == ".prefab")
	{
		tempGameObject = InstantiatePrefab(assetName);
	}

	else if (extension == ".fbx")
	{
		tempGameObject = Instantiate();
		if (aPath.string().find("_anim.fbx") != string::npos)
		{
			int aCounter = 0;
			std::string skName = "";
			std::string animName = assetName;
			animName.resize(animName.size() - 4);

			while (assetName[aCounter] != '_')
			{
				skName.push_back(assetName[aCounter]);
				aCounter++;
			}
			skName = skName += "_sk";
			if (AssetRegistry::GetInstance()->GetAsset<Animation>(animName) == nullptr)
			{
				if (AssetRegistry::GetInstance()->GetAsset<Model>(skName) == nullptr)
				{
					printerror("Could not find the animations sk. Make sure its loaded in!");
				}
				else
				{
					AssetLoader::LoadAnimation(pathStr);
				}
			}
			tempGameObject->SetName(assetName.c_str());
			ModelInstance* aModelInstance = tempGameObject->AddComponent<ModelInstance>();
			aModelInstance->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(skName.c_str()));
			Animator* aAnimator = tempGameObject->AddComponent<Animator>();
			//aAnimator->SetAnimation(AssetRegistry::GetInstance()->GetAsset<Animation>(animName));
		}
		else
		{
			tempGameObject->SetName(assetName.c_str());
			ModelInstance* aModelInstance = tempGameObject->AddComponent<ModelInstance>();
			if (AssetRegistry::GetInstance()->GetAsset<Model>(assetName) == nullptr)
			{
				AssetLoader::LoadFBX(pathStr);
			}
			aModelInstance->SetModel(AssetRegistry::GetInstance()->GetAsset<Model>(assetName));
		}
	}
	else if (extension == ".pe")
	{
		tempGameObject = Instantiate();
		tempGameObject->SetName(assetName.c_str());
		ParticleSystem* particleSystem = tempGameObject->AddComponent<ParticleSystem>();
		particleSystem->AddEmitter(AssetRegistry::GetInstance()->GetAsset<ParticleEmitterSettings>(assetName));
	}

	else if (extension == ".scene")
	{
		GetAsset<Scene>(assetName)->ImportScene();
	}

	return tempGameObject;
}

void AssetRegistry::SaveAssetPath(const Asset& anAsset)
{
	auto path = anAsset.GetPath().string();
	const auto& name = anAsset.GetName().c_str();

	rapidjson::Document document;
	FILE* fp;
	bool fail = fopen_s(&fp, "AssetPaths.json", "rb");
	if (fail) return;

	char readbuffer[65536]{};
	rapidjson::FileReadStream is(fp, readbuffer, sizeof(readbuffer));
	document.ParseStream(is);
	fclose(fp);
	rapidjson::Value pathsObj;
	pathsObj.SetObject();
	document.AddMember("Paths", pathsObj, document.GetAllocator());

	auto paths = document["Paths"].GetObj();

	if (!paths.HasMember(name))
	{
		rapidjson::Value pathObj;
		pathObj.SetString(path.c_str(), rapidjson::SizeType(path.length()), document.GetAllocator());
		rapidjson::Value n(anAsset.GetName().c_str(), document.GetAllocator());
		paths.AddMember(n, pathObj, document.GetAllocator());
	}
	else
	{
		paths[name].SetString(path.c_str(), document.GetAllocator());
	}

	paths[anAsset.GetName().c_str()].SetString(path.c_str(), document.GetAllocator());
	std::ofstream ofs("AssetPaths.json");
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	document.Accept(writer);
}

void AssetRegistry::CreateShaders()
{
	shared_ptr<VertexShader> vertexShader = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/DefaultVS.sh");
	std::string shaderData;
	HRESULT result;
	ifstream vsFile;
	vsFile.open("Resources/BuiltIn/Shaders/DefaultVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &vertexShader->vertexShader);
	vsFile.close();
	assert(!FAILED(result) && "Loading default vertex shader failed!");

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//Instance data
		{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};
	DX11::Device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), shaderData.data(), shaderData.size(), &vertexShader->inputLayout);


	shared_ptr<PixelShader> defaultPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/DefaultPS.sh");
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/DefaultPS.cso", ios::binary);
	string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &defaultPS->pixelShader);
	assert(!FAILED(result) && "Loading default pixel shader failed!");
	psFile.close();

	shared_ptr<PixelShader> unlitPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/UnlitPS.sh");
	psFile;
	psFile.open("Resources/BuiltIn/Shaders/UnlitPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &unlitPS->pixelShader);
	assert(!FAILED(result) && "Loading unlit pixel shader failed!");
	psFile.close();

	shared_ptr<PixelShader> tilingShader = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/TilingPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/TilingPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &tilingShader->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading tiling pixel shader failed!");

	std::shared_ptr<VertexShader> scalingVS = std::make_shared<ScalingShader>();
	scalingVS->SetPath("Resources/BuiltIn/Shaders/ScalingVS.sh");
	AddToRegistry<VertexShader>(scalingVS);

	std::shared_ptr<VertexShader> waterDisplacementVS = std::make_shared<WaterDisplacementVS>();
	waterDisplacementVS->SetPath("Resources/BuiltIn/Shaders/WaterDisplacementVS.sh");
	AddToRegistry<VertexShader>(waterDisplacementVS);

	std::shared_ptr<PixelShader> fadeoutPS = std::make_shared<FadeoutShader>();
	fadeoutPS->SetPath("Resources/BuiltIn/Shaders/FadeoutPS.sh");
	AddToRegistry<PixelShader>(fadeoutPS);

	std::shared_ptr<PixelShader> particleAtlasWalk = std::make_shared<ParticleAtlasWalkShader>();
	particleAtlasWalk->SetPath("Resources/BuiltIn/Shaders/ParticleAtlasWalkPS.sh");
	AddToRegistry<PixelShader>(particleAtlasWalk);

	std::shared_ptr<PixelShader> UVPannerPS = std::make_shared<UVPannerShader>();
	UVPannerPS->SetPath("Resources/BuiltIn/Shaders/UVPannerPS.sh");
	AddToRegistry<PixelShader>(UVPannerPS);

	std::shared_ptr<PixelShader> flipbook = std::make_shared<FlipbookShader>();
	flipbook->SetPath("Resources/BuiltIn/Shaders/FlipbookPS.sh");
	AddToRegistry<PixelShader>(flipbook);

	shared_ptr<PixelShader> particleAtlasWalkPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/ParticleAtlasWalkPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/ParticleAtlasWalkPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &particleAtlasWalkPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading particleAtlasWalkPS shader failed!");

	shared_ptr<PixelShader> OutlinePS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/OutlinePS.sh");
	psFile.open("Resources/BuiltIn/Shaders/OutlinePS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &OutlinePS->pixelShader);
	assert(!FAILED(result) && "Loading Outline pixel shader failed!");
	psFile.close();

	shared_ptr<PixelShader> OutlineMaskPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/OutlineMaskPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/OutlineMaskPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &OutlineMaskPS->pixelShader);
	assert(!FAILED(result) && "Loading Outline mask pixel shader failed!");
	psFile.close();

	shared_ptr<PixelShader> HologramPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/HologramPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/HologramPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &HologramPS->pixelShader);
	assert(!FAILED(result) && "Loading Hologram pixel shader failed!");
	psFile.close();

	shared_ptr<VertexShader> HologramVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/HologramVS.sh");
	vsFile.open("Resources/BuiltIn/Shaders/HologramVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &HologramVS->vertexShader);
	vsFile.close();
	assert(!FAILED(result) && "Loading Hologram vertex shader failed!");

#pragma region Billboard

	shared_ptr<PixelShader> billboardPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/BillboardPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/BillboardPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &billboardPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading billboard pixel shader failed!");

	shared_ptr<VertexShader> billboardVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/BillboardVS.sh");
	vsFile.open("Resources/BuiltIn/Shaders/BillboardVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &billboardVS->vertexShader);
	vsFile.close();
	assert(!FAILED(result) && "Loading billboard vertex shader failed!");

	D3D11_INPUT_ELEMENT_DESC billboardLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};
	DX11::Device->CreateInputLayout(billboardLayout, sizeof(billboardLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC), shaderData.data(), shaderData.size(), &billboardVS->inputLayout);


	shared_ptr<GeometryShader> billboardGS = CreateAsset<GeometryShader>("Resources/BuiltIn/Shaders/BillboardGS.sh");
	std::ifstream gsFile;
	gsFile.open("Resources/BuiltIn/Shaders/BillboardGS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(gsFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateGeometryShader(shaderData.data(), shaderData.size(), nullptr, &billboardGS->geometryShader);
	gsFile.close();
	assert(!FAILED(result) && "Loading billboard geometry shader failed!");

	shared_ptr<GeometryShader> SubdivideSmoothGS = CreateAsset<GeometryShader>("Resources/BuiltIn/Shaders/SubdivideSmoothGS.sh");
	gsFile;
	gsFile.open("Resources/BuiltIn/Shaders/SubdivideSmoothGS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(gsFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateGeometryShader(shaderData.data(), shaderData.size(), nullptr, &SubdivideSmoothGS->geometryShader);
	gsFile.close();
	assert(!FAILED(result) && "Loading SubdivideSmoothGS geometry shader failed!");

#pragma endregion

	//particle shaders
	shared_ptr<PixelShader> particlePS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/ParticlePS.sh");
	psFile.open("Resources/BuiltIn/Shaders/ParticlePS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &particlePS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading particle pixel shader failed!");

	shared_ptr<PixelShader> particleFlipbookPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/ParticleFlipbookPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/ParticleFlipbookPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &particleFlipbookPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading particle flipbook pixel shader failed!");

	shared_ptr<VertexShader> particleVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/ParticleVS.sh");
	vsFile.open("Resources/BuiltIn/Shaders/ParticleVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &particleVS->vertexShader);
	vsFile.close();
	assert(!FAILED(result) && "Loading particle vertex shader failed!");

	D3D11_INPUT_ELEMENT_DESC pvsLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"ID", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	DX11::Device->CreateInputLayout(pvsLayout, sizeof(pvsLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC), shaderData.data(), shaderData.size(), &particleVS->inputLayout);


	shared_ptr<GeometryShader> particleGS = CreateAsset<GeometryShader>("Resources/BuiltIn/Shaders/ParticleGS.sh");
	gsFile.open("Resources/BuiltIn/Shaders/ParticleGS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(gsFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateGeometryShader(shaderData.data(), shaderData.size(), nullptr, &particleGS->geometryShader);
	gsFile.close();
	assert(!FAILED(result) && "Loading particle geometry shader failed!");

	std::shared_ptr<PixelShader> myVideoPS = CreateAsset<PixelShader>("Assets/Shaders/VideoPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/VideoPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myVideoPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading Video pixel shader failed!");

	std::shared_ptr<PixelShader> myUISpritePS = CreateAsset<PixelShader>("Assets/Shaders/UISpritePS.sh");
	psFile.open("Resources/BuiltIn/Shaders/UISpritePS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myUISpritePS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading UISprite pixel shader failed!");

	//shared_ptr<VertexShader> WaterDisplacementVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/WaterDisplacementVS.sh");
	//vsFile.open("Resources/BuiltIn/Shaders/WaterDisplacementVS.cso", ios::binary);
	//std::string vsData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	//result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &WaterDisplacementVS->vertexShader);
	//vsFile.close();
	//assert(!FAILED(result) && "Loading WaterDisplacementVS vertex shader failed!");

	shared_ptr<PixelShader> WaterTestPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/WaterTestPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/WaterTestPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &WaterTestPS->pixelShader);
	assert(!FAILED(result) && "Loading WaterTestPS pixel shader failed!");
	psFile.close();





	std::shared_ptr<VertexShader> myUISpriteVS = CreateAsset<VertexShader>("Assets/Shaders/UISpriteVS.sh");
	vsFile.open("Resources/BuiltIn/Shaders/UISpriteVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &myUISpriteVS->vertexShader);
	assert(!FAILED(result) && "Loading UISprite vertex shader failed!");
	vsFile.close();
	D3D11_INPUT_ELEMENT_DESC uiSpriteLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	result = DX11::Device->CreateInputLayout(uiSpriteLayout, sizeof(uiSpriteLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC), shaderData.data(), shaderData.size(), myUISpriteVS->inputLayout.GetAddressOf());
	assert(!FAILED(result) && "Creating UISprite vertex shader input layout failed!");
}

void AssetRegistry::CreateMaterials()
{
	CreateAsset<Material>("Resources/BuiltIn/Materials/DefaultMat.mat");
	auto uiMat = CreateAsset<Material>("Resources/BuiltIn/Materials/SpriteDefaultMat.mat");
	auto particleMat = CreateAsset<Material>("Resources/BuiltIn/Materials/ParticleDefaultMat.mat");
	auto particleFlipbookMat = CreateAsset<Material>("Resources/BuiltIn/Materials/ParticleFlipbookMat.mat");
	uiMat->SetPixelShader(GetAsset<PixelShader>("UISpritePS"));
	uiMat->SetVertexShader(GetAsset<VertexShader>("UISpriteVS"));

	particleMat->SetPixelShader(GetAsset<PixelShader>("ParticlePS"));
	particleMat->SetVertexShader(GetAsset<VertexShader>("ParticleVS"));

	particleFlipbookMat->SetPixelShader(GetAsset<PixelShader>("ParticleFlipbookPS"));
	particleFlipbookMat->SetVertexShader(GetAsset<VertexShader>("ParticleVS"));
}

void AssetRegistry::CreateMeshes()
{
	//Create cube mesh
	shared_ptr<MeshData> cube = CreateAsset<MeshData>("Resources/BuiltIn/Meshes/Cube.fbx");
	auto vertices = std::vector<Vertex>();
	for (float x = -1; x < 2; x += 2)
	{
		for (float y = -1; y < 2; y += 2)
		{
			for (float z = -1; z < 2; z += 2)
			{
				Vertex v;
				v.position = Vector4f(x / 2.f, y / 2.f, z / 2.f, 1);
				v.colors[0] = { (float)std::fmax(0, x) , (float)std::fmax(0, y) , (float)std::fmax(0, z), 1 };
				v.UVs[0] = { (x + 1) / 2.f , 1.f - ((y + 1) / 2.f) };
				v.normal = { 0,1,0 };
				v.binormal = { 1,0,0 };
				v.tangent = { 0,0,1 };
				vertices.push_back(v);
			}
		}
	}
	cube->SetVertices(vertices);

	std::vector<unsigned int> mdlIndices =
	{
		2,0,1,
		1,3,2,

		5,1,0,
		0,4,5,

		6,4,0,
		0,2,6,

		6,2,3,
		3,7,6,

		7,5,4,
		4,6,7,

		3,1,5,
		5,7,3
	};

	cube->SetIndices(mdlIndices);

	vertices.clear();
	vertices.emplace_back();
	shared_ptr<MeshData> billboard = CreateAsset<MeshData>("Resources/BuiltIn/Meshes/Billboard.fbx");
	billboard->SetVertices(vertices);
	billboard->myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	shared_ptr<MeshData> plane = CreateAsset<MeshData>("Resources/BuiltIn/Meshes/Plane.fbx");
	vertices.clear();
	for (float x = -1; x < 2; x += 2)
	{
		for (float y = -1; y < 2; y += 2)
		{
			Vertex v;
			v.position = Vector4f(x / 2.f, 0, y / 2.f, 1);
			v.colors[0] = { (float)std::fmax(0, x) , 0 , (float)std::fmax(0, y), 1 };
			v.UVs[0] = { (x + 1) / 2.f , 1.f - ((y + 1) / 2.f) };
			v.normal = { 0,1,0 };
			v.binormal = { 1,0,0 };
			v.tangent = { 0,0,1 };
			vertices.push_back(v);
		}
	}

	plane->SetVertices(vertices);
	mdlIndices.clear();
	mdlIndices =
	{
		2,0,1,
		1,3,2
	};

	plane->SetIndices(mdlIndices);
}

void AssetRegistry::CreateModels()
{
	shared_ptr<Model> model = CreateAsset<Model>("Assets\\BuiltIn\\Models\\Cube.fbx");
	model->myBoundsRadius = 0.5f;
	model->myCenter = Vector3f(0, 0, 0);
	model->myModelParts.push_back(GetAsset<MeshData>("Cube"));
	model->myConvertUnits = false;

	model = CreateAsset<Model>("Assets\\BuiltIn\\Models\\Plane.fbx");
	model->myBoundsRadius = 0.5f;
	model->myCenter = Vector3f(0, 0, 0);
	model->myModelParts.push_back(GetAsset<MeshData>("Plane"));
	model->myConvertUnits = false;

	model = CreateAsset<Model>("Assets\\BuiltIn\\Models\\Billboard.fbx");
	model->myCenter = Vector3f(0, 0, 0);
	model->myModelParts.push_back(GetAsset<MeshData>("Billboard"));
	model->myConvertUnits = false;
}

void AssetRegistry::CreateTextures()
{
	auto defC = AssetLoader::LoadTexture("Resources/Textures/T_default_C.dds");
	auto defN = AssetLoader::LoadTexture("Resources/Textures/T_default_N.dds");
	auto defM = AssetLoader::LoadTexture("Resources/Textures/T_default_M.dds");
	AssetLoader::LoadTexture("Resources/Textures/Particle-Default.dds");
	AssetLoader::LoadTexture("Resources/Textures/Circle-Solid.dds");
	AssetLoader::LoadTexture("Resources/Textures/Square-Default.dds");
	AssetLoader::LoadTexture("Resources/Textures/BlueNoise.dds");
	AssetLoader::LoadTexture("Resources/Textures/studio_cubemap.dds");
	AssetLoader::LoadTexture("Resources/Textures/WhiteCubeMap.dds");

	auto defMat = GetAsset<Material>("DefaultMat");
	defMat->SetAlbedoTexture(defC);
	defMat->SetNormalTexture(defN);
	defMat->SetPbrTexture(defM);
}


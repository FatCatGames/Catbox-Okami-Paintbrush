#include "stdafx.h"
#include "AssetLoader.h"
#include <TGAFbx.h>
#include "MeshData.h"
#include <memory>
#include "Material.h"
#include "Model.h"
#include <UtilityFunctions.hpp>
#include "Graphics/Animations/SkeletonData.h"
#include "Texture.h"
#include <DDSTextureLoader11.h>
#include <Windows.h>
#include "Graphics/Particles/ParticleEmitterSettings.h"
#include "Assets\PostProcessingProfile.h"
#include "Assets\Scene.h"
#include "Assets\Blueprint.h"
#include "Assets\AnimationController.h"
#include "Assets/Prefab.h"
#include "CommonUtilities\ObjParser.h"
#include "Navigation\NavMesh.h"
#include "../Game/Managers/GameManager.h"
#include "../ComponentTools/ThreadPool.h"
#include <future>

static const std::filesystem::path AssetPath = "Assets\\";
static std::vector<UnloadedAsset> myAnimationsToLoad;
static std::vector<UnloadedAsset> myMaterialsToLoad;
static std::vector<UnloadedAsset> myParticleSystemsToLoad;
//static std::vector<UnloadedAsset> myPrefabsToLoad;
static std::vector<UnloadedAsset> myAnimationControllersToLoad;
AssetFolder AssetLoader::myAssetFolder;
static bool myThreadActive;

std::vector<AnimatedVertex> LoadAnimatedVertices(TGA::FBX::Model::Mesh &aMesh)
{
	std::vector<AnimatedVertex> vertices;
	int size = 0;
	for (auto& tgaVertex : aMesh.Vertices)
	{

		/*if (vertices[i].position.LengthSqr() > maxDistSqr)
		{
			maxDistSqr = vertices[i].position.LengthSqr();
		}*/

		AnimatedVertex vertex;
		auto& position = tgaVertex.Position;
		vertex.position = { position[0], position[1], position[2], position[3] };

		auto& color = tgaVertex.VertexColors;
		for (size_t col = 0; col < 4; col++) {
			vertex.colors[col] = { color[col][0], color[col][1], color[col][2], color[col][3] };
		}

		vertex.tangent = {
			tgaVertex.Tangent[0],
			tgaVertex.Tangent[1],
			tgaVertex.Tangent[2]
		};

		vertex.normal = {
		tgaVertex.Normal[0],
		tgaVertex.Normal[1],
		tgaVertex.Normal[2]
		};

		vertex.binormal = {
		tgaVertex.BiNormal[0],
		tgaVertex.BiNormal[1],
		tgaVertex.BiNormal[2]
		};

		auto& uvs = tgaVertex.UVs;
		for (int uvCh = 0; uvCh < 4; uvCh++)
		{
			vertex.UVs[uvCh] = { uvs[uvCh][0], uvs[uvCh][1] };
		}

		vertex.boneIDs = {
					tgaVertex.BoneIDs[0],
					tgaVertex.BoneIDs[1],
					tgaVertex.BoneIDs[2],
					tgaVertex.BoneIDs[3]
		};
		vertex.boneWeights = {
			tgaVertex.BoneWeights[0],
			tgaVertex.BoneWeights[1],
			tgaVertex.BoneWeights[2],
			tgaVertex.BoneWeights[3]
		};
		size += sizeof(vertex);
		vertices.push_back(vertex);
	}
	return vertices;
}

std::vector<Vertex> LoadStaticVertices(TGA::FBX::Model::Mesh aMesh)
{
	std::vector<Vertex> vertices;
	int size = 0;
	for (auto& tgaVertex : aMesh.Vertices)
	{

		/*if (vertices[i].position.LengthSqr() > maxDistSqr)
		{
			maxDistSqr = vertices[i].position.LengthSqr();
		}*/

		Vertex vertex;

		auto& position = tgaVertex.Position;
		vertex.position = Vector4f(position[0], position[1], position[2], position[3]);

		auto& color = tgaVertex.VertexColors;
		for (size_t col = 0; col < 4; col++) {
			vertex.colors[col] = { color[col][0], color[col][1], color[col][2], color[col][3] };
		}

		auto& uvs = tgaVertex.UVs;
		for (int uvCh = 0; uvCh < 4; uvCh++) {
			vertex.UVs[uvCh] = { uvs[uvCh][0], uvs[uvCh][1] };
		}

		vertex.tangent = {
			tgaVertex.Tangent[0],
			tgaVertex.Tangent[1],
			tgaVertex.Tangent[2]
		};

		vertex.normal = {
		tgaVertex.Normal[0],
		tgaVertex.Normal[1],
		tgaVertex.Normal[2]
		};

		vertex.binormal = {
		tgaVertex.BiNormal[0],
		tgaVertex.BiNormal[1],
		tgaVertex.BiNormal[2]
		};

		vertices.push_back(vertex);
		size += sizeof(vertex);
	}
	return vertices;
}

std::shared_ptr<Asset> AssetLoader::LoadAsset(const std::string& aPath)
{
	TGA::FBX::Importer::InitImporter();

	std::string extension = Catbox::GetExtensionFromPath(aPath);
	std::shared_ptr<Asset> asset;
	
	if (extension == ".prefab") return CreateAsset<Prefab>(aPath);
	else if (extension == ".fbx")
	{
		if (aPath.find("_anim") == string::npos) return LoadFBX(aPath);
		else return LoadAnimation(aPath);
	}
	else if (extension == ".dds") return LoadTexture(aPath);
	else if (extension == ".pe") asset = CreateAsset<ParticleEmitterSettings>(aPath);
	else if (extension == ".mat") asset = CreateAsset<Material>(aPath);
	else if (extension == ".profile") asset = CreateAsset<PostProcessingProfile>(aPath);
	else if (extension == ".scene") return CreateAsset<Scene>(aPath);
	else if (extension == ".obj") return ParseObj(aPath.c_str());
	else if (extension == ".nav") asset = CreateAsset<NavMesh>(aPath);
	else if (extension == ".bp") asset = CreateAsset<Blueprint>(aPath);
	else if (extension == ".ac") return CreateAsset<AnimationController>(aPath);

	if (!asset)
	{
		printerror("LoadAsset() not implemented for " + extension);
	}
	else asset->LoadFromPath(aPath.c_str());
	return asset;
}

std::shared_ptr<Model> AssetLoader::LoadFBX(const std::string& aPath)
{
	std::shared_ptr<Model> model = AssetRegistry::GetInstance()->GetAssetByPath<Model>(aPath);
	if (model) return model;

	TGA::FBX::Model tgaModel;
	model = CreateAsset<Model>(aPath);

	std::wstring aPathW = std::wstring(aPath.begin(), aPath.end());

	if (!TGA::FBX::Importer::LoadModel(aPathW, tgaModel))
	{
		std::cout << "Failed to load " << aPath << std::endl;
		return nullptr;
	}


	bool hasSkeleton = tgaModel.Skeleton.GetRoot();
	if (hasSkeleton)
	{
		model->mySkeleton = CreateAsset<Skeleton>(aPath);
		for (size_t i = 0; i < tgaModel.Skeleton.Bones.size(); i++)
		{
			auto& bone = tgaModel.Skeleton.Bones[i];
			model->mySkeleton->boneNameToIndex[bone.Name] = model->mySkeleton->bones.size();
			Catbox::Matrix4x4<float> bindPoseInverse;
			for (int row = 1; row < 5; row++)
			{
				for (int col = 1; col < 5; col++)
				{
					bindPoseInverse(row, col) = bone.BindPoseInverse.Data[((row - 1) * 4) + (col - 1)];
					//if (row != 4) bindPoseInverse(row, col);
				}
			}

			model->mySkeleton->boneNames.push_back(bone.Name);
			model->mySkeleton->bones.push_back(Skeleton::Bone(bone.Name, static_cast<int>(i), bone.ParentIdx, bindPoseInverse, bone.Children));
		}

		for (size_t i = 0; i < model->mySkeleton->bones.size(); i++)
		{
			if (tgaModel.Skeleton.Bones[i].ParentIdx == -1) continue;
		}
	}


	float maxDistSqr = 0;
	for (auto& mesh : tgaModel.Meshes)
	{
		std::shared_ptr<Material> material = AssetRegistry::GetInstance()->GetAsset<Material>(tgaModel.Materials[mesh.MaterialIndex].MaterialName);
		if (material == nullptr)
		{
			material = AssetRegistry::GetInstance()->GetAsset<Material>("DefaultMat");
		}

		std::shared_ptr<MeshData> meshData = std::make_shared<MeshData>();

		if (hasSkeleton) meshData->SetVertices(LoadAnimatedVertices(mesh));
		else meshData->SetVertices(LoadStaticVertices(mesh));

		meshData->SetIndices(mesh.Indices);

		model->myModelParts.push_back(meshData);
		model->myBoundsRadius = std::sqrt(maxDistSqr);
	}
	model->CreateBoundingUnits();
	return model;
}

std::shared_ptr<Prefab> AssetLoader::LoadPrefab(const std::string& aPath)
{
	auto prefab = AssetRegistry::GetInstance()->GetAssetByPath<Prefab>(aPath);
	if (prefab) return prefab;

	prefab = CreateAsset<Prefab>(aPath);
	return prefab;
}

std::shared_ptr<Animation> AssetLoader::LoadAnimation(const std::string& aPath)
{
	std::shared_ptr<Animation> animation = AssetRegistry::GetInstance()->GetAssetByPath<Animation>(aPath);
	if (animation) return animation;


	std::string animName = Catbox::GetNameFromPath(aPath);
	std::string skName = "";
	std::wstring aPathW = std::wstring(aPath.begin(), aPath.end());
	int aCounter = 0;
	while (animName[aCounter] != '_')
	{
		skName.push_back(animName[aCounter]);
		aCounter++;
		if (aCounter == animName.size())
		{
			printerror(aPath + " does not follow animation naming convention.");
			return nullptr;
		}
	}
	skName = skName += "_sk";
	std::shared_ptr<Model> sk = AssetRegistry::GetInstance()->GetAsset<Model>(skName);
	std::shared_ptr<Skeleton> skeleton;
	if (!sk)
	{
		printerror("No skeleton found: " + skName);
		return nullptr;
	}
	else
	{
		skeleton = sk->GetSkeleton();
		if (!skeleton)
		{
			printerror("No skeleton found: " + skName);
			return nullptr;
		}
	}

	TGA::FBX::Animation fbxAnimation;

	if (TGA::FBX::Importer::LoadAnimation(aPathW.c_str(), fbxAnimation))
	{
		animation = CreateAsset<Animation>(aPath);
		animation->frameCount = fbxAnimation.Length;
		animation->duration = static_cast<float>(fbxAnimation.Duration);
		animation->framesPerSecond = fbxAnimation.FramesPerSecond;
		animation->frames.resize(animation->frameCount);

		for (int i = 0; i < fbxAnimation.Frames.size(); i++) 
		{

			for (const auto& [bonename, transform] : fbxAnimation.Frames[i].LocalTransforms)
			{
				Catbox::Matrix4x4<float> matrixToTranspose;

				memcpy_s(&matrixToTranspose, sizeof(Catbox::Matrix4x4<float>), &transform, sizeof(float) * 16);

				//for (int row = 1; row < 5; row++)
				//{
				//	for (int col = 1; col < 5; col++)
				//	{
				//		matrixToTranspose(row, col) = [((row - 1) * 4) + (col - 1)];
				//	}
				//}

				Catbox::Matrix4x4<float> m = matrixToTranspose;
				Vector3f pos = { m(4,1), m(4,2), m(4,3) };
				Vector3f scale = m.GetScaleFromMatrix(m);
				Vector3f rot = m.GetRotationFromMatrix(m);
				animation->frames[i] .localPositions.emplace(bonename, pos);
				animation->frames[i].localScales.emplace(bonename, scale);
				//newFrame.localRotations.emplace_back(rot);
				animation->frames[i].localRotations.emplace(bonename,QuatFunctions::FromRotationMatrix4x4(m));
				animation->frames[i].transposedLocalTransforms.emplace(bonename,m);
				m = m.Transpose(m);
				animation->frames[i].localTransforms.emplace(bonename, m);
			}
		}

		skeleton->animations[animation->GetName()] = animation;
		skeleton->allAnimations.push_back(animation);
		return animation;
	}
	return nullptr;
}

std::shared_ptr<Texture> AssetLoader::LoadTexture(const std::string& aPath)
{
	auto texture = AssetRegistry::GetInstance()->GetAssetByPath<Texture>(aPath);
	if (texture) return texture;

	texture = CreateAsset<Texture>(aPath);
	const HRESULT hResult = DirectX::CreateDDSTextureFromFile(
		DX11::Device.Get(),
		std::wstring(aPath.begin(), aPath.end()).c_str(),
		texture->myTexture.GetAddressOf(),
		texture->mySRV.GetAddressOf()
	);

	if (SUCCEEDED(hResult))
	{
		return texture;
	}

	std::cout << "Failed to load " << aPath << std::endl;
	return nullptr;
}

//void AssetLoader::UpdateFolderStructure(std::filesystem::path aPath)
//{
//	for (auto& directoryEntry : std::filesystem::directory_iterator(aPath))
//	{
//		std::filesystem::path myCurrentDirectory = aPath.string();
//		const auto& path = directoryEntry.path();
//		auto relativePath = std::filesystem::relative(path, AssetPath);
//		std::string fileName = path.filename().string();
//		if (directoryEntry.is_directory()) 
//		{
//			fileName.push_back('/');
//			UpdateFolderStructure(myCurrentDirectory.string() + fileName);
//		}
//		AssetRegistry::GetInstance()->AddToAssetFolder(myCurrentDirectory.string(), fileName);
//	}
//}
//



AssetFolder AssetLoader::LoadFolder(const std::filesystem::path& aDirectory)
{
	AssetFolder newFolder;
	newFolder.path = aDirectory;
	newFolder.name = aDirectory.filename().string();

	for (auto& directoryEntry : std::filesystem::directory_iterator(aDirectory))
	{
		if (directoryEntry.is_directory())
		{
			auto aThingy = std::async(std::launch::async,&LoadFolder, directoryEntry.path());
			newFolder.directories.push_back(aThingy.get());
			//newFolder.directories.push_back(LoadFolder(directoryEntry.path()));
		}
		else
		{
			std::shared_ptr<Asset> asset = AssetLoader::LoadAsset(directoryEntry.path().string());
			newFolder.assets.push_back(asset);
			if (directoryEntry.path().extension() == ".mat")
			{
				UnloadedAsset mat;
				mat.path = directoryEntry.path().string();
				mat.asset = asset;
				mat.folderName = newFolder.name;
				myMaterialsToLoad.push_back(mat);
			}
			else if (!asset && directoryEntry.path().string().find("_anim") != string::npos)
			{
				UnloadedAsset anim;
				anim.path = directoryEntry.path().string();
				anim.asset = asset;
				anim.folderName = newFolder.name;
				newFolder.assets.push_back(asset);
				myAnimationsToLoad.push_back(anim);
			}
			else if (directoryEntry.path().extension() == ".pe")
			{
				UnloadedAsset pe;
				pe.path = directoryEntry.path().string();
				pe.asset = asset;
				pe.folderName = newFolder.name;
				myParticleSystemsToLoad.push_back(pe);
			}
			/*else if (directoryEntry.path().extension() == ".prefab")
			{
				UnloadedAsset prefab;
				prefab.path = directoryEntry.path().string();
				prefab.asset = asset;
				prefab.folderName = newFolder.name;
				myPrefabsToLoad.push_back(prefab);
			}*/
			else if (directoryEntry.path().extension() == ".ac")
			{
				UnloadedAsset ac;
				ac.path = directoryEntry.path().string();
				ac.asset = asset;
				ac.folderName = newFolder.name;
				myAnimationControllersToLoad.push_back(ac);
			}
		}
	}

	return newFolder;
}

void AssetLoader::LoadAllAssets()
{
	myAnimationsToLoad.clear();
	myMaterialsToLoad.clear();
	myParticleSystemsToLoad.clear();
	myAnimationControllersToLoad.clear();

	myAssetFolder = LoadFolder(AssetPath);
	myAssetFolder.name = "Assets";

	for (auto& anim : myAnimationsToLoad)
	{
		anim.asset = AssetLoader::LoadAnimation(anim.path);
	}
	myAnimationsToLoad.clear();

	for (auto& ac : myAnimationControllersToLoad)
	{
		ac.asset->LoadFromPath(ac.path.c_str());
	}
	myAnimationControllersToLoad.clear();

	for (auto& material : myMaterialsToLoad)
	{
		material.asset->LoadFromPath(material.path.c_str());
	}
	myMaterialsToLoad.clear();

	for (auto& pe : myParticleSystemsToLoad)
	{
		pe.asset->LoadFromPath(pe.path.c_str());
	}
	myParticleSystemsToLoad.clear();
	
	//for (auto& prefab : myPrefabsToLoad)
	//{
	//	prefab.asset->LoadFromPath(prefab.path.c_str());
	//}
	//myPrefabsToLoad.clear();

	AssetRegistry::GetInstance()->SortAll();
}

void AssetLoader::ThreadedLoading()
{
	if (myThreadActive == false)
	{
		myThreadActive = true;
		Engine::GetInstance()->GetThreadPool()->QueueJob([&] {	ThreadLoadAllAssets(); });
	}
}

bool AssetLoader::GetIsThreadActive()
{
	return myThreadActive;
}

void AssetLoader::ThreadLoadAllAssets()
{
	myAnimationsToLoad.clear();
	myMaterialsToLoad.clear();
	myParticleSystemsToLoad.clear();

	myAssetFolder = LoadFolder(AssetPath);
	myAssetFolder.name = "Assets";
	for (auto& anim : myAnimationsToLoad)
	{
		anim.asset = AssetLoader::LoadAnimation(anim.path);
	}
	myAnimationsToLoad.clear();
	for (auto& material : myMaterialsToLoad)
	{
		material.asset->LoadFromPath(material.path.c_str());
	}
	myMaterialsToLoad.clear();

	for (auto& pe : myParticleSystemsToLoad)
	{
		pe.asset->LoadFromPath(pe.path.c_str());
	}
	myParticleSystemsToLoad.clear();
	myThreadActive = false;
	AssetRegistry::GetInstance()->SortAll();
}



#pragma once
#include <string>

class Texture;
class Model;
class Animation;
struct Skeleton;
struct AssetFolder;
class Prefab;

struct UnloadedAsset
{
	std::shared_ptr<Asset> asset;
	std::string path;
	std::string folderName;
};

struct AssetFolder
{
	std::string name;
	std::filesystem::path path;
	std::vector<AssetFolder> directories;
	std::vector<std::shared_ptr<Asset>> assets;
};

class AssetLoader
{
public:
	static std::shared_ptr<Asset> LoadAsset(const std::string& aPath);
	static std::shared_ptr<Model> LoadFBX(const std::string& aPath);
	static std::shared_ptr<Prefab> LoadPrefab(const std::string& aPath);
	static std::shared_ptr<Animation> LoadAnimation(const std::string& aPath);
	static std::shared_ptr<Texture> LoadTexture(const std::string& aPath);
	//static void UpdateFolderStructure(std::filesystem::path aPath = "Assets/");
	static AssetFolder LoadFolder(const std::filesystem::path& aDirectory);
	static void LoadAllAssets();
	static void ThreadedLoading();
	static bool GetIsThreadActive();
	static AssetFolder myAssetFolder;

private:
	static void ThreadLoadAllAssets();
};
#pragma once
#include "EditorWindow.h"
#include "Assets/AssetLoader.h"
#include <stack>

class Texture;
class Asset;
class AssetBrowserWindow : public EditorWindow
{


	friend class AssetLoader;
public:
	AssetBrowserWindow();
	const inline std::filesystem::path& GetCurrentFolder() { return myCurrentDirectory->path; }
	void AddAssetToCurrentFolder(std::shared_ptr<Asset> anAsset);

private:
	template<typename T>
	void CreateAssetDropdown(const std::string& aTypeName, const std::string& anExtension);
	void Render() override;

	std::vector<std::shared_ptr<Asset>> Search(std::string& aTerm);

	std::filesystem::path myDragDropPath = { "Undefined" };
	std::shared_ptr<Texture> myDirectoryTex;
	std::shared_ptr<Texture> myFileTex;
	AssetFolder* myCurrentDirectory{ nullptr };
	std::vector<AssetFolder*> myCurrentPath;
	AssetFolder* myStartertDirectory{ nullptr };
	std::string mySearchTerm = "";
	std::vector<std::shared_ptr<Asset>> myCurrentSearchAssets;
	std::shared_ptr<Asset> myHoveredAsset;
	std::unordered_map<std::string, std::shared_ptr<Texture>> myFileIconExtensions;
	std::stack<std::vector<AssetFolder*>> myRedoQueue;
	std::list<std::vector<AssetFolder*>> myUndoQueue;
};

template<typename T>
void AssetBrowserWindow::CreateAssetDropdown(const std::string& aTypeName, const std::string& anExtension)
{
	if (ImGui::MenuItem(("New " + aTypeName).c_str())) 
	{
		std::string currentDir = std::filesystem::current_path().string() + "\\" + myCurrentDirectory->path.string() + "\\";
		std::string assetName = aTypeName;
		std::string extension = anExtension;

		std::string path = currentDir + assetName + extension;
		bool success = !AssetRegistry::GetInstance()->GetAsset<T>(assetName);
		int num = 2;
		while (!success)
		{
			std::string newName = assetName + std::to_string(num);

			if (!AssetRegistry::GetInstance()->GetAsset<T>(newName))
			{
				success = true;
				path = currentDir + newName + extension;
			}
			++num;
		}

		Catbox::CreateFileK(path);
		std::shared_ptr<Asset> tempAssetPtr = CreateAsset<T>(path);
		tempAssetPtr->SaveAsset(&path[0]);
		//Only used by shaders
		tempAssetPtr->LoadDefaultValues();
		AddAssetToCurrentFolder(tempAssetPtr);
	}
}
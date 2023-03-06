#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <typeindex>
#include "Texture.h"
#include "CommonUtilities\GUID.h"
#include "CommonUtilities\UtilityFunctions.hpp"
#include "CommonUtilities\InputHandler.h"
#include "..\External\ImGui\imgui_stdlib.h"

class Material;
class Texture;
class Model;
class Animator;
class Animation;
class MeshData;
struct Skeleton;
class PixelShader;
class VertexShader;
class GeometryShader;
class Scene;
class Prefab;

using namespace std;
class AssetRegistry
{
public:
	inline static AssetRegistry* GetInstance() { return Instance; }
	AssetRegistry();
	~AssetRegistry() = default;
	void LoadAssets();

	template<typename T>
	shared_ptr<T> GetAsset(const std::string& aName);

	template<typename T>
	shared_ptr<T> GetAssetByPath(const std::string& aPath);

	template<typename T>
	void RemoveAsset(const std::string& aName);

	void RenameAsset(Asset* anAsset, const std::string& aNewName);

	template<typename T>
	vector<shared_ptr<Asset>> GetAssets();

	template<typename T>
	void Sort();

	void SortAll();

	std::vector<std::shared_ptr<Asset>> GetAllAssets();

	template<typename T>
	inline std::shared_ptr<T> AssetDropdownMenu(T* aCurrentItem, const std::string& aDropdownName, bool& aReturnIsEdited);

	template<typename T>
	void AddToRegistry(shared_ptr<Asset> anAsset);

	void SaveAssetPaths();

	//void AddToAssetFolder(std::string& aKey, std::string& aFileName);

	//unordered_map<std::string, std::vector<std::string>>& GetAssetFolder();

	GameObject* AutoMakeGameObject(const std::filesystem::path& aPath);

private:
	void CreateShaders();
	void CreateMaterials();
	void CreateMeshes();
	void CreateModels();
	void CreateTextures();
	void SaveAssetPath(const Asset& anAsset);
	bool myLoadedStart = false;

	static AssetRegistry* Instance;

	unordered_map<type_index, vector<shared_ptr<Asset>>> myAssets;
	unordered_map<std::string, std::string> myAssetPaths;
	std::string myDropDownText = "";

};

template<typename T>
static shared_ptr<T> CreateAsset(const std::string& aPath)
{
	std::shared_ptr<T> existing = AssetRegistry::GetInstance()->GetAssetByPath<T>(aPath);
	if (existing)
	{
		if (dynamic_pointer_cast<Asset>(existing)->GetPathStr() != aPath)
		{
			MessageHandler::debugMessages.errors.push_back("Asset duplicate found: " + aPath + ", " + dynamic_pointer_cast<Asset>(existing)->GetPathStr());
		}
		return AssetRegistry::GetInstance()->GetAssetByPath<T>(aPath);
	}
	shared_ptr<T> asset = nullptr;
	asset = std::make_shared<T>();
	auto castedAsset = dynamic_pointer_cast<Asset>(asset);
	castedAsset->SetPath(aPath);
	castedAsset->ExtractNameFromPath();
	AssetRegistry::GetInstance()->AddToRegistry<T>(asset);
	return asset;
}

template <typename T>
inline void AssetRegistry::Sort()
{
	if (myLoadedStart)
	{
		for (int i = 0; i < myAssets[typeid(T)].size(); i++)
		{
			for (int j = 0; j < myAssets[typeid(T)].size() - i - 1; j++)
			{
				if (Catbox::ToLowerString(myAssets[typeid(T)][j]->GetName()) > Catbox::ToLowerString(myAssets[typeid(T)][j + 1]->GetName()))
				{
					auto temp = myAssets[typeid(T)][j];
					myAssets[typeid(T)][j] = myAssets[typeid(T)][j + 1];
					myAssets[typeid(T)][j + 1] = temp;
				}
			}
		}
	}
}

inline void AssetRegistry::SortAll()
{
	for (auto key : myAssets)
	{
		for (int i = 0; i < myAssets[key.first].size(); i++)
		{
			for (int j = 0; j < myAssets[key.first].size() - i - 1; j++)
			{
				if (Catbox::ToLowerString(myAssets[key.first][j]->GetName()) > Catbox::ToLowerString(myAssets[key.first][j + 1]->GetName()))
				{
					auto temp = myAssets[key.first][j];
					myAssets[key.first][j] = myAssets[key.first][j + 1];
					myAssets[key.first][j + 1] = temp;
				}
			}
		}
	}
}

inline std::vector<std::shared_ptr<Asset>> AssetRegistry::GetAllAssets()
{
	std::vector<std::shared_ptr<Asset>> allAssets;
	for (auto key : myAssets)
	{
		for (int i = 0; i < myAssets[key.first].size(); i++)
		{
			allAssets.push_back(myAssets[key.first][i]);
		}
	}
	return allAssets;
}

template<typename T>
inline static void CreateAsset(const std::string& aPath, std::shared_ptr<Asset> aAsset)
{
	std::shared_ptr<T> existing = AssetRegistry::GetInstance()->GetAssetByPath<T>(aPath);
	if (existing)
	{
		if (dynamic_pointer_cast<Asset>(existing)->GetPathStr() != aPath)
		{
			MessageHandler::debugMessages.errors.push_back("Asset duplicate found: " + aPath + ", " + dynamic_pointer_cast<Asset>(existing)->GetPathStr());
		}
	}

	aAsset->SetPath(aPath);
	aAsset->ExtractNameFromPath();
	AssetRegistry::GetInstance()->AddToRegistry<T>(aAsset);
}

template<typename T>
vector<shared_ptr<Asset>> AssetRegistry::GetAssets()
{
	return 	myAssets[typeid(T)];
}

template<typename T>
shared_ptr<T> AssetRegistry::GetAsset(const std::string& aName)
{
	for (auto& a : myAssets[typeid(T)])
	{
		auto n = Catbox::ToLowerString(a->GetName());
		if (n == Catbox::ToLowerString(aName)) return dynamic_pointer_cast<T>(a);
	}

	std::cout << "No asset found: " << aName << std::endl;
	return nullptr;
}

template<typename T>
inline shared_ptr<T> AssetRegistry::GetAssetByPath(const std::string& aPath)
{
	std::string name = Catbox::GetNameFromPath(aPath);
	for (auto& a : myAssets[typeid(T)])
	{
		if (a->GetName() == name) return dynamic_pointer_cast<T>(a);
	}

	return nullptr;
}

template<typename T>
inline void AssetRegistry::RemoveAsset(const std::string& aName)
{
	int index = 0;
	for (auto& a : myAssets[typeid(T)])
	{
		if (a->GetName() == aName)
		{
			myAssets[typeid(T)].erase(myAssets[typeid(T)].begin() + index);
			//Delete the file from folder too later
			return;
		}
		index++;
	}
}




template<typename T>
void AssetRegistry::AddToRegistry(shared_ptr<Asset> anAsset)
{
	myAssets[typeid(T)].push_back(anAsset);
	myAssetPaths[anAsset->GetName().c_str()] = anAsset->GetPath().string().c_str();
	if (myLoadedStart)
	{
		Sort<T>();
	}
}

template<typename T>
std::shared_ptr<T> AssetRegistry::AssetDropdownMenu(T* aCurrentItem, const std::string& aDropdownName, bool& aReturnIsEdited)
{
	aReturnIsEdited = false;
	int index = 0;
	auto assetList = GetAssets<T>();
	std::vector<std::string> assetNamesString(assetList.size() + 1, "");
	std::vector<const char*> assetNames(assetList.size() + 1, nullptr);
	assetNamesString[0] = "Empty";
	assetNames[0] = "Empty";

	int counter = 1;
	for (auto& asset : assetList)
	{
		if (aCurrentItem && aCurrentItem->myRootAsset == asset.get()) index = counter;
		assetNamesString[counter] = asset->GetName();
		assetNames[counter] = assetNamesString[counter].c_str();
		counter++;
	}

	if (ImGui::BeginCombo(aDropdownName.c_str(), assetNamesString[index].c_str()))
	{
		if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			ImGui::SetKeyboardFocusHere(0);
		ImGui::InputText("Search", &myDropDownText);
		for (int i = 0; i < assetNamesString.size(); i++)
		{
			bool isSelected = (assetNamesString[index] == assetNamesString[i]);

			if (Catbox::ToLowerString(assetNamesString[i]).find(Catbox::ToLowerString(myDropDownText)) != std::string::npos)
			{
				if (ImGui::Selectable((assetNamesString[i]).c_str(), false))
				{
					index = i;
					aReturnIsEdited = true;
				}
			}
		}
		ImGui::EndCombo();
	}
	if (aReturnIsEdited)
	{
		myDropDownText = "";
		if (index == 0) return nullptr;
		return dynamic_pointer_cast<T>(assetList[index - 1]);
	}

	if (!assetList.empty())
	{
		if (const auto& payload = assetList[0]->DragDropTarget())
		{
			aReturnIsEdited = true;
			std::filesystem::path aPath = *(std::filesystem::path*)payload->Data;
			return GetAssetByPath<T>(aPath.string());
		}
	}

	return nullptr;
}
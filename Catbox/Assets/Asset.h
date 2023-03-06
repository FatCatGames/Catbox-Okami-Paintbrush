#pragma once
#include <fstream>
#include <filesystem>
#include <memory>
#include "CommonUtilities\GUID.h"
#include "CommonUtilities\RapidJsonWrapper.h"

struct ImGuiPayload;

class Asset : public std::enable_shared_from_this<Asset>
{
public:
	Asset();
	virtual ~Asset() = default;
	const std::string& GetName() const { return myName; }
	const std::filesystem::path& GetPath() const { return myPath; }
	const std::string& GetPathStr() const { return myPathStr; }
	inline void SetName(const std::string aName) { myName = aName; }
	void SetPath(const std::string aPath);
	inline void SetSelected(int aKey, bool aValue) { (aKey == 0 ? myIsSelected : myIsRightClicked) = aValue; }
	inline bool IsSelected(int aKey) { return aKey == 0 ? myIsSelected : myIsRightClicked; }
	virtual void RenderInProperties();
	const ImGuiPayload* DragDropTarget();
	virtual void RenderInProperties(std::vector<Asset*>& anAssetList);
	void Deselect();
	void Select();
	void AddSelected();
	void ExtractNameFromPath();
	virtual void SaveAsset(const char* aPath) {};
	virtual void LoadFromPath(const char*) {};
	//ONLY USED BY SHADERS
	virtual void LoadDefaultValues() {};
	virtual void DoubleClickAsset() { };
	virtual bool operator<(Asset& aAsset) const;
	virtual bool operator>(Asset& aAsset) const;
	inline Asset* GetRootAsset() { return myRootAsset; }
	/*virtual std::string SaveAssetData() { return "SaveAssetData() not overriden"; }
	std::string GetAssetData() { return SaveAssetData(); }
	void LoadFromString(std::string someData) { }*/

protected:
	friend class AssetRegistry;
	friend class AssetBrowserWindow;
	Catbox::GUID myAssetId;
	static int globalAssetCount;
	Asset* myRootAsset = nullptr;
	std::string myRuntimeId; //used as a faster unique identifier for imgui purposes
	std::filesystem::path myPath;
	std::string myPathStr;
	std::string myName = "Unnamed asset";
	bool myIsSelected = false;
	bool myIsRightClicked = false;
	int myVersion = 0;
	bool myWasCreatedInRuntime = false;

	void RenderNameInputBox();

};

template<typename T>
std::vector<T*> AssetVectorCast(std::vector<Asset*>& anAssetList)
{
	std::vector<T*> castedAssetList;
	for (Asset* asset : anAssetList)
	{
		T* castedAsset = dynamic_cast<T*>(asset);
		castedAssetList.push_back(castedAsset);
	}
	return castedAssetList;
}
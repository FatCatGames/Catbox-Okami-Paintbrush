#pragma once
#include "EditorWindow.h"
#include "Assets/Asset.h"

class Asset;
class PropertiesWindow : public EditorWindow
{
public:
	inline const bool& IsLocked() { return myIsLocked; }
	inline void Lock() { myIsLocked = true; }
	void SetSelected(Asset* anAsset);
	void AddSelected(Asset* anAsset);
	void DeselectAll();
	void Deselect(Asset* anAsset);
	inline std::vector<Asset*> GetSelected() { return mySelectedAssets; }

private:
	void Render() override;
	bool myIsLocked = false;
	std::vector<Asset*> mySelectedAssets;
};
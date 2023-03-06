#pragma once
class ScriptGraph;
class Blueprint : public Asset
{
public:
	void SaveAsset(const char* aPath) override;
	void LoadFromPath(const char* aPath) override;
	void DoubleClickAsset() override;
	const std::shared_ptr<ScriptGraph> GetGraph() const { return myGraph; }
	void RenderInProperties(std::vector<Asset*>& anAssetList) override;

private:
	std::shared_ptr<ScriptGraph> myGraph;
};
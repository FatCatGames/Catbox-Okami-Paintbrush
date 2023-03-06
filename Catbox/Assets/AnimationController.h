#pragma once
#include "Assets\Asset.h"
#include "Graphics/Animations/Animation.h"
class ScriptGraphVariable;
enum class AnimationState;
class AnimationGraph;
class AnimationController : public Asset
{
public:
	void SaveAsset(const char* aPath) override;
	void LoadFromPath(const char* aPath) override;
	void DoubleClickAsset() override;
	void RenderInProperties(std::vector<Asset*>& anAssetList) override;

	void Reset();
	std::shared_ptr<Animation> UpdateAnimations(Animator* anActor);
	std::shared_ptr<Animation> GetCurrentAnimation();
	//int GetCurrentFrame() { return myCurrentLayer->myFrameIndex; }
	//int GetNextFrame() { return myCurrentLayer->myNextFrameIndex; }
	//float GetFramePercent() { return myCurrentLayer->myFramePercent; }
	//AnimationState GetAnimationState() { return myCurrentLayer->myCurrentState; }
	bool GetShouldLoop();
	std::shared_ptr<AnimationController> MakeInstance();
	std::unordered_map<std::string, std::shared_ptr<ScriptGraphVariable>>& GetVariables() { return myVariables; }

	std::unordered_map<std::string, std::shared_ptr<AnimationGraph>>& GetLayers() { return myLayers; }
	std::shared_ptr<AnimationGraph> AddLayer(std::string& aName);
	void RemoveLayer(std::string& aName);
	void SetActiveLayer(const std::string& aLayer, bool aIsActive);

	std::shared_ptr<Skeleton> GetSkeleton() { return mySkeleton; }
	void SetSkeleton(std::shared_ptr<Skeleton> aSkeleton);
private:


	std::unordered_map<string, std::shared_ptr<AnimationGraph>> myLayers;
	std::shared_ptr<AnimationGraph> myCurrentLayer;
	std::unordered_map<std::string, std::shared_ptr<ScriptGraphVariable>> myVariables;
	std::shared_ptr<Skeleton> mySkeleton;
	std::vector<std::vector<Animation::Frame>> myFrames;
	std::vector<std::vector<Animation::Frame>> myNextFrames;
	std::vector<float> myPercentages;
	std::vector<std::vector<int>> myBondeIdToAnimationList;
	std::vector<std::vector<float>> myBlendedValues;
};
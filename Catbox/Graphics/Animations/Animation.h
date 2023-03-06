#pragma once
#include <vector>
#include <string>
#include <CommonUtilities/Matrix4x4.hpp>
#include "Assets\Asset.h"

enum class AnimationState
{
	Playing,
	Finished
};

class Animation : public Asset
{
public:
	struct Frame
	{
		std::unordered_map<std::string, Catbox::Matrix4x4<float>> localTransforms;
		std::unordered_map<std::string, Catbox::Matrix4x4<float>> transposedLocalTransforms;
		std::unordered_map<std::string, Vector3f> localPositions;
		std::unordered_map<std::string, Vector4f> localRotations;
		std::unordered_map<std::string, Vector3f> localScales;
	};
	const inline std::vector<Frame>& GetFrames() { return frames; }

	virtual void RenderInProperties() override;

private:
	void SaveAsset(const char* aPath) override;
	friend class AssetLoader;
	friend class Animator;
	friend class AnimationController;
	std::vector<Frame> frames;
	unsigned int frameCount;
	float duration; //in seconds
	float framesPerSecond;
};
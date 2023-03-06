#pragma once
#include "Graphics/Animations/SkeletonData.h"

enum class AnimationEventType
{
	Prefab,
	PlaySound
};

struct PrefabData
{
	std::shared_ptr<Prefab> prefab;
	bool worldSpace = false;
	Vector3f position;
	Skeleton::Bone* bone = nullptr;
};

class AnimationController;
class AnimationEvent
{
public:
	PrefabData myPrefabData;
	unsigned int myFrame = 0;
	AnimationEventType myType = AnimationEventType::Prefab;

	AnimationEvent() = default;
	~AnimationEvent() = default;
	inline void SetIndex(size_t anIndex) { myID = anIndex; }
	void Execute(Animator* anActor);
	void RenderInProperties(AnimationController* aController);

private:
	size_t myID;
	std::string mySoundEffectName = "";
};


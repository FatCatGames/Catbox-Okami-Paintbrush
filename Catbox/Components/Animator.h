#pragma once

class AnimationController;
class ModelInstance;
class Animation;
struct Skeleton;
struct SkeletonInstance;
class Animator : public Component
{
public:
	~Animator() override;
	void Init() override;
	void Awake() override;
	void Update() override;
	void RunInEditor() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void OnModelChanged();
	void SetModelInstance(ModelInstance* aModelInstance);
	//void SetAnimation(std::shared_ptr<Animation> anAnimation);
	std::shared_ptr<SkeletonInstance> GetSkeleton() const { return mySkeleton; }
	std::shared_ptr<Animation> GetCurrentAnimation() const { return myCurrentAnimation; };
	//void ResetAnimation();
	//bool AnimationFinished();
	inline void SetEnableBoneParenting(bool aValue) { myEnableBoneParenting = aValue; }
	void SetActiveLayer(const std::string& aLayer, bool aIsActive);
	void SetBool(const std::string& aName, bool aValue);
	void SetFloat(const std::string& aName, float aValue);
	void SetInt(const std::string& aName, int aValue);

private:
	void Animate();
	ModelInstance* myModel = nullptr;
	std::shared_ptr<AnimationController> myController;
	std::shared_ptr<SkeletonInstance> mySkeleton = nullptr;
	std::shared_ptr<Animation> myCurrentAnimation = nullptr;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	void GetChildrenBoneIDs(const int& aID, std::vector<int>& aOutVector);


	int myRootIndex = 0;
	int myAnimationIndex = 0;
	int myHoveredIndex = 0;
	bool myEnableBoneParenting = false;
};

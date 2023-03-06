#pragma once
#include "Graphics\Rendering\Buffers\CBuffer.hpp"

class Model;
class SphereCollider;
class Material;
struct SkeletonInstance;
class ModelInstance : public Component
{
public:

	~ModelInstance();
	void Update() override;
	void RunInEditor() override;
	void Render();
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void ClearModel();
	void OnTransformChanged() override;
	void SetModel(std::shared_ptr<Model> aModel);
	inline std::shared_ptr<Model> GetModel() { return myModel; }
	std::shared_ptr<SkeletonInstance> GetSkeleton() { return mySkeletonInstance; }
	std::shared_ptr<Material> GetMaterial(int anIndex);
	std::vector<std::shared_ptr<Material>> GetMaterials() const { return myMaterials; }
	void SetMaterial(std::shared_ptr<Material> aMaterial, int anIndex, bool isInstance = false);
	inline bool IsUsedForNavigation() { return myUsedForNavMesh; }
	void SetRenderToUse(Renderer aChoice) { myRendererToUse = aChoice; }
	bool ShouldDrawOutline();
	void Init() override;
	void Awake() override;
	bool ShouldInstanceMaterial() const { return myShouldBeInstanceMaterial; }
	Renderer GetRenderer() const { return myRendererToUse; }
	bool ShouldCastShadows() const { return myShouldCastShadows; }
	void SetShouldCastShadows(bool aChoice) { myShouldCastShadows = aChoice; }


private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	std::string mySearchTerm;
	int myInstanceCount = 0;
	std::vector<Transform> myInstances;
	std::shared_ptr<Model> myModel;
	std::shared_ptr<SkeletonInstance> mySkeletonInstance;
	std::vector<std::shared_ptr<Material>> myMaterials;
	bool myShouldBeInstanceMaterial;
	bool myUsedForNavMesh = false;
	Renderer myRendererToUse;
	bool myInstanceMaterial = false;
	bool myShouldCastShadows = true;
	void UpdateInstanceBuffer();
};
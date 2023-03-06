#pragma once

#include "MeshData.h"
#include "Material.h"
#include "Graphics/Animations/SkeletonData.h"


class Model : public Asset
{
public:

	struct InstanceData
	{
		Catbox::Matrix4x4<float> transform;
	};
	Model() = default;
	Model(std::vector<std::shared_ptr<MeshData>> aMeshDataList);
	Model(std::shared_ptr<MeshData> aMeshdata);

	const float GetBoundsRadius();
	std::shared_ptr<MeshData> GetModelPart(int anIndex);
	std::vector<std::shared_ptr<MeshData>>& GetModelParts();
	void CreateBoundingUnits();
	std::array<float, 6> GetBoundingUnits() const { return myBoundingUnits; }
	inline bool IsUnitsConverted() { return myConvertUnits; }
	inline std::shared_ptr<Skeleton> GetSkeleton() { return mySkeleton; }
	inline Vector3f GetCenter() const { return myCenter; }
	inline Vector3f GetPivot() const { return myPivot; }
	void SetCenter(Vector3f aCenter) { myCenter = aCenter; }
	void RenderInstance(Transform* aTransform);
	inline std::vector<InstanceData>& GetRenderedInstances() { return myInstances; }
	inline void ClearRenderedInstances() { myInstances.clear(); }
	void SetMaxInstances(int aMaxCount);
	ComPtr<ID3D11Buffer> GetInstanceBuffer() { return myInstanceBuffer; }


private:
	friend class AssetRegistry;
	friend class AssetLoader;

	void SaveAsset(const char* aPath) override;
	Vector3f myPivot = {};
	std::shared_ptr<Skeleton> mySkeleton;
	std::vector<std::shared_ptr<MeshData>> myModelParts;
	ComPtr<ID3D11Buffer> myBuffer;
	ComPtr<ID3D11Buffer> myInstanceBuffer;
	Vector3f myCenter = {};
	float myBoundsRadius = 0;
	bool myConvertUnits = true; //meter to cm
	std::array<float, 6> myBoundingUnits;
	std::vector<InstanceData> myInstances{};
};
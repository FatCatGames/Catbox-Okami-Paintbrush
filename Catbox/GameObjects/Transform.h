#pragma once
#include "../CommonUtilities/Matrix4x4.hpp"

class NavMeshExporter;
class GameObject;
class Transform
{
public:
	enum class Mode
	{
		World,
		Local
	};

	enum class Operation
	{
		Translate,
		Rotate,
		Scale
	};


	Transform();
	~Transform();

	Catbox::Matrix4x4<float> toWorldSpace();
	Catbox::Matrix4x4<float> toLocalSpace();

	const Vector3f localPos();
	inline const Vector3f& localRot() { return myRotation; }
	inline const Vector3f& localScale() { return myScale; }

	const Vector3f worldPos();
	const Vector3f worldRot();
	const Vector3f worldScale();

	inline const Vector3f right() { return myWorldTransform.right(); }
	inline const Vector3f left() { return myWorldTransform.left(); }
	inline const Vector3f up() { return myWorldTransform.up(); }
	inline const Vector3f down() { return myWorldTransform.down(); }
	inline const Vector3f forward() { return myWorldTransform.forward(); }
	inline const Vector3f back() { return myWorldTransform.back(); }

	void SetLocalTransform(const Vector3f& aNewPos, const Vector3f& aNewRot, const Vector3f& aNewScale);
	void SetWorldTransform(const Vector3f& aNewPos, const Vector3f& aNewRot, const Vector3f& aNewScale);

	void SetLocalPos(const Vector3f& aNewPos);
	void SetLocalPos(const float& aX, const float& aY, const float& aZ);
	void SetWorldPos(const Vector3f& aNewPos);
	void SetWorldPos(const float& aX, const float& aY, const float& aZ);
	void Translate(const Vector3f& aVector);
	void Translate(const float& aX, const float& aY, const float& aZ);
	void SetLocalRot(const Vector3f& aRotation);
	void SetLocalRot(const float& aX, const float& aY, const float& aZ);
	void SetWorldRot(const Vector3f& aRotation);
	void SetWorldRot(const float& aX, const float& aY, const float& aZ);
	void Rotate(const Vector3f& aRotation);
	void Rotate(const float& aX, const float& aY, const float& aZ);
	void SetLocalScale(const Vector3f& aNewScale);
	void SetLocalScale(const float& aX, const float& aY, const float& aZ);
	void SetWorldScale(const Vector3f& aNewScale);
	void SetWorldScale(const float& aX, const float& aY, const float& aZ);
	void SetCenter(Vector3f aCenter);
	void SetBoundsRadius(float aRadius);
	Vector3f GetCenter() const { return myCenter; }
	float GetBoundsRadius() const { return myBoundsRadius; }
	void LookAt(const Vector3f& aTarget);
	void LookAtHorizontal(const Vector3f& aTarget);

	void Reset();

	void SetParent(Transform* aParent);
	Transform* GetParent() { return myParent; }
	Transform* GetRootParent() { return myRootParent; }
	std::vector<Transform*>& GetChildren();
	inline bool IsParentedToBone() { return myBoneParentId != -1; }
	const Catbox::Matrix4x4<float>& GetWorldTransformMatrix();
	const Catbox::Matrix4x4<float>& GetLocalTransformMatrix();
	void SetWorldTransformMatrix(const Catbox::Matrix4x4<float>& aMatrix);
	void SetLocalTransformMatrix(const Catbox::Matrix4x4<float>& aMatrix);

	GameObject* GetGameObject() { return myGameObject; }

	static void RenderInProperties(std::vector<Transform*>& aTransformList);

private:

	void Save();
	void Load(rapidjson::Value& aTransformData);
	void LoadChildren(rapidjson::Value& aTransformData);
	void AddChild(Transform& aChild);
	void RemoveChild(Transform* aChild);
	void UpdateTransform();
	void UpdateChildren();
	void UpdateWorldTransform();
	void RebuildMatrix();

	Catbox::Matrix4x4<float> myLocalTransform;
	Catbox::Matrix4x4<float> myRotationMatrix;
	Catbox::Matrix4x4<float> myWorldTransform;

	Vector3f myPosition;
	Vector3f myScale;
	Vector3f myRotation;
	Vector3f myCenter;
	float myBoundsRadius = 1;


	GameObject* myGameObject = nullptr;
	Transform* myParent = nullptr;
	Transform* myRootParent = nullptr;
	std::vector<Transform*> myChildren;

	int myBoneParentId = -1;
	bool myIsBone = false;

	friend class GameObject;
	friend class NavMeshExporter;
	friend class ModelInstance;
	friend class SceneHierarchyWindow;
	static bool ImguiRenderWorldTransform;
};
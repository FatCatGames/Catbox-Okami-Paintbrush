#pragma once
#include "../../CommonUtilities/Matrix4x4.hpp"
#include "Assets\Texture.h"
#include "Graphics\Rendering\PostProcessingVolume.h"
#include "CommonUtilities/Plane.hpp"
#include "CommonUtilities/Sphere.hpp"
#include "GameObjects\Component.h"
#include "ComponentTools\Event.h"
#include "Graphics\Rendering\DepthStencil.h"

class Camera : public Component
{
public:
	struct CameraSettingsTemplate
	{
		std::string name;
		Vector3f rotation;
		Vector3f offset;
		float fov;
	};
	struct Frustum
	{
		Catbox::Plane<float> topFace;
		Catbox::Plane<float> bottomFace;

		Catbox::Plane<float> rightFace;
		Catbox::Plane<float> leftFace;

		Catbox::Plane<float> farFace;
		Catbox::Plane<float> nearFace;
	};


	Camera();
	Camera(const Camera& aCamera);
	~Camera();
	void Awake() override;
	void OnEnable() override;
	void Init() override;
	void OnTransformChanged() override;
	Catbox::Matrix4x4<float>& GetProjectionMatrix();
	inline float GetFarPlane() { return myFarPlane; }
	inline float GetNearPlane() { return myNearPlane; }
	inline float GetFoV() { return myFoV; }
	inline void SetFarPlane(float aFarPlane) { myFarPlane = aFarPlane; }
	inline void SetNearPlane(float aNearPlane) { myNearPlane = aNearPlane; }
	inline void SetFoV(float anFoV) { myFoV = anFoV; }
	
	Vector3f ToCameraSpace(const Vector3f& aPosition);
	Vector4f ToProjectionSpace(const Vector3f& aPosition);

	virtual void RotateAroundObject(Transform* aTransform, float someDegreesX, float someDegreesY, Vector3f anOffset = {0.0f, 0.0f, 1.0f}, bool shouldLookAtObject = true);
	virtual void RotateAroundObject(Vector3f aPosition, float someDegreesX, float someDegreesY, Vector3f anOffset = {0.0f, 0.0f, 1.0f}, bool shouldLookAtObject = true);

	inline Texture& GetRenderTexture() { return myRenderTexture; }
	inline DepthStencil& GetDepthStencil() { return myDepthStencil; }
	std::shared_ptr<PostProcessingVolume> GetPostProcessingVolume();
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	bool IsPostProcessingEnabled();
	bool IsInsideFrustum(Transform* aObject);
	void AddCameraSettingsTemplate(CameraSettingsTemplate aCameraSettingsTemplate);
	Frustum CreateFrustum();

	Vector3f MouseToWorldPos(Vector2i aMousePos, float anYPos);
	Vector4f MouseToWorld(Vector2i aMousePos, int aDepth = 1);
	Vector3f GetMouseClickDirection(Vector2i aMousePos);

protected:
	void RunInEditor() override;
	void Update() override;
	void CreateProjectionMatrix();
	void CreateRenderTexture();
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	Catbox::Matrix4x4<float> myProjectionMatrix;
	float myFoV = 90;
	float myNearPlane = 0.1f;
	float myFarPlane = 1000.f;

	Frustum myFrustum;

	// Used for RotateAroundObject.
	float myXRotationRadians;
	float myYRotationRadians;

	std::shared_ptr<PostProcessingVolume> myPostProcessingVolume;
	bool myIsPreviewing = false;

	Texture myRenderTexture;
	DepthStencil myDepthStencil;
	Listener myOnResolutionChangedListener;

	// Only used for Editor Camera
	void OverrideTransform(Transform& aTransform);
	friend class EditorCamera;
	std::vector<CameraSettingsTemplate> myCameraSettings;
};
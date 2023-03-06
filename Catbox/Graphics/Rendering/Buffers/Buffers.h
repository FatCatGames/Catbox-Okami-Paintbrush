#pragma once
#include "CBuffer.hpp"
#include "StructuredBuffer.hpp"
#include "Graphics\Rendering\PostProcessingVolume.h"
#include "../Buffers/GBuffer.h"
#include "Assets\Material.h"

using namespace Catbox;
struct ID3D11Buffer;
class SceneLightData;
struct LightData;
class Light;
class Camera;

class Buffers
{
public:
	bool Initialize();
	void CreateFrameBuffer(Camera* aCamera);
	void CreateLightsBuffer(SceneLightData* aSceneLightData);
	inline const GBuffer& GetGbuffer() { return myGBuffer; }

private:

	friend class ForwardRenderer;
	friend class DeferredRenderer;
	friend class PostProcessRenderer;
	friend class DebugRenderer;
	friend class ShadowRenderer;
	friend class GraphicsEngine;

	struct FrameBufferData
	{
		Matrix4x4<float> View;
		Matrix4x4<float> Projection;
		Vector3f CamPosition;
		int DebugMode;
		float TotalTime;
		float DeltaTime;
		Vector2f Resolution;
		float NearPlane;
		float FarPlane;
		Vector2f padding;
		Vector4f FrustumCorners[4];
	} myFrameBufferData;

	struct ColorBufferData
	{
		Color Color;
	} myColorBufferData;

	struct ObjectBufferData
	{
		Matrix4x4<float> world; //64 bytes
		Vector3f scale;
		int id = 0; //4 bytes
		float OB_SelectionMask; //4 bytes
		float aliveTime = 0; //4 bytes
		alignas(4) bool isStatic = true; //4 bytes
		alignas(4) bool isInstanced; //4 bytes
		alignas(4) bool isTiling = false;
		Vector3f padding;
	} myObjectBufferData;


	struct SkeletonBufferData
	{
		Matrix4x4<float> BoneData[128]; //8192 bytes
	} mySkeletonBufferData;

	struct SceneLightsBufferData
	{
		int pointStartIndex; //4 bytes
		int spotStartIndex; //4 bytes
		int IBLStartIndex;//4 bytes
		int lightsCount;//4 bytes
	} mySceneLightsBufferData;
	
	Light* myEnvironmentLight;
	GBuffer myGBuffer;
	StructuredBuffer<LightData> myLightBuffer;
	CBuffer<SceneLightsBufferData> mySceneLightsBuffer;
	CBuffer<FrameBufferData> myFrameBuffer;
	CBuffer<ObjectBufferData> myObjectBuffer;
	CBuffer<SkeletonBufferData> mySkeletonBuffer;
	CBuffer<Material::MaterialBufferData> myMaterialBuffer;
	CBuffer<ColorBufferData> myColorBuffer;
};
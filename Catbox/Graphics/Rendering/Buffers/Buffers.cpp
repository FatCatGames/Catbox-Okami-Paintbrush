#include "stdafx.h"
#include "Buffers.h"
#include "Components\3D\Camera.h"
#include "Graphics/Rendering/SceneLightData.h"
#include "Components\3D\Light.h"

bool Buffers::Initialize()
{
	if (!myObjectBuffer.Initialize()) return false;
	if (!myFrameBuffer.Initialize()) return false;
	if (!mySkeletonBuffer.Initialize()) return false;
	if (!myMaterialBuffer.Initialize()) return false;
	if (!mySceneLightsBuffer.Initialize()) return false;
	if (!myLightBuffer.Initialize()) return false;
	if (!myColorBuffer.Initialize()) return false;
	return true;
}

void Buffers::CreateFrameBuffer(Camera* aCamera)
{
	assert(aCamera != nullptr && "Tried to create frame buffer with no camera assigned!");
	myFrameBufferData.View = Catbox::Matrix4x4<float>::GetFastInverse(aCamera->GetTransform()->GetWorldTransformMatrix());
	auto windowSize = DX11::GetResolution();
	myFrameBufferData.Resolution = {static_cast<float>(windowSize.x) / 4.f, static_cast<float>(windowSize.y) / 4.f};
	myFrameBufferData.Projection = aCamera->GetProjectionMatrix();
	myFrameBufferData.CamPosition = aCamera->GetTransform()->worldPos();

	myFrameBufferData.DeltaTime = deltaTime;
	myFrameBufferData.TotalTime = static_cast<float>(Engine::GetInstance()->GetTotalTime());

	const float aspectRatio = (float)myFrameBufferData.Resolution.x / (float)myFrameBufferData.Resolution.y;
	myFrameBufferData.FarPlane = aCamera->GetFarPlane();
	myFrameBufferData.NearPlane = aCamera->GetNearPlane();
	const float halfHeight = myFrameBufferData.FarPlane * tanf(0.5f * aCamera->GetFoV());
	const float halfWidth = aspectRatio * halfHeight;

	myFrameBufferData.FrustumCorners[0] = Vector4f(-halfWidth, -halfHeight, myFrameBufferData.FarPlane, 0.0f);
	myFrameBufferData.FrustumCorners[1] = Vector4f(-halfWidth, halfHeight, myFrameBufferData.FarPlane, 0.0f);
	myFrameBufferData.FrustumCorners[2] = Vector4f(halfWidth, halfHeight, myFrameBufferData.FarPlane, 0.0f);
	myFrameBufferData.FrustumCorners[3] = Vector4f(halfWidth, -halfHeight, myFrameBufferData.FarPlane, 0.0f);

	myFrameBuffer.SetData(&myFrameBufferData);
}

void Buffers::CreateLightsBuffer(SceneLightData* aSceneLightData)
{
	if (!aSceneLightData) return;
	mySceneLightsBufferData.lightsCount = 0;
	mySceneLightsBufferData.pointStartIndex = aSceneLightData->GetPointStartIndex();
	mySceneLightsBufferData.spotStartIndex = aSceneLightData->GetSpotStartIndex();
	mySceneLightsBufferData.IBLStartIndex = aSceneLightData->GetIBLStartIndex();
	const std::vector<LightData*>& sceneLights = aSceneLightData->GetLights();
	LightData Lights[128];
	for (size_t i = 0; i < sceneLights.size(); i++)
	{
		if (!sceneLights[i]->isActive)
		{
			if (sceneLights[i]->type < 1) --mySceneLightsBufferData.pointStartIndex;
			if (sceneLights[i]->type < 2) --mySceneLightsBufferData.spotStartIndex;
			if (sceneLights[i]->type < 3) --mySceneLightsBufferData.IBLStartIndex;
			continue;
		}
		Lights[mySceneLightsBufferData.lightsCount++] = *sceneLights[i];
	}

	myLightBuffer.SetData(Lights);
	mySceneLightsBuffer.SetData(&mySceneLightsBufferData);
}

#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "CommonUtilities/Matrix4x4.hpp"
#include "../Buffers/CBuffer.hpp"
#include "../Buffers/StructuredBuffer.hpp"
#include "../DXDeviceStates.h"
#include "Editor\Windows\SceneWindow.h"

using namespace Catbox;

class Buffers;
class ModelInstance;
class Light;

class ShadowRenderer
{
public:
	bool Initialize(std::shared_ptr<Buffers> aBuffers);
	void Render(const std::vector<ModelInstance*>& aModelList, std::vector<Light*>& aLightList);

private:
	std::shared_ptr<Buffers> myBuffers;
};
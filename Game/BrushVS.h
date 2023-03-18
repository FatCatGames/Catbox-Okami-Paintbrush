#pragma once
#include "Assets\Shader.h"
#include "Graphics\Rendering\Buffers\CBuffer.hpp"

class BrushVS : public VertexShader
{
public:
	BrushVS();
	void SetResource() override;
	void UpdateShaderData(const Vector3f& aPaintDir, float aMouseDelta);
	struct PaintData
	{
		Vector3f paintDir;
		float strength;
	} myPaintData;

private:

	Catbox::CBuffer<PaintData> myPaintDataBuffer;
};
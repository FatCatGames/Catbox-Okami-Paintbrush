#pragma once
#include "Assets\Shader.h"

class CanvasPS : public PixelShader
{
public:
	CanvasPS();

	void SetResource() override;

private:
	
	struct PaintBuffer 
	{
		float paperTexPercent;
		Vector3f padding;
	} myPaintBufferData;

	Catbox::CBuffer<PaintBuffer> myPaintBuffer;
};
#pragma once
#include "Assets\Shader.h"

class CanvasPS : public PixelShader
{
public:
	
	struct PaintingData
	{
		int x = 0;
		int y = 0;
		int radius = 5;
		float padding = 0;
	} myPaintingData;

	CanvasPS();
	void SetResource() override;
	void Paint(int aPosX, int aPosY, int aRadius);
	void Clear();

private:
	Texture myPainting;
	float myClearColor[3] = { 1, 1, 1 };
	Catbox::CBuffer<PaintingData> myPaintingDataBuffer;
};
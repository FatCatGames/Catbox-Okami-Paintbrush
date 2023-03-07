#pragma once
#include "Assets\Shader.h"

class CanvasPS : public PixelShader
{
public:
	CanvasPS();
	void SetResource() override;
	void Paint(int aPosX, int aPosY, float aRadius);
	void Clear();

private:
	Texture myPainting;
	float myClearColor[3] = { 1, 1, 1 };
};
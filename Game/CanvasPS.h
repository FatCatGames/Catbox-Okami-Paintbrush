#pragma once
#include "Assets\Shader.h"

class CanvasPS : public PixelShader
{
public:
	CanvasPS();
	void SetResource() override;
	void Paint(int aPosX, int aPosY, int aRadius, const Color& aColor);
	void Clear();

private:
	Texture myPaintingTex;
	Texture myStagingTex;
	const int myWidth = 1920;
	const int myHeight = 1080;
};
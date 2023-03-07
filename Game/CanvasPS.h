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
	Texture myPainting;
};
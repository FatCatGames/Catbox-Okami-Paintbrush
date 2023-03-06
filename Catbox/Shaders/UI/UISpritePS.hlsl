#include "../Struct/ShaderStructs.hlsli"
#include "UIShaderStructs.hlsli"

Texture2D albedoTexture : register(t0);


PixelOutput main(UISpriteVertexToPixel input)
{
	PixelOutput result;
	result.Color = albedoTexture.Sample(defaultSampler, input.uv);
	if (result.Color.a < 0.05f) discard;
	result.id = UI_id;
	result.Color *= UI_color;

	return result;
}
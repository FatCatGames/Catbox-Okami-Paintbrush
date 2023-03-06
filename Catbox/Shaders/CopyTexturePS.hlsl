#include "Struct/ShaderStructs.hlsli"

struct CopyPixelOutput
{
	float4 Color : SV_TARGET0;
};

Texture2D sourceTexture : register(t0);

CopyPixelOutput main(DeferredVertexToPixel input)
{
	CopyPixelOutput result;
	const float4 pixel = sourceTexture.Sample(defaultSampler, input.UV);
	if (pixel.a < 0.05f) discard;
	result.Color = pixel;
	return result;
}
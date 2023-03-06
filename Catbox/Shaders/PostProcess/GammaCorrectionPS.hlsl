#include "../Struct/ShaderStructs.hlsli"
#include "../PBRFunctions.hlsli"
#include "Tonemaps.hlsli"

Texture2D sourceTexture : register(t0);


struct GammaPixelOutput
{
	float4 Color : SV_TARGET0;
};

GammaPixelOutput main(DeferredVertexToPixel input)
{
	GammaPixelOutput result;
	const float4 pixel = sourceTexture.Sample(defaultSampler, input.UV);
	if (pixel.a < 0.05f) discard;
	result.Color.rgb = LinearToGamma(Tonemap_ACES(pixel.rgb)).rgb;
	result.Color.a = 1;
	return result;
}
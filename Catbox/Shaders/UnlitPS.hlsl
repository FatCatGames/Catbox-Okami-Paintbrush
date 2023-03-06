#include "Struct/ShaderStructs.hlsli"

Texture2D albedoTexture : register(t0);

PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	float4 albedo = albedoTexture.Sample(defaultSampler, input.AlbedoUV).rgba;
	if (albedo.a < 0.05f) discard;

	result.Color.rgba = albedo * MB_Color;
    return result;
}
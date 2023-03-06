#include "ParticleShaderStructs.hlsli"
#include "../Struct/ShaderStructs.hlsli"

Texture2D albedoTexture : register(t0);

ParticlePixelOutput main(ParticleGeometryToPixel input)
{
	ParticlePixelOutput result;
	float4 textureColor = albedoTexture.Sample(defaultSampler, input.UV);

	if (textureColor.a < 0.05f) discard;
	//result.id = OB_id;
	result.Color.rgba = textureColor.rgba * input.Color.rgba;
	return result;
}
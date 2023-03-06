#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"

Texture2D screenTexture : register(t0);
Texture2D blurredLuminanceTexture : register(t1);

cbuffer BloomData : register(b10)
{
	float PPBloom_Strength;
	float PPBloom_Cutoff;
	float PPBloom_Blur;
	float PPBloom_padding;
};

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;

	const float4 resource1 = screenTexture.Sample(defaultSampler, input.UV);
	if (resource1.a < 0.05f) discard;
	const float3 resource2 = blurredLuminanceTexture.Sample(defaultSampler, input.UV).rgb;

	result.Color.rgb = resource1 + (resource2 * PPBloom_Strength);
	result.Color.a = 1;

	return result;
}
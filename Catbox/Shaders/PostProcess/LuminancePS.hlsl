#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"

Texture2D screenTexture : register(t0);

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

	const float4 color = screenTexture.Sample(defaultSampler, input.UV);

	if (color.a < 0.05f) discard;

	const float3 resource = color.rgb;

	float luminance = dot(resource, float3(0.2126f, 0.7152f, 0.0722f));
	luminance = saturate(luminance - PPBloom_Cutoff);
	result.Color.rgb = resource * luminance * (1.0f / PPBloom_Cutoff);
	result.Color.a = 1;

	return result;
}
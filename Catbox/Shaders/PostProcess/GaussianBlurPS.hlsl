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

	float4 color = screenTexture.Sample(defaultSampler, input.UV);
	if (color.a < 0.05f) discard;
	
	const float pi2 = 6.28318530718f;
	const float directions = 16.f;
	const float quality = 4.f;
	const float radius = 2.f;
	const float2 rad = radius / FB_Resolution * PPBloom_Blur;

	for (float d = 0.f; d < pi2; d += pi2 / directions) 
	{
		for (float i = 1.f / quality; i <= 1.f; i += 1.f / quality)
		{
			color += screenTexture.Sample(defaultSampler, input.UV + float2(cos(d), sin(d)) * rad * i);
		}
	}

	color /= quality * directions - 15.f;
	result.Color.rgb = color.rgb;
	result.Color.a = 1;

	return result;
}
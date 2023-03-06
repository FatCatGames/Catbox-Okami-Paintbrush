#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"
//#define VignetteRatio   1.00  //[0.15 to 6.00]  Sets a width to height ratio. 1.00 (1/1) is perfectly round, while 1.60 (16/10) is 60 % wider than it's high.
//#define VignetteRadius  1.00  //[-1.00 to 3.00] lower values = stronger radial effect from center
//#define VignetteAmount -1.00  //[-2.00 to 1.00] Strength of black. -2.00 = Max Black, 1.00 = Max White.
//#define VignetteSlope      8  //[2 to 16] How far away from the center the change should start to really grow strong (odd numbers cause a larger fps drop than even numbers)
//#define VignetteCenter float2(0.500, 0.500)  //[0.000 to 1.000, 0.000 to 1.000] Center of effect for VignetteType 1. 2 and 3 do not obey this setting.

Texture2D screenTexture : register(t0);
Texture2D depthTexture : register(t1);
Texture2D lightMap : register(t2);
Texture2D worldPositionTexture : register(t4);

cbuffer FogData : register(b10)
{
	float4 FogColor;
	float FogDensity;
	float FogCutoff;

	float yDepthDensity = 1;
	float yDepthCutoff = -2;
	float yDepthSmoothing = -5;
	float3 padding;
	float4 yDepthColor;
};

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;
	float3 col = screenTexture.Sample(defaultSampler, input.UV);

	float depth = depthTexture.Sample(defaultSampler, input.UV).r;
	depth = saturate(((depth - 0.98f) * 50.f) - FogCutoff);
	depth = saturate((depth * FogDensity) * (depth * FogDensity));
	float3 fogCol = ((col * (1 - depth)) + (FogColor * depth));

	float yDepth = -(worldPositionTexture.Sample(defaultSampler, input.UV).y - yDepthCutoff) / yDepthSmoothing;
	yDepth = saturate(yDepth);
	yDepth = saturate((yDepth * yDepthDensity) * (yDepth * yDepthDensity));
	fogCol = ((fogCol * (1 - yDepth)) + (yDepthColor * yDepth));

	result.Color.a = 1;
	float3 lightResult = lightMap.Sample(defaultSampler, input.UV).rgb;
	float lightAverage = (lightResult.r + lightResult.b + lightResult.g) / 3.f;
	result.Color.rgb = saturate(lerp(fogCol, col, lightAverage));
	//result.Color.rgb = lightResult;
	return result;
}
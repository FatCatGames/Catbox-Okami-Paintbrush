#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"
#include "../ShaderFunctions.hlsli"

//#define VignetteRatio   1.00  //[0.15 to 6.00]  Sets a width to height ratio. 1.00 (1/1) is perfectly round, while 1.60 (16/10) is 60 % wider than it's high.
//#define VignetteRadius  1.00  //[-1.00 to 3.00] lower values = stronger radial effect from center
//#define VignetteAmount -1.00  //[-2.00 to 1.00] Strength of black. -2.00 = Max Black, 1.00 = Max White.
//#define VignetteSlope      8  //[2 to 16] How far away from the center the change should start to really grow strong (odd numbers cause a larger fps drop than even numbers)
//#define VignetteCenter float2(0.500, 0.500)  //[0.000 to 1.000, 0.000 to 1.000] Center of effect for VignetteType 1. 2 and 3 do not obey this setting.

Texture2D screenTexture : register(t0);


cbuffer PostProcessingVignetteData : register(b10)
{
	float VignetteAmount;
	float VignetteRatio;
	float VignetteRadius;
	int VignetteSlope;
	float2 VignetteCenter;
	float2 VignettePadding;
};

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;
    float2 tex = input.UV;
    const float4 pixel = screenTexture.Sample(defaultSampler, tex);

	//Set the center
    float2 tc = tex - VignetteCenter;

	//Adjust the ratio
    tc *= float2((FB_Resolution.y / FB_Resolution.x), VignetteRatio);
   
	////Calculate the distance
    tc /= VignetteRadius;
    float v = dot(tc, tc);

	//Apply the vignette
    result.Color.rgb = saturate((1.0 + pow(v, VignetteSlope * 0.5) * VignetteAmount)) * pixel.rgb; //pow - multiply;
    result.Color.a = 1;
	return result;
}
#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"

Texture2D screenTexture : register(t0);

cbuffer PostProcessingColorAdjustmentData : register(b10)
{
	float4 PP_Tint;
	float PP_Brightness;
	float PP_Contrast;
	float PP_Saturation;
	float PP_padding;
};

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;
	const float4 pixel = screenTexture.Sample(defaultSampler, input.UV);
	if (pixel.a < 0.05f) discard;
	float4 newColor = pixel;

	//contrast
	newColor.rgb = (newColor.rgb - 0.5) * (PP_Contrast) + 0.5;


	newColor += (PP_Brightness - 1);
	newColor *= PP_Tint;

	//saturation
	const float3 lumCoeff = float3(0.2125, 0.7154, 0.0721);
	float intensity = float(dot(newColor.rgb, lumCoeff));
	newColor.rgb = lerp(intensity, newColor.rgb, PP_Saturation);

	result.Color = newColor;
	result.Color.a = 1;
	return result;
}
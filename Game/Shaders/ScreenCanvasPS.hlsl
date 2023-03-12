#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../Catbox/Shaders/Struct/PostProcessingShaderStructs.hlsli"

Texture2D screenTexture: register(t0);
Texture2D paintingTexture: register(t1);
Texture2D paperTexture: register(t2);


PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;
	
	float3 screenCol = screenTexture.Sample(defaultSampler, input.UV).rgb;
	float screenAvg = (screenCol.r + screenCol.g + screenCol.b) / 3.0f;
	
	float3 paperCol = paperTexture.Sample(defaultSampler, input.UV * 0.5).rgb;
	float paperAvg = (paperCol.r + paperCol.g + paperCol.b) / 3.0f;

	
	float3 col = screenAvg;
	col.r *= 1.3f;
	col.g *= 1.1f;
	col *= paperAvg;

	result.Color.rgb = col;
	result.Color.a = 1;

	return result;
}
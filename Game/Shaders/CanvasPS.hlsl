#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"


Texture2D paintingTexture: register(t0);
Texture2D screenTexture: register(t1);
Texture2D paperTexture: register(t2);

PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	float3 screenCol = screenTexture.Sample(defaultSampler, input.AlbedoUV).rgb;
	float screenAvg = (screenCol.r + screenCol.g + screenCol.b) / 3.0f;

	float3 paperCol = paperTexture.Sample(defaultSampler, input.AlbedoUV * 0.5).rgb;
	float paperAvg = (paperCol.r + paperCol.g + paperCol.b) / 3.0f;

	float3 col = screenAvg;
	col.r *= 1.3f;
	col.g *= 1.1f;

	float painting = paintingTexture.Sample(defaultSampler, input.AlbedoUV).r;
	col = col * paperAvg * painting;

	result.Color.rgb = col;
	result.Color.a = 1;

	//result.Color.rgb = painting;


	result.id = OB_id;
	return result;
}
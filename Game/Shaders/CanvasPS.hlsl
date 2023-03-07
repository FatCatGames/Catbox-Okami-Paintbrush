#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"


Texture2D screenTexture: register(t0);
Texture2D paintingTexture: register(t1);

PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	//result.Color.rgba = screenTexture.Sample(defaultSampler, input.AlbedoUV) * paintingTexture.Sample(defaultSampler, input.AlbedoUV);
	result.Color.rgb = paintingTexture.Sample(defaultSampler, input.AlbedoUV).rgb;
	result.Color.a = 1;
	result.id = OB_id;
	return result;
}
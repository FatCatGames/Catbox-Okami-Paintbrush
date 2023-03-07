#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"


Texture2D screenTexture;
Texture2D paintingTexture;

PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	result.Color.rgba = screenTexture.Sample(defaultSampler, input.AlbedoUV) * paintingTexture.Sample(defaultSampler, input.AlbedoUV);
	result.id = OB_id;
	return result;
}
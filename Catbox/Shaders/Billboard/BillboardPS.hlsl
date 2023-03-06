#include "../Struct/ShaderStructs.hlsli"

Texture2D albedoTexture : register(t0);

PixelOutput main(SimpleGeometryInput input)
{
	PixelOutput result;
	//float4 textureColor = albedoTexture.Sample(defaultSampler, input.UV);
	//if (textureColor.a < 0.05f) discard;
	//result.id = OB_id;
	//result.Color.rgba = textureColor.rgba;
	result.Color = float4(0.05f, 0, 0.2f, 1);
	return result;
}
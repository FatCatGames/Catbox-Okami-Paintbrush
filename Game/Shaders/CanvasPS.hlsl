#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"



PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	result.Color.rgba = float4(1,0,0,1);
	result.id = OB_id;
	return result;
}
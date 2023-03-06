#include "Struct/ShaderStructs.hlsli"

struct SimpleVertexToPixel
{
	float4 ProjectedPosition    : SV_POSITION;
	float4 WorldPosition		: POSITION;
};

PixelOutput main(SimpleVertexToPixel input)
{
	PixelOutput result;
	result.Color.rgba = MB_Color;
	return result;
}
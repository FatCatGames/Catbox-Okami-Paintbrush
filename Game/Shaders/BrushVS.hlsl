#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"

cbuffer BrushData : register(b10)
{
	float3 paintDir;
	float strength = 0;
}

VertexToPixel main(VertexInput input)
{
	VertexToPixel result;


	float4 vertexWorldPosition = mul(OB_ToWorld, input.Position);
	const float4 vertexViewPosition = mul(FB_ToView, vertexWorldPosition);
	const float4 vertexProjectionPosition = mul(FB_ToProjection, vertexViewPosition);

	const float3x3 worldNormalRotation = (float3x3)OB_ToWorld;

	result.ProjectedPosition = vertexProjectionPosition;
	result.WorldPosition = vertexWorldPosition;

	float influence = 1 - input.VxColor.b;
	//result.WorldPosition.xyz += 100;
	vertexWorldPosition.xyz += paintDir * paintDir * influence * 100;

	result.VxColor = input.VxColor;
	result.AlbedoUV = input.AlbedoUV;


	result.Tangent = mul(worldNormalRotation, input.Tangent);
	result.Binormal = mul(worldNormalRotation, input.Binormal);
	result.Normal = normalize(mul(worldNormalRotation, input.Normal)); 

	return result;
}
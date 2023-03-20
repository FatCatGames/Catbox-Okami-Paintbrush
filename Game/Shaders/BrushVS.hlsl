#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"

cbuffer BrushData : register(b10)
{
	float3 paintDir;
	float strength = 0;
}


VertexToPixel main(VertexInput input)
{
	VertexToPixel result;


	float influence = 0.1f * pow(input.VxColor.r, 1);
	float4 vertexWorldPosition = mul(OB_ToWorld, input.Position);
	//float length = clamp(influence * strength * 10, 0, myMaxLength);
	//float length = clamp(influence, 0, 0.1f);
	vertexWorldPosition.xz += paintDir.xz * influence;
	vertexWorldPosition.y += 0.045f * influence * input.VxColor.r;
	//vertexWorldPosition.xyz += paintDir * influence * strength * 10;

	const float4 vertexViewPosition = mul(FB_ToView, vertexWorldPosition);
	const float4 vertexProjectionPosition = mul(FB_ToProjection, vertexViewPosition);

	const float3x3 worldNormalRotation = (float3x3)OB_ToWorld;

	result.ProjectedPosition = vertexProjectionPosition;
	result.WorldPosition = vertexWorldPosition;

	result.VxColor = input.VxColor;
	result.AlbedoUV = input.AlbedoUV;
	result.Tangent = mul(worldNormalRotation, input.Tangent);
	result.Binormal = mul(worldNormalRotation, input.Binormal);
	result.Normal = normalize(mul(worldNormalRotation, input.Normal)); 


	return result;
}
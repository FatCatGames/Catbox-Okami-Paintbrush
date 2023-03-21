#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"

cbuffer BrushData : register(b10)
{
	float3 paintDir;
	float strength = 0;
}


VertexToPixel main(VertexInput input)
{
	VertexToPixel result;


	float brush = pow(input.VxColor.r, 0.3f);
	float brushTip = pow(input.VxColor.r, 2);

	//Moves brush up/down when painting
	input.Position.y += 10 * strength;
	
	//Accidental squash/stretch!
	input.Position.y *= 1 + strength;
	input.Position.y *= 1 - strength * 0.5f;

	//Squash the brush
	input.Position.y -= (strength * brush) * 5;
	input.Position.y -= (strength * pow(brushTip,2)) * 3;
	input.Position.xz *= 1 + strength * brush * 0.3f;


	float4 vertexWorldPosition = mul(OB_ToWorld, input.Position);
	
	float brushOffset = pow((input.VxColor.r), 0.7f);
	vertexWorldPosition.xz += 0.15f * strength * paintDir.xz * brushTip;

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
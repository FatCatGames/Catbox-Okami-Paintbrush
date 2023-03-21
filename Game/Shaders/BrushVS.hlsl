#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"

cbuffer BrushData : register(b10)
{
	float3 paintDir;
	float strength = 0;
}


VertexToPixel main(VertexInput input)
{
	VertexToPixel result;


	float influence = pow(input.VxColor.r, 0.3f);

	//Moves brush up/down when painting
	input.Position.y += 5 * strength;
	
	//Accidental squash/stretch!
	//input.Position.y *= 1 + strength;
	//input.Position.y *= 1 - strength * 0.5f;

	//Squash the brush
	input.Position.y -= (strength * influence) * 5;
	input.Position.y -= (strength * pow(influence,2));
	//input.Position.y -= (strength * pow(input.VxColor.r, 10)) * 1.f;
	input.Position.xz *= 1 + strength * influence * 0.25f;
	input.Position.xz *= 1 + strength * pow(influence, 2) * 0.25f;


	float4 vertexWorldPosition = mul(OB_ToWorld, input.Position);
	
	float brushOffset = pow((input.VxColor.r), 0.7f);
	vertexWorldPosition.xz += 0.07f * strength * paintDir.xz * brushOffset;

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
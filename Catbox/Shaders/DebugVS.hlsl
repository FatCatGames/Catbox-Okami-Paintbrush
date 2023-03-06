#include "Struct/ShaderStructs.hlsli"

struct SimpleVertexToPixel
{
	float4 ProjectedPosition    : SV_POSITION;
	float4 WorldPosition		: POSITION;
};


SimpleVertexToPixel main(SimpleVertexInput input)
{
	SimpleVertexToPixel result;


	const float4 vertexWorldPosition = mul(OB_ToWorld, input.Position);
	const float4 vertexViewPosition = mul(FB_ToView, vertexWorldPosition);
	const float4 vertexProjectionPosition = mul(FB_ToProjection, vertexViewPosition);

	const float3x3 worldNormalRotation = (float3x3)OB_ToWorld;

	result.ProjectedPosition = vertexProjectionPosition;
	result.WorldPosition = vertexWorldPosition;

	return result;
}
#include "Struct/ShaderStructs.hlsli"

VertexToPixel main(VertexInput input)
{
	VertexToPixel result;

	float4x4 skinningMatrix = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 };

	if (!OB_isStatic)
	{
		skinningMatrix = 0;
		skinningMatrix += input.BoneWeights.x * SB_BoneData[input.BoneIDs.x];
		skinningMatrix += input.BoneWeights.y * SB_BoneData[input.BoneIDs.y];
		skinningMatrix += input.BoneWeights.z * SB_BoneData[input.BoneIDs.z];
		skinningMatrix += input.BoneWeights.w * SB_BoneData[input.BoneIDs.w];
	}

	float4 vertexWorldPosition = mul(OB_ToWorld, mul(input.Position, skinningMatrix));

	if (OB_isInstanced) 
	{
		vertexWorldPosition = mul(input.World, mul(input.Position, skinningMatrix));
	}

	const float4 vertexViewPosition = mul(FB_ToView, vertexWorldPosition);
	const float4 vertexProjectionPosition = mul(FB_ToProjection, vertexViewPosition);

	const float3x3 worldNormalRotation = (float3x3)OB_ToWorld;
	const float3x3 skinNormalRotation = (float3x3)skinningMatrix;

	result.ProjectedPosition = vertexProjectionPosition;
	result.WorldPosition = vertexWorldPosition;

	result.VxColor	 = input.VxColor;
	result.NormalTex	 = input.NormalTex;
	result.Roughness   = input.Roughness;
	result.AO			 = input.AO;
	result.AlbedoUV	 = input.AlbedoUV;
	result.NormalUV	 = input.NormalUV;
	result.RoughnessUV = input.RoughnessUV;
	result.AOUV		 = input.AOUV;

	result.Tangent = mul(worldNormalRotation, mul(input.Tangent, skinNormalRotation));
	result.Binormal = mul(worldNormalRotation, mul(input.Binormal, skinNormalRotation));
	result.Normal = normalize(mul(worldNormalRotation, mul(input.Normal, skinNormalRotation)));

	return result;
}
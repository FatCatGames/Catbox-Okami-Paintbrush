#include "../Struct/ShaderStructs.hlsli"


SimpleVertexInput main(SimpleVertexInput input)
{
	SimpleVertexInput result = input;

	const float4 particleWorldPosition = mul(OB_ToWorld, input.Position);
	result.Position = particleWorldPosition;

	return result;
}
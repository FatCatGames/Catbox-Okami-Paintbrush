#include "../Struct/ShaderStructs.hlsli"

[maxvertexcount(4)]
void main(
	point SimpleVertexInput input[1],
	inout TriangleStream<SimpleGeometryInput> output
)
{
	const float2 offsets[4] =
	{
		{ -1,  1 },
		{  1,  1 },
		{ -1, -1 },
		{  1, -1 }
	};

	const float2 uvs[4] =
	{
		{  0.f,  0.f },
		{  1.f,  0.f },
		{  0.f,  1.f },
		{  1.f,  1.f }
	};

	const SimpleVertexInput inputVertex = input[0];
	for (unsigned int index = 0; index < 4; ++index)
	{
		SimpleGeometryInput result;
		result.Position = mul(FB_ToView, inputVertex.Position);
		result.Position.xy += offsets[index] * OB_Scale.xy * 0.1f;
		result.Position = mul(FB_ToProjection, result.Position);
		result.UV = uvs[index];
		output.Append(result);
	}
}
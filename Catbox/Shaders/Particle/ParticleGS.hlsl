#include "../Struct/ShaderStructs.hlsli"
#include "ParticleShaderStructs.hlsli"
#include "../ShaderFunctions.hlsli"

[maxvertexcount(4)]
void main(
	point ParticleVertexData input[1],
	inout TriangleStream<ParticleGeometryToPixel> output
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

	const ParticleVertexData inputParticle = input[0];
	for (unsigned int index = 0; index < 4; ++index)
	{
		ParticleGeometryToPixel result;
		result.Position = mul(FB_ToView, inputParticle.Position);
		result.Position.xy += offsets[index] * inputParticle.Scale.xy;
		result.Position = mul(FB_ToProjection, result.Position);
		result.Color = inputParticle.Color;
		result.Lifetime = inputParticle.Lifetime;
		result.UV = RotateUV(uvs[index], inputParticle.Rotation);
		result.ParticleID = inputParticle.ParticleID;
		output.Append(result);
	}
}
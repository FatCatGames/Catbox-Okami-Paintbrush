#include "../Struct/ShaderStructs.hlsli"
#include "UIShaderStructs.hlsli"


UISpriteVertexToPixel main(SpriteVertexInput input)
{
	UISpriteVertexToPixel result;

	const float2 uv[4] =
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};

	result.position = input.position;
	result.position.xy *= UI_size;
	result.position.xy += UI_offset;
	result.uv = uv[input.Index];

	return result;
}

struct SpriteVertexInput
{
	float4 position : POSITION;
	unsigned int Index : SV_VertexID;
};

struct UISpriteVertexToPixel
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

cbuffer UITransformBuffer : register(b11)
{
	float2 UI_offset;
	float2 UI_size;
	float4 UI_color;
	uint UI_id;
	float3 UI_padding;
}
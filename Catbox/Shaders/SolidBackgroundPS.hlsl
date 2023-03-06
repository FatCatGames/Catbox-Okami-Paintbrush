SamplerState defaultSampler : register(s0);

struct DeferredVertexToPixel
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

Texture2D sourceTexture : register(t0);

cbuffer ColorBuffer : register(b0)
{
	float4 BG_Color : COLOR;
}

struct BGPixelOutput
{
	float4 Color : SV_TARGET;
};

BGPixelOutput main(DeferredVertexToPixel input)
{
	BGPixelOutput result;
	result.Color = sourceTexture.Sample(defaultSampler, input.UV);
	if (result.Color.a < 0.05f)
	{
		result.Color.rgba = BG_Color;
	}
	return result;
}
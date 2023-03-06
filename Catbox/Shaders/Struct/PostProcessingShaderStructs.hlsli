struct PostProcessPixelOutput
{
	float4 Color : SV_TARGET0;
};

struct PostProcessVertexToPixel
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};


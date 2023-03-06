
struct ParticleVertexData
{
	float4 Position : POSITION;
	float4 Color : COLOR;
	float3 Scale: SCALE;
	float Lifetime : LIFETIME;
	int ParticleID : ID;
	float Rotation : ROTATION;
};

struct ParticleGeometryToPixel
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 UV: TEXCOORD;
	float Lifetime : LIFETIME;
	int ParticleID : ID;
};

struct ParticlePixelOutput
{
	float4 Color : SV_TARGET0;
	uint id : SV_TARGET1;
};
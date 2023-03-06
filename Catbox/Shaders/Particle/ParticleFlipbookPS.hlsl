#include "ParticleShaderStructs.hlsli"
#include "../Struct/ShaderStructs.hlsli"
#include "../ShaderFunctions.hlsli"

//#define Rows 8
//#define Columns 8
//#define Framerate 24.0
//#define StartDelay 5.0
//#define StopDelay 500.0
//#define Loop true

cbuffer FlipbookBuffer : register(b10)
{
    int Rows = 8;
    int Columns = 8;
    int Framerate = 24;
    bool Loop = true;
}

Texture2D albedoTexture : register(t0);

ParticlePixelOutput main(ParticleGeometryToPixel input)
{
    ParticlePixelOutput result;

    float2 activeFrame = FlipBook(input.UV, Rows, Columns, Framerate, input.Lifetime, Loop);
    float4 textureColor = albedoTexture.Sample(defaultSampler, activeFrame);

    if (textureColor.a < 0.05f)  discard;
	result.id = OB_id;
    result.Color.rgba = textureColor.rgba * input.Color.rgba;
    return result;
}
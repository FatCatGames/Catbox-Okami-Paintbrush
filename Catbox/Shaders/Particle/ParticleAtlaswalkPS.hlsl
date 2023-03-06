#include "ParticleShaderStructs.hlsli"
#include "../Struct/ShaderStructs.hlsli"
#include "../ShaderFunctions.hlsli"

cbuffer AtlaswalkBuffer : register(b10)
{
    int Rows        = 8;
    int Columns     = 8;
    int Sprite      = 1;  //Sets start of range in sheet
    int NoSprites   = 18; //Sets end of range in sheet
}

Texture2D albedoTexture : register(t0);

ParticlePixelOutput main(ParticleGeometryToPixel input)
{
    ParticlePixelOutput result;
    
    float startOfRange = min(NoSprites, Sprite) - 1; //Start of range can never be past numbers of sprites   
    float ranSprite = floor(Remap(0, 1, startOfRange, NoSprites, frac(Hash23(float2(input.ParticleID, input.ParticleID + input.ParticleID)).x)));
    float2 activeFrame = FlipBook(input.UV, Rows, Columns, 1, ranSprite, true);
    float4 textureColor = albedoTexture.Sample(defaultSampler, activeFrame);

    if (textureColor.a < 0.05f)
        discard;
    result.id = OB_id;
    result.Color.rgba = textureColor.rgba * input.Color.rgba;
    return result;
}
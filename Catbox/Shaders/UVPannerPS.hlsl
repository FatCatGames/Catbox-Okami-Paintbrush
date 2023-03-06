#include "Struct/ShaderStructs.hlsli"
#include "ShaderFunctions.hlsli"

cbuffer UVPannerBuffer : register(b10)
{
    float xSpeed = 0;
    float ySpeed = 0;
    bool isFrac = false;
    bool isAccelerate = false;
}

Texture2D albedoTexture : register(t0);


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;

    float usedTime = OB_AliveTime;
    float2 speed = float2(xSpeed, ySpeed);
    
    if (isAccelerate)
    {
        usedTime = OB_AliveTime * OB_AliveTime;
    }
    if (isFrac)
    {
        usedTime = frac(usedTime);
    }
    speed.x *= 0.1;
    speed.y *= 0.1;
    
    float4 albedo = albedoTexture.Sample(wrapSampler, float2(input.AlbedoUV.x + speed.x * usedTime, input.AlbedoUV.y + speed.y * usedTime));
    result.Color = albedo;
    return result;
}
#include "Struct/ShaderStructs.hlsli"
#include "ShaderFunctions.hlsli"
//#include "Functions/Ease.hlsli"

Texture2D albedoTexture : register(t0);

//ProjectedPosition = localspace in here
float RandNumber(int pos) //Grundfunktionen f?r randomisering
{
    
    unsigned int seed = 2147483647;
    
    unsigned int BIT_NOISE1 = 0xB5297A4D;
    unsigned int BIT_NOISE2 = 0x68E31DA4;
    unsigned int BIT_NOISE3 = 0x1B56C4E9;
    
    unsigned int mangled = (unsigned int) pos;
    mangled *= BIT_NOISE1;
    mangled += seed;
    mangled ^= (mangled >> 8);
    mangled += BIT_NOISE2;
    mangled ^= (mangled << 8);
    mangled *= BIT_NOISE3;
    mangled ^= (mangled >> 8);
    return (float) mangled / 0xffffffff;
	//STOLEN FROM SQUIRREL EISERLOH
}

PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    
    float calcTime = FB_TotalTime * MB_CustomValue1;
    float noiseTime = calcTime + SimplexNoise(float2(calcTime, calcTime));
    float noiseFun = SimplexNoise(input.AlbedoUV * calcTime);
    float heightNoise = input.AlbedoUV.y + input.AlbedoUV.x * SimplexNoise(input.Normal.xy) *
    max(min(BellCurve(frac(FB_TotalTime * 0.1f)), 0.2f), 0.5f) * MB_CustomValue2;
    
    //float lineA = frac((input.AlbedoUV.y + noiseFun * 0.02f) * MB_CustomValue1) * MB_CustomValue1;
    
    float lineA = frac(heightNoise * MB_CustomValue1) * MB_CustomValue1;
    float lineC = frac((heightNoise * MB_CustomValue1) * 0.5f) * MB_CustomValue1;
    float lineB = lineA + noiseFun * MB_CustomValue3;
    float lineD = lineC + noiseFun * MB_CustomValue3;

    if (lineB < noiseTime % MB_CustomValue1 * 1.1f && lineB > noiseTime % MB_CustomValue1)
    {
        discard;
    }
    if (lineD < noiseTime % MB_CustomValue1 * 1.1f && lineD > noiseTime % MB_CustomValue1)
    {
        discard;
    }

    
        float4 albedo = AlbedoTex.Sample(wrapSampler, float2(input.AlbedoUV.x + noiseFun * 0.02f, input.AlbedoUV.y));
    albedo *= float4(0.2f, 0.4, 0.8f, 1);
    result.Color = albedo;
    float randPixel = RandNumber(input.WorldPosition.x * input.WorldPosition.y * calcTime);
    if (randPixel < 0.35f)
    {
        result.Color = float4(randPixel, randPixel, randPixel, 0);
    }
    
    //result.Color = float4(1, 1, 1, 1);
    return result;
}
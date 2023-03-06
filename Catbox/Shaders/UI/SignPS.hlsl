#include "../Struct/ShaderStructs.hlsli"
#include "../PBRFunctions.hlsli"

Texture2D albedoTexture : register(t0);

float x2(float x)
{
    return x * x;
}

PixelOutput main(VertexToPixel input)
{
    
    PixelOutput result;
    float2 tex = input.AlbedoUV.xy* 0.9;
    float signTexts = albedoTexture.Sample(wrapSampler, tex).r;
    

    //textMask = lerp(textMask, signTexts.b, MB_Color.g);
    
    if (signTexts < 0.01)
        discard;
    result.Color.rgb = float3(signTexts, signTexts * 0.2, signTexts * 0.3);
    //result.Color.a = 1-signTexts;
    return result;
}

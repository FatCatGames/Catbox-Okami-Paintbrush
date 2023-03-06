#include "../ShaderFunctions.hlsli"
#include "../Struct/ShaderStructs.hlsli"

Texture2D albedoTexture : register(t0);


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    
    float2 uv = TransformUV(input.AlbedoUV, 0.5, sin(FB_TotalTime), sin(FB_TotalTime), sin(FB_TotalTime));
    
    float4 textureColor = albedoTexture.Sample(defaultSampler, uv);
    //if (textureColor.a < 0.05f) discard;



    result.Color = textureColor;

    //result.Color.rgb = Hash23(input.AlbedoUV);   
    //result.Color.a = 1;   
    //result.id = 0;
    return result;
}
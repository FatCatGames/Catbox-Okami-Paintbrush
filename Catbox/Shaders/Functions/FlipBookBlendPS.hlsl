#include "../Struct/ShaderStructs.hlsli"
#include "../ShaderFunctions.hlsli"

cbuffer FlipbookBuffer : register(b10)
{
    int Rows = 8;
    int Columns = 8;
    int Framerate = 24;
    bool Loop = true;
}

Texture2D albedoTexture : register(t0);


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;

    float2 tex = input.AlbedoUV;

    FlipBookBlendData d = FlipBookUVBlend(tex, Rows, Columns, Framerate, FB_TotalTime);
    
    float4 currColor = albedoTexture.Sample(defaultSampler, d.CurrentImageTexCoord);
    float4 nextColor = albedoTexture.Sample(defaultSampler, d.NextImageTexCoord);
    float4 textureColor = lerp(currColor, nextColor, d.Ratio);

    if (textureColor.a < 0.05f)
        discard;
    result.Color = textureColor;
    result.Color.a *= textureColor.a;
    //result.id = 0;
    return result;
}
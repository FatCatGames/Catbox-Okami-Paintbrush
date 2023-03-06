#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"

Texture2D screenTexture : register(t0);
Texture2D outlineMask : register(t1);

cbuffer PostProcessingOutlineData : register(b10)
{
    float4 OutlineColor;
}

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
    PostProcessPixelOutput result;
    float2 tex = input.UV;
    float4 pos = input.Position;
    float2 dim;
    uint nolev;

    float4 orig = screenTexture.Sample(defaultSampler, tex);
    screenTexture.GetDimensions(0, dim.x, dim.y, nolev);

    float3x3 sobelx = float3x3(
                                 1.0,  2.0,  1.0,
                                 0.0,  0.0,  0.0,
                                -1.0, -2.0, -1.0
                               );
    float3x3 sobely = float3x3(
                                 1.0,  0.0,  -1.0,
                                 2.0,  0.0,  -2.0,
                                 1.0,  0.0,  -1.0
                               );
    float3x3 magnitudes; // = mat3(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float2 coords = float2(tex.x + (float(i) - 1.0) / dim.x, tex.y + (float(j) - 1.0) / dim.y);
            magnitudes[i][j] = length(outlineMask.Sample(defaultSampler, coords).r);
        }
    }
    
    float x = dot(sobelx[0], magnitudes[0]) + dot(sobelx[1], magnitudes[1]) + dot(sobelx[2], magnitudes[2]);
    float y = dot(sobely[0], magnitudes[0]) + dot(sobely[1], magnitudes[1]) + dot(sobely[2], magnitudes[2]);
    
    float final = pow(sqrt(x * x + y * y) * 0.5, 2.);

    result.Color = orig + OutlineColor * final;
    return result;
}
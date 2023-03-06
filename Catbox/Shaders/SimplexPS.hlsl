#include "Struct/ShaderStructs.hlsli"
#include "Noise/Noise.hlsli"

Texture2D screenTexture : register(t0);

PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    float2 dim;
    uint nolev;

    screenTexture.GetDimensions(0, dim.x, dim.y, nolev);
    float2 p = input.AlbedoUV.xy/* / dim.xy*/;

    float2 uv = p + FB_TotalTime * 0.25;; /** float2(dim.x / dim.y, 1.0) + FB_TotalTime * 0.25;*/
	
    float f = 0.0;
	
    // left: value SimplexNoise	
    if (p.x < 0.6)
    {
        f = SimplexNoise(16.0 * uv);
    }
    // right: fractal SimplexNoise (4 octaves)
    else
    {
        uv *= 5.0;
        float2 m = float2(1.6, 1.2);
        float2 mm = float2(-1.2, 1.6);
        f = 0.5000 * SimplexNoise(uv);
        uv = m * uv * mm;
        f += 0.2500 * SimplexNoise(uv);
        uv = m * uv * mm;
        f += 0.1250 * SimplexNoise(uv);
        uv = m * uv * mm;
        f += 0.0625 * SimplexNoise(uv);
        uv = m * uv * mm;
    }

    f = 0.5 + 0.5 * f;
	
    f *= smoothstep(0.0, 0.005, abs(p.x - 0.6));
	
    result.Color = float4(f, f, f, 1.0);


    return result;
}
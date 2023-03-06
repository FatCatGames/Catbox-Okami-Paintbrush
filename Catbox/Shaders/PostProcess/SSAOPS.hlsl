#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"

float4 GetViewPosition(float2 uv)
{
    const float4 worldPosition = float4(WorldPosTex.Sample(defaultSampler, uv).rgb, 1);
    const float4 viewPosition = mul(FB_ToView, worldPosition);
    return viewPosition;
}

float4 GetViewNormal(float2 uv)
{
    const float4 worldNormal = float4(NormalTex.Sample(defaultSampler, uv).rgb, 0);
    const float4 viewNormal = mul(FB_ToView, worldNormal);
    return viewNormal;
}

float2 GetRandom(float2 uv, float2 uvScale)
{
    const float3 random = 2.0f * NoiseTex.Sample(pointWrapSampler, uv * uvScale).rgb - 1.0f;
    return random.xy;
}

float SSAO(float2 screenUV, float2 uv, float3 pxViewPos, float3 pxViewNrm, float bias, float intensity)
{
	const float3 diff = GetViewNormal(screenUV + uv).xyz - pxViewPos;
	const float3 v = normalize(diff);
	const float occ = max(0.0f, dot(pxViewNrm, v) - bias) * intensity;
	return occ;
}

cbuffer SSAOData : register(b10)
{
    float SSAO_intensity;
    float SSAO_bias;
    float SSAO_radius;
    float SSAO_padding;
}

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;

    const float offset = 0.707f;
    const float intensity = SSAO_intensity / 20.f;
    const float bias = SSAO_bias;
    const float radius = SSAO_radius / 5.f;
    
    int noiseWidth = 0;
    int noiseHeight = 0;
    int numMips = 0;
    
    NoiseTex.GetDimensions(0, noiseWidth, noiseHeight, numMips);
    const float2 randomUVScale = FB_Resolution / float2(noiseWidth, noiseHeight);
    const float2 random = GetRandom(input.UV, randomUVScale);
    
    const float4 pxPos = GetViewPosition(input.UV);
    const float4 pxNrm = GetViewNormal(input.UV);

    
    const float2 frustrumCorners[4] = { float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1) };
    
    float occ = 0.0f;
    
    for (uint i = 0; i < 4; i++)
    {
        const uint idx = i % 4;
        const float2 coord1 = reflect(frustrumCorners[idx], random) * radius;
        const float2 coord2 = float2(coord1.x * offset - coord1.y * offset, coord1.x * offset + coord1.y * offset);
        
        occ += SSAO(input.UV, coord1 * 0.25f, pxPos.xyz, pxNrm.xyz, 0, intensity);
        occ += SSAO(input.UV, coord2 * 0.5f, pxPos.xyz, pxNrm.xyz, 0, intensity);
        occ += SSAO(input.UV, coord1 * 0.75f, pxPos.xyz, pxNrm.xyz, 0, intensity);
        occ += SSAO(input.UV, coord2 * 0.25f, pxPos.xyz, pxNrm.xyz, 0, intensity);
    }
    
    occ = (occ - 0.5f) * (bias) + 0.5f;
    occ = saturate(occ);

    result.Color.r = occ;
    result.Color.a = 1;
    return result;
}
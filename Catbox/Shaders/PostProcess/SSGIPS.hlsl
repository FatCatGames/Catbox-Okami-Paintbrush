#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"
#include "../Noise/Noise.hlsli"
#include "../Functions/Ease.hlsli"
//#define VignetteRatio   1.00  //[0.15 to 6.00]  Sets a width to height ratio. 1.00 (1/1) is perfectly round, while 1.60 (16/10) is 60 % wider than it's high.
//#define VignetteRadius  1.00  //[-1.00 to 3.00] lower values = stronger radial effect from center
//#define VignetteAmount -1.00  //[-2.00 to 1.00] Strength of black. -2.00 = Max Black, 1.00 = Max White.
//#define VignetteSlope      8  //[2 to 16] How far away from the center the change should start to really grow strong (odd numbers cause a larger fps drop than even numbers)
//#define VignetteCenter float2(0.500, 0.500)  //[0.000 to 1.000, 0.000 to 1.000] Center of effect for VignetteType 1. 2 and 3 do not obey this setting.

Texture2D screenTexture : register(t0);
Texture2D depthTexture : register(t1);
Texture2D worldPositionTexture : register(t4);

#define MAX_STEPS 100
#define MAX_DIST 100.0
#define SURF_DIST 0.01

//float pulse = 30.2;
//float intensity = 6.2 * input.myVxColor;
//float speed = 0.8;
	
//float4 vertexWorldPosition;
//float4 localPos = input.myPosition;
//    localPos *= 0.001;
//float dist = length(float3(localPos.xyz));

//float pulseDist = dist * pulse;
	
//float motion = SmoothStart(pow(sin(Time * speed + pulseDist), 2));
//float3 posOffset = input.myNormal * motion;
//    posOffset.x *= 1.6;
//    posOffset *=
//intensity;

cbuffer FogData : register(b10)
{
    float4 FogColor;
    float FogDensity;
    float FogCutoff;

    float yDepthDensity = 1;
    float yDepthCutoff = -2;
    float yDepthSmoothing = -5;
    float3 padding;
    float4 yDepthColor;
};
float RandNumber(int pos) //Grundfunktionen fr randomisering
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

float3 GetPerpendicularVector(float3 normal)
{
    float3 biTangent;

    float3 c1 = cross(normal, float3(0.0, 0.0, 1.0));
    float3 c2 = cross(normal, float3(0.0, 1.0, 0.0));

    if (length(c1) > length(c2))
    {
        biTangent = c1;
    }
    else
    {
        biTangent = c2;
    }

    return biTangent;
}

// Get a cosine-weighted random vector centered around a specified normal direction.
float3 GetCosHemisphereSample(float rand1, float rand2, float3 hitNorm)
{
	// Get 2 random numbers to select our sample with
    float2 randVal = float2(rand1, rand2);

	// Cosine weighted hemisphere sample from RNG
    float3 bitangent = GetPerpendicularVector(hitNorm);
    float3 tangent = cross(bitangent, hitNorm);
    float r = sqrt(randVal.x);
    float phi = 2.0f * 3.14159265f * randVal.y;

	// Get our cosine-weighted hemisphere lobe sample direction
    return tangent * (r * cos(phi).x) + bitangent * (r * sin(phi)) + hitNorm.xyz * sqrt(max(0.0, 1.0f - randVal.x));
}
float GetDist(float3 p)
{
                         //from left, up, away, radius
    float rightLeft = (1 - sin(FB_TotalTime)) - 0.5;
    float forwardBackward = (1 - cos(FB_TotalTime)) + 7;

    float4 sphere = float4(rightLeft, 1.2, forwardBackward, 1);

    float sphereDist = length(p - sphere.xyz) - sphere.w;
    float planeDist = p.y;

    float d = min(sphereDist, planeDist);
    return d;
}



float4 RayMarch(float3 rayOrigin, float3 rayDirection, float depth)
{
    float step = 0;
    float stepSize = 0.1f;
    float3 p = rayOrigin;
    float endStep = 0;
    float newDepth = 0;
    for (int i = 0; i < MAX_STEPS; i++)
    {
        p = rayOrigin + (rayDirection * step);
        //p = normalize(mul(FB_ToView, float4(p, 1)).xyz);
        //p = normalize(mul(FB_ToView, float4(p, 1)).xyz);
        newDepth = depthTexture.Sample(defaultSampler, p.xy).r;
        if (depth < newDepth)
        {
            //step = i / step;
            endStep = step;
            break;
        }
        step += stepSize;
        
    }
    return float4(p.xyz, newDepth);
}
PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
    
    PostProcessPixelOutput result;
    float4 worldNormal = VertexNormalTex.Sample(defaultSampler, input.UV);
    float normLength = length(worldNormal);
    float4 worldPos = worldPositionTexture.Sample(defaultSampler, input.UV);
    //float noise = SimplexNoise(float2(input.UV.x * 100.f, input.UV.y * 100.f) * FB_TotalTime);
    float noise = RandNumber((worldPos.x * 100 * worldPos.y * 100 * worldPos.z) * frac(FB_TotalTime));
    float4 worldNorms = normalize(mul(FB_ToView, worldNormal));
    worldNorms = normalize(mul(FB_ToProjection, worldNormal));
    float3 stochasticNormals = GetCosHemisphereSample(noise, noise, float3(worldNormal.xyz));
    result.Color.rgba = float4(stochasticNormals, 1);
    
    
    
    float depth = depthTexture.Sample(defaultSampler, input.UV).r;
    
    float3 screenPos = float3(input.UV * 2 - 1, depth);
    
    //float3 viewPos = FB_CamPosition;
    
    float4 dir = mul(FB_ToView, float4(stochasticNormals, 1));
    dir = mul(FB_ToProjection, dir);
    
    //worldPos = normalize(mul(FB_ToView, worldPos));
    //worldPos = normalize(mul(FB_ToProjection, worldPos));
    
    
    
    float4 rayMatch = RayMarch(screenPos, dir.xyz, depth);
    
    //float stepSize = (1.0f / )
    //result.Color = float4(rayMatch.xyz, 1);
    
    float4 rayMarchCol = AlbedoTex.Sample(defaultSampler, rayMatch.xy);
    float4 normalCol = AlbedoTex.Sample(defaultSampler, stochasticNormals.xy);
    
    result.Color = lerp(normalCol, rayMarchCol, rayMatch.w);
    //result.Color = normalCol;
    return result;
}
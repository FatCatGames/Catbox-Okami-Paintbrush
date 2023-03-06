#include "Struct/ShaderStructs.hlsli"
#include "ShaderFunctions.hlsli"
#include "PBRFunctions.hlsli"
#include "UI/UIShaderStructs.hlsli"

//--- Parameters ---                   
//---0..1 Controll fill level of orb
//---Color of orb fluid

cbuffer ResourceInfoBuffer : register(b10)
{
    float Resource;
    float3 OrbColor;
};

Texture2D albedoTexture : register(t0);


float CircleFromPlane(float2 texCenterOffset)
{
	//Inverts texCenterOffset
    float circleLerp = lerp(1, 0, length(texCenterOffset));
    float circle = ceil(circleLerp - 0.5);
    return circle;
}

float HealthAmount(float2 texUV, float albedo)
{  
    float normalizedHealth = max(min(Resource, 1), 0);
    float healthAmount = smoothstep(0, 1, (texUV.y + (albedo * 0.15)) + (normalizedHealth - 1));
    return healthAmount;
}

PixelOutput main(UISpriteVertexToPixel input)
{
	PixelOutput result;
    
    //-- PlaceHodlers --
    float myTotalTime = FB_TotalTime;

    //-- Get UV coordinates --
    float2 tex = input.uv.xy;
    
    float2 texCenterOffset = tex - 0.5;
    float circleLerp = lerp(1, 0, saturate(length(texCenterOffset)));
    float circle = ceil(circleLerp - 0.5);
    float circleLearpInverted = pow(circleLerp * -1 + 1, 3) * 30;

//-- Change to Fast and Slow instead of Positiv and Negative
    float2 rotXYFast = RotateUV(tex, myTotalTime * 0.5);
    float2 rotXYSlow = RotateUV(tex, myTotalTime * 0.1);
    
    
    //-- Sample emissive -- not used right now
    //float emissive = shaderTextures[MATERIAL].Sample(SampleType, tex).b; 
 
    //-- Sample albedo in two rotating speeds
    float3 albedoPositive = GammaToLinear(albedoTexture.Sample(defaultSampler, rotXYFast + (circleLearpInverted * 0.18)).r);
    float3 albedoNegative = GammaToLinear(albedoTexture.Sample(defaultSampler, rotXYSlow + (circleLearpInverted * 0.12)).r);
    
    
    //-- IDK yet what this does :D
    float3 albedoPanningGreen = GammaToLinear(albedoTexture.Sample(defaultSampler, float2(tex.x + myTotalTime * -0.3, tex.y + myTotalTime * 0.5) + (circleLearpInverted * 0.05)).g);
    float3 albedoPanningBlue = GammaToLinear(albedoTexture.Sample(defaultSampler, float2(tex.x + myTotalTime * -0.3, tex.y + myTotalTime * 0.3) * 1.15).b);
    
    
    float3 albedoAlpha = GammaToLinear(albedoTexture.Sample(defaultSampler, float2(tex.x, tex.y)).a);
    float4 albedo = albedoTexture.Sample(defaultSampler, tex);
   
    albedo.xyz = (albedoPositive * albedoNegative) * 5 + (albedoPanningGreen * pow(circleLerp, 5) * 5 * (Resource * 2)) + (albedoPanningBlue * 0.3);
    
    float alphaGradient = HealthAmount(tex, albedo.r);
 
    float upperLimitMask = lerp(1, 0, min(floor(alphaGradient + 0.9995), 1));
    //float3 upperLimitMaskColor = upperLimitMask * float3(0.03, 0, 0);
    float3 upperLimitMaskColor = upperLimitMask * OrbColor * 0.03;
    float3 maskColor = (alphaGradient + circleLearpInverted * 0.15)/* * float3(0.5, 0.5, 0.1)*/;
    float alpha = ceil(alphaGradient);

    //float3 orbColor = OrbColor * maskColor + upperLimitMaskColor + (albedoAlpha * float3(0.5, 0, 0));
    float3 orbColor = OrbColor * maskColor + upperLimitMaskColor + (albedoAlpha * OrbColor * 0.5);

    result.Color = float4(LinearToGamma(orbColor), alpha * circle);
    
    if (result.Color.a <= 0.05f) { discard; }

	return result;
}
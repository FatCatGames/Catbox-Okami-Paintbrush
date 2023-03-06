#include "../Struct/ShaderStructs.hlsli"
#include "../Noise/Noise.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel result;

    float4x4 skinningMatrix =
    {
        1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
    };

    if (!OB_isStatic)
    {
        skinningMatrix = 0;
        skinningMatrix += input.BoneWeights.x * SB_BoneData[input.BoneIDs.x];
        skinningMatrix += input.BoneWeights.y * SB_BoneData[input.BoneIDs.y];
        skinningMatrix += input.BoneWeights.z * SB_BoneData[input.BoneIDs.z];
        skinningMatrix += input.BoneWeights.w * SB_BoneData[input.BoneIDs.w];
    }

    float3 windDomain = float3(OB_ToWorld._m11, OB_ToWorld._m22, OB_ToWorld._m33);
    
    ////////////////////////////////// Noise of two different scales are blended and animated per direction ////////////////////////////////////////////////////////////////////////////
    
    float noiseA = SimplexNoise(float2((input.Position.x * (windDomain.x * 0.05) * 0.1) * sin(FB_TotalTime * 0.022), FB_TotalTime)) * 0.5;
    float noiseB = SimplexNoise(float2((input.Position.x * (windDomain.x * 0.045) * 0.02) * cos(FB_TotalTime), FB_TotalTime * 0.07)) * 0.25;
    float learpNoise = SimplexNoise(float2(FB_TotalTime * 0.2, FB_TotalTime * 0.2));
    
    float noiseScaleX = windDomain.x * lerp(noiseA, noiseB, learpNoise);
   
    noiseA = SimplexNoise(float2((input.Position.y * (windDomain.y * 0.05) * 0.13), sin(FB_TotalTime))) * 0.025;
    noiseB = SimplexNoise(float2((input.Position.y * (windDomain.y * 0.05) * 0.02), cos(FB_TotalTime))) * 0.012;
    learpNoise = SimplexNoise(float2(FB_TotalTime * 0.02, FB_TotalTime * 0.02));
    float noiseScaleY = windDomain.y * lerp(noiseA, noiseB, learpNoise);
    
    noiseA = SimplexNoise(float2((input.Position.z * (windDomain.z * 0.1) * 0.15) * sin(FB_TotalTime), FB_TotalTime * 0.09)) * 0.6;
    noiseB = SimplexNoise(float2((input.Position.z * (windDomain.z * 0.15) * 0.2) * cos(FB_TotalTime), FB_TotalTime)) * 0.06;
    learpNoise = SimplexNoise(float2(FB_TotalTime * 0.022, FB_TotalTime * 0.022));
    float noiseScaleZ = windDomain.z * lerp(noiseA, noiseB, learpNoise);
    
    float displaceAmount = (sin(FB_TotalTime) + 2) * 0.1 + 0.5;
    
    float noiseStrength = 0.05;
    
    
    /////////////////////////////////// Matrix-multiplying the noise to the skinning-matrix ////////////////////////////////////////////////////////////////////////////////////////////
    
    float4x4 windEffect =
    {
        1 * noiseScaleZ*noiseStrength+1,                                          0,                                        0,                          0,
		0,                                          1 * noiseScaleY*noiseStrength+1,                                        0,                          0,
		0,                                                                        0,           1* noiseScaleX*noiseStrength+1,                          0,
		0,                                                                        0,                                        0,                          1
    };
    
    ////////////////////////////////////            Multiplying the Matricies              ////////////////////////////////////////////////////////////////////////////////////////////
    
    
    float4 vertexWorldPosition = mul(OB_ToWorld, mul(input.Position, mul(skinningMatrix, windEffect)));
    
    float4 vertexDisplacementPosition = float4(vertexWorldPosition.x + noiseScaleX, vertexWorldPosition.y + noiseScaleY, vertexWorldPosition.z + noiseScaleZ, vertexWorldPosition.w);

 
    if (OB_isInstanced)
    {
        vertexWorldPosition = mul(input.World, mul(input.Position, mul(skinningMatrix, windEffect)));
    }

    const float4 vertexViewPosition = mul(FB_ToView, vertexWorldPosition);
    const float4 vertexProjectionPosition = mul(FB_ToProjection,vertexViewPosition);

    const float3x3 worldNormalRotation = (float3x3) OB_ToWorld;
    const float3x3 skinNormalRotation = (float3x3) skinningMatrix;

    result.ProjectedPosition = vertexProjectionPosition;
    result.WorldPosition = vertexWorldPosition;

    result.VxColor = input.VxColor;
    result.NormalTex = input.NormalTex;
    result.Roughness = input.Roughness;
    result.AO = input.AO;
    result.AlbedoUV = input.AlbedoUV;
    result.NormalUV = input.NormalUV;
    result.RoughnessUV = input.RoughnessUV;
    result.AOUV = input.AOUV;

    result.Tangent = mul(worldNormalRotation, mul(input.Tangent, skinNormalRotation));
    result.Binormal = mul(worldNormalRotation, mul(input.Binormal, skinNormalRotation));
    result.Normal = normalize(mul(worldNormalRotation, mul(input.Normal, skinNormalRotation)));

    return result;
}
#include "Struct/ShaderStructs.hlsli"
#include "PBRFunctions.hlsli"
#include "ShaderFunctions.hlsli"


GBufferOutput main(VertexToPixel input)
{
    GBufferOutput result;

    float4 albedo = AlbedoTex.Sample(defaultSampler, input.AlbedoUV).rgba;
    if (albedo.a <= 0.05f) discard;

    float4 normalMapSample = NormalTex.Sample(defaultSampler, input.AlbedoUV);
    float2 normalMap = normalMapSample.ag;
    float ambientOcclusion = normalMapSample.b;
    float4 material = MaterialTex.Sample(defaultSampler, input.AlbedoUV);
    
    
    float3x3 TBN = float3x3
        (
            normalize(input.Tangent),
            normalize(input.Binormal),
            normalize(input.Normal)
        );
    if (OB_isTiling)
    {
        float3 WorldSpaceUV = (input.WorldPosition.rgb * MB_CustomValue1);
	
        //float4 albedo1 = AlbedoTex.Sample(wrapSampler, WorldSpaceUV).rgba;
        //float4 albedo2 = AlbedoTex.Sample(wrapSampler, WorldSpaceUV * 0.23).rgba;
        //float4 albedo3 = AlbedoTex.Sample(wrapSampler, WorldSpaceUV * 0.23).rgba;
        //albedo = MultiplyAlbedos(albedo1, albedo2);
		
        //normalMapSample = NormalTex.Sample(wrapSampler, WorldSpaceUV);
        //ambientOcclusion = normalMapSample.b;
        ////float3 normalMap2 = NormalTex.Sample(wrapSampler, float2(WorldSpaceUV.x * 0.23, WorldSpaceUV.y * 0.23)).agr;
        ////normalMap = MultiplyNormals(normalMap1, normalMap2);
        //float3 normalMap1 = NormalTex.Sample(wrapSampler, float2(WorldSpaceUV.x, WorldSpaceUV.y)).agr;
        //float3 normalMap2 = NormalTex.Sample(wrapSampler, float2(WorldSpaceUV.x * 0.23, WorldSpaceUV.y * 0.23)).agr;
        //normalMap = MultiplyNormals(normalMap1, normalMap2);
        
        //float4 pbr1 = MaterialTex.Sample(wrapSampler, float2(WorldSpaceUV.x, WorldSpaceUV.y));
        //float4 pbr2 = MaterialTex.Sample(wrapSampler, float2(WorldSpaceUV.x * 0.23, WorldSpaceUV.y * 0.23));
        //material = MultiplyMaterials(pbr1, pbr2);
        
    //Sampling stuff for triplanar shait   	
        float4 albedo_GB = AlbedoTex.Sample(wrapSampler, WorldSpaceUV.gb);
        float4 albedo_RB = AlbedoTex.Sample(wrapSampler, WorldSpaceUV.rb);
        float4 albedo_RG = AlbedoTex.Sample(wrapSampler, WorldSpaceUV.rg);
        
        float4 material_GB = MaterialTex.Sample(wrapSampler, WorldSpaceUV.gb);
        float4 material_RB = MaterialTex.Sample(wrapSampler, WorldSpaceUV.rb);
        float4 material_RG = MaterialTex.Sample(wrapSampler, WorldSpaceUV.rg);
     
        float ambientOcclusion_GB = NormalTex.Sample(wrapSampler, WorldSpaceUV.gb).b;
        float ambientOcclusion_RB = NormalTex.Sample(wrapSampler, WorldSpaceUV.rb).b;
        float ambientOcclusion_RG = NormalTex.Sample(wrapSampler, WorldSpaceUV.rg).b;

    //Sampling normals for triplanar shait 
        
        float3 VNWS = float3(-1, 1, 1) * sign(input.Normal);        
        float3 AWP = float3(1, 1, -1) * WorldSpaceUV;
        
        float2 normalSamplingX = float2(VNWS.r, 1) * AWP.gb;
        float2 normalSamplingY = float2(VNWS.g, 1) * AWP.rb;
        float2 normalSamplingZ = float2(VNWS.b, 1) * AWP.rg;
        
        float2 normalMap_X = NormalTex.Sample(wrapSampler, normalSamplingX).ag;
        float2 normalMap_Y = NormalTex.Sample(wrapSampler, normalSamplingY).ag;
        float2 normalMap_Z = NormalTex.Sample(wrapSampler, normalSamplingZ).ag;
	    
    //Calculating lerp value for triplanar shait    
        float3 someValue = pow(abs(input.Normal),  MB_CustomValue2);
        float theDot = dot(someValue, float3(1, 1, 1));
        float3 lerpValue = someValue / theDot;
	
        albedo = lerp(albedo_GB, albedo_RB, lerpValue.g);
        albedo = lerp(albedo, albedo_RG, lerpValue.b);        

        material = lerp(material_GB, material_RB, lerpValue.g);
        material = lerp(material, material_RG, lerpValue.b);
         
        ambientOcclusion = lerp(ambientOcclusion_GB, ambientOcclusion_RB, lerpValue.g);
        ambientOcclusion = lerp(ambientOcclusion, ambientOcclusion_RG, lerpValue.b);
             
        normalMap = lerp(normalMap_X, normalMap_Y, lerpValue.g);
        normalMap = lerp(normalMap, normalMap_Z, lerpValue.b);
        
        //TBN = float3x3
        //(
        //    normalize(-input.Tangent),
        //    normalize(-input.Binormal),
        //    normalize(input.Normal)
        //);
    }
   
    float3 pixelNormal;
    pixelNormal.xy = normalMap;
    pixelNormal = 2.0f * pixelNormal - 1;
    pixelNormal.z = sqrt(normalize(dot(pixelNormal.xy, pixelNormal.xy)));
    //pixelNormal.z = sqrt(1 - saturate(pixelNormal.x + pixelNormal.x + pixelNormal.y + pixelNormal.y));
    pixelNormal = normalize(pixelNormal);
    pixelNormal = normalize(mul(pixelNormal, TBN));

    //float4 clouds = SimplexNoise(input.WorldPosition.xz * 0.051 + (FB_TotalTime* 0.12));
    //float4 clouds2 = SimplexNoise(input.WorldPosition.xz * 0.07 - (FB_TotalTime * 0.083));
    //float4 cloudsX = lerp(clouds, clouds2, 0.5);

    //result.Albedo = lerp((albedo * MB_Color), (albedo * (clouds * 0.5)), 0.3);
    result.Albedo = albedo * MB_Color;
    result.Normal = float4(pixelNormal, 1.0f);
    material.a *= MB_EmissiveStrength;
    result.Material = material;
    result.VertexNormal = float4(normalize(input.Normal), 1.0f);
    result.WorldPosition = input.WorldPosition;
    result.AmbientOcclusion = ambientOcclusion;
    result.id = OB_id;
    float time = FB_TotalTime;
    float deltatime = FB_DeltaTime;

    return result;
}
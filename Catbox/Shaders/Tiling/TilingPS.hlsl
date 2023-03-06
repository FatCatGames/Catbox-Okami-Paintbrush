#include "../Struct/ShaderStructs.hlsli"
#include "../PBRFunctions.hlsli"
#include "../ShaderFunctions.hlsli"

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D pbrTexture : register(t2);
StructuredBuffer<LightData> lights : register(t9);
TextureCube environmentTexture : register(t10);
Texture2D shadowMap : register(t7);
Texture2D SSAOTexture : register(t8);

PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    float2 WorldSpaceUV = (input.WorldPosition.xz / 2.f);
	
	//Scale:ar om lite och kastar in i funktionen MultiplyAlbedos i ShaderFunctions.hlsli som lerpar mellan första och andra med 0.5 i alpha
    float4 albedo1 = albedoTexture.Sample(wrapSampler, WorldSpaceUV).rgba;
    float4 albedo2 = albedoTexture.Sample(wrapSampler, WorldSpaceUV * 0.23).rgba;
    float4 albedo3 = albedoTexture.Sample(wrapSampler, WorldSpaceUV * 0.23).rgba;
	
    float4 albedo = MultiplyAlbedos(albedo1, albedo2);
	
    result.Color.rgba = albedo;
	//tangent space matrix
    const float3x3 TBN = float3x3(
		normalize(input.Tangent),
		normalize(input.Binormal),
		normalize(input.Normal));

	//R = 0
	//G = normal Y
	//B = AO
	//A = normal X
	  
	//Scale:ar om lite och kastar in i funktionen MultiplyNormals i ShaderFunctions.hlsli som lerpar mellan första och andra med 0.5 i alpha
	float3 normalMap1 = normalTexture.Sample(wrapSampler, float2(WorldSpaceUV.x, WorldSpaceUV.y)).agb;
    float3 normalMap2 = normalTexture.Sample(wrapSampler, float2(WorldSpaceUV.x * 0.23, WorldSpaceUV.y * 0.23)).agb;
    float3 normalMap = MultiplyNormals(normalMap1, normalMap2);

    const float ao = normalMap.b;
    //first method : float3 pixelNormal = 1 - normalMap;
	float3 pixelNormal = 1 - normalMap;
    pixelNormal = 2.0f * pixelNormal - 1;
    //first method : pixelNormal.z = sqrt(1 - saturate(pixelNormal.x + pixelNormal.x + pixelNormal.y + pixelNormal.y));
    pixelNormal.z = sqrt(normalize(dot(pixelNormal.xy, pixelNormal.xy)));
	
 
	pixelNormal = normalize(pixelNormal);
    pixelNormal = normalize(mul(pixelNormal, TBN));

	//Scale:ar om lite och kastar in i funktionen MultiplyMaterials i ShaderFunctions.hlsli som lerpar mellan första och andra med 0.5 i alpha
    float4 pbr1 = pbrTexture.Sample(wrapSampler, float2(WorldSpaceUV.x * 0.23, WorldSpaceUV.y));
    float4 pbr2 = pbrTexture.Sample(wrapSampler, float2(WorldSpaceUV.x * 0.23, WorldSpaceUV.y * 0.23));
    float4 pbr = MultiplyMaterials(pbr1, pbr2);
	
    const float metalness = pbr.r;
    const float roughness = pbr.g;
    const float emissive = pbr.b;
    const float emissiveStr = pbr.a;

    const float3 toEye = normalize(FB_CamPosition.xyz - input.WorldPosition.xyz);
    const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

	//#ifdef _DEBUG

    if (FB_DebugMode != 0)
    {
        result.id = OB_id;
		//albedo
        if (FB_DebugMode == 1)
        {
            result.Color = albedoTexture.Sample(wrapSampler, WorldSpaceUV);
        }
		//vx Color
        else if (FB_DebugMode == 2)
        {
            result.Color = input.VxColor * MB_Color;
        }
		//vertex normals
        else if (FB_DebugMode == 3)
        {
			//float3 debugNormal = pixelNormal;
            float3 debugNormal = input.Normal;
            const float signedLength = (debugNormal.r + debugNormal.g + debugNormal.b) / 3;
            if (signedLength < 0)
            {
                debugNormal = float3(1 - abs(debugNormal));
            }
            result.Color.rgb = debugNormal;
        }
		//normal map
        else if (FB_DebugMode == 4)
        {
            float3 debugPixelNormal = normalMap;
            debugPixelNormal = 2.0f * debugPixelNormal - 1;
            debugPixelNormal.z = sqrt(1 - saturate(debugPixelNormal.x + debugPixelNormal.x + debugPixelNormal.y + debugPixelNormal.y));
            debugPixelNormal = normalize(debugPixelNormal);
            result.Color = float4(debugPixelNormal.r, debugPixelNormal.g, debugPixelNormal.b, 1);
        }
		//UVs
        else if (FB_DebugMode == 5)
        {
            result.Color = float4(WorldSpaceUV.r, WorldSpaceUV.g, 0, 1);
        }
		//AO
        else if (FB_DebugMode == 6)
        {
            result.Color.rgb = ao;
        }
		//Roughness
        else if (FB_DebugMode == 7)
        {
            result.Color.rgb = roughness;
        }
		//Metallic
        else if (FB_DebugMode == 8)
        {
            result.Color.rgb = metalness;
        }
		//Emissive
        else if (FB_DebugMode == 9)
        {
            result.Color.rgb = emissive * emissiveStr;
        }
        return result;
    }

	
    //const float ssaoValue = SSAOTexture.Sample(defaultSampler, WorldSpaceUV).r;
    const float ssaoValue = 1;
	
	//Oklart om SSAO ska tile:as om som de andra texturerna, post process-baserad?
	
	/*
    const float ssaoValue1 = SSAOTexture.Sample(defaultSampler, WorldSpaceUV).r;
    const float ssaoValue2 = SSAOTexture.Sample(defaultSampler, WorldSpaceUV * 0.23).r;
	
    const float ssaoValue = MultiplySsao(ssaoValue1, ssaoValue2);
	*/
	
    float3 lightResult;
	//Calculate directional lights
    for (int i = 0; i < SLB_PointStartIndex; i++)
    {
		float3 lightAmount = EvaluateDirectionalLight(
			diffuseColor,
			specularColor,
			pixelNormal,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity,
			-lights[i].LB_Forward,
			toEye
		);

		if (lights[i].LB_CastShadows)
		{
			const float4x4 lightView = lights[i].LB_View;
			const float4x4 lightProj = lights[i].LB_Projection;
			const float4 worldToLightView = mul(lightView, input.WorldPosition);
			const float4 lightViewToProj = mul(lightProj, worldToLightView);

			float2 projectedTexCoord;
			projectedTexCoord.x = lightViewToProj.x / lightViewToProj.w / 2.0f + 0.5f;
			projectedTexCoord.y = -lightViewToProj.y / lightViewToProj.w / 2.0f + 0.5f;

			if (saturate(projectedTexCoord.x) == projectedTexCoord.x && saturate(projectedTexCoord.y) == projectedTexCoord.y)
			{
				const float shadowBias = 0.0005f;
				const float shadow = 0.0f;
				const float viewDepth = (lightViewToProj.z / lightViewToProj.w) - shadowBias;
				const float lightDepth = shadowMap.Sample(pointClampSampler, projectedTexCoord).r;

				if (lightDepth < viewDepth)
				{
					lightAmount *= shadow;
				}
			}
		}
		lightResult += lightAmount * ssaoValue;
    }

	//Calculate point lights
    for (i = SLB_PointStartIndex; i < SLB_SpotStartIndex; i++)
    {
        float3 dir = lights[i].LB_Pos - input.WorldPosition.xyz;
        float dist = length(dir);
        float multiplier = clamp(dot(normalize(dir), pixelNormal), 0, 1);
        multiplier *= clamp(lights[i].LB_MaxRange - dist, 0, lights[i].LB_MaxRange) / lights[i].LB_MaxRange;
        multiplier = pow(multiplier, 2);
        lightResult += lights[i].LB_Color.rgb * lights[i].LB_Intensity * 3 * multiplier;
    }

	//Calculate spot lights
    for (i = SLB_SpotStartIndex; i < SLB_IBLStartIndex; i++)
    {
        lightResult += EvaluateSpotLight(
			diffuseColor.rgb,
			specularColor.rgb,
			pixelNormal.rgb,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity * 3,
			lights[i].LB_MaxRange,
			lights[i].LB_Pos.xyz,
			lights[i].LB_Forward.xyz,
			lights[i].LB_OuterRadius,
			lights[i].LB_InnerRadius,
			toEye.xyz,
			input.WorldPosition.xyz
		);
    }

	//Calculate IBL light
    for (i = SLB_IBLStartIndex; i < SLB_LightsCount; i++)
    {
        float3 ambientLighting = EvaluateAmbience(
			environmentTexture,
			pixelNormal.rgb,
			normalMap,
			toEye.xyz,
			roughness,
			ao,
			diffuseColor.rgb,
			specularColor.rgb
		);
        ambientLighting *= lights[i].LB_Intensity;

        /*const float3 directLighting = EvaluateDirectionalLight(
			diffuseColor,
			specularColor,
			pixelNormal,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity,
			-lights[i].LB_Forward,
			toEye
		);*/

		lightResult += ambientLighting * ssaoValue;
    }

    result.Color.rgb = (albedo.rgb * MB_Color.rgb * lightResult + (albedo.rgb * emissive * emissiveStr * 5));
    result.Color.a = result.Color.a * MB_Color.a;
    result.id = OB_id;


	//some different variants to try out once I have a more complex environment set up

	/*float multiplier = 1 - dot(normalize(LB_Forward), input.Normal);
	result.Color.rgb *= LB_Color * LB_Intensity * multiplier;*/

	/*float multiplier = 1 - dot(normalize(LB_Forward), pixelNormal);
	result.Color.rgb *= LB_Color * LB_Intensity * saturate(multiplier);*/

	//POINT


		//BLINN PHONG MODEL (use once material roughness is implemented)
		

		//ENVIRONMENT

			/*const float3 L = -1 * normalize(LB_Forward);
			const float3 N = pixelNormal;
			const float LdotN = saturate(dot(L, N));
			const float3 C = LB_Color;
			const float Intensity = LB_Intensity;
			const float3 IPixel = LdotN * C * Intensity;
			float3 diffuse = albedo * IPixel;

			const float3 env = environmentTexture.SampleLevel(defaultSampler, input.Normal, 5).rgb;
			const float3 ambient = albedo * env;
			result.Color.rgb = diffuse + ambient;*/



			//}

    return result;
}
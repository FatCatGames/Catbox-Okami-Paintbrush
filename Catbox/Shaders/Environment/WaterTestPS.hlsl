#include "../Struct/ShaderStructs.hlsli"
#include "../PBRFunctions.hlsli"
#include "../ShaderFunctions.hlsli"
//#include "../Struct/PostProcessingShaderStructs.hlsli"

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D pbrTexture : register(t2);
Texture2D depthTexture : register(t3);
StructuredBuffer<LightData> lights : register(t9);
TextureCube environmentTexture : register(t10);
Texture2D shadowMap : register(t7);
Texture2D SSAOTexture : register(t8);


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    float2 WorldSpaceUV = (input.WorldPosition.xz / 5.f);
	
    
    
    /*
    ______                               
    | ___ \                              
    | |_/ /_ _ _ __  _ __   ___ _ __ ___ 
    |  __/ _` | '_ \| '_ \ / _ \ '__/ __|
    | | | (_| | | | | | | |  __/ |  \__ \
    \_|  \__,_|_| |_|_| |_|\___|_|  |___/
    */
    
    float translateX = WorldSpaceUV.x * 0.05 - (FB_TotalTime * 0.002);
    float translateY = WorldSpaceUV.y * 0.05 + (FB_TotalTime * 0.001);
    float2 translateXY = float2(translateX, translateY);
    
    float translateX2 = WorldSpaceUV.x * 0.12 + (FB_TotalTime * 0.005);
    float translateY2 = WorldSpaceUV.y * 0.12 - (FB_TotalTime * 0.0017);
    float2 translateXY2 = float2(translateX2, translateY2);
    
    float translateXwaves = WorldSpaceUV.x * 0.002 - (FB_TotalTime * 0.001);
    float translateYwaves = WorldSpaceUV.y * 0.002 + (FB_TotalTime * 0.0007);
    float2 translateXYwaves = float2(translateXwaves, translateYwaves);
    
    float translateX2waves = WorldSpaceUV.x * 0.021 + (FB_TotalTime * 0.001);
    float translateY2waves = WorldSpaceUV.y * 0.021 - (FB_TotalTime * 0.0005);
    float2 translateXY2waves = float2(translateX2waves, translateY2waves);
    
    ///////////////////////////////////////////////////////////////////////
    /*
     _____         _                  _____                       _ _             
    |_   _|       | |                /  ___|                     | (_)            
      | | _____  _| |_ _   _ _ __ ___\ `--.  __ _ _ __ ___  _ __ | |_ _ __   __ _ 
      | |/ _ \ \/ / __| | | | '__/ _ \`--. \/ _` | '_ ` _ \| '_ \| | | '_ \ / _` |
      | |  __/>  <| |_| |_| | | |  __/\__/ / (_| | | | | | | |_) | | | | | | (_| |
      \_/\___/_/\_\\__|\__,_|_|  \___\____/ \__,_|_| |_| |_| .__/|_|_|_| |_|\__, |
                                                           | |               __/ |
                                                           |_|              |___/ 
    */
    
    //float4 cubeMap = environmentTexture.Sample(defaultSampler, input.AlbedoUV).rgb;
    
    float4 albedo1 = albedoTexture.Sample(wrapSampler, translateXY).rgba;
    float4 albedo2 = albedoTexture.Sample(wrapSampler, translateXY2).rgba;

    //float4 albedo = MultiplyAlbedos(albedo1, albedo2);
    float4 albedo = normalize(albedo1 * albedo2);
    
    result.Color.rgba = albedo;
	//tangent space matrix
    const float3x3 TBN = float3x3(
		normalize(-input.Tangent),
		normalize(-input.Binormal),
		normalize(input.Normal));

	//R = 0
	//G = normal Y
	//B = AO
	//A = normal X

    float3 normalMap1 = normalTexture.Sample(wrapSampler, translateXY).agb;
    float3 normalMap2 = normalTexture.Sample(wrapSampler, translateXY2).agb;
    
    float3 WhiteWaterMap1 = normalTexture.Sample(wrapSampler, translateXY * 5).agb;
    float3 WhiteWaterMap2 = normalTexture.Sample(wrapSampler, translateXY2 * 10).agb;
    
    float creatures = normalTexture.Sample(wrapSampler, translateXY * 8).r;
    float creatures2 = normalTexture.Sample(wrapSampler, translateXY2 * 8).r;
    float creaturesMask = normalTexture.Sample(wrapSampler, translateXY * 0.45).r;
    float creaturesMask2 = normalTexture.Sample(wrapSampler, translateXY2 * 0.45).r;
    
    float creaturesCombined = lerp(0, creatures * creatures2, creaturesMask * creaturesMask2);
    
    float3 creaturesColor = lerp(float3(0, 0, 0), float3(0, 0.1, 0), creaturesCombined);
    
    //creaturesCombined = lerp(0, 1, creaturesCombined);
    
    float4 pbr1 = pbrTexture.Sample(wrapSampler, translateXY);
    float4 pbr2 = pbrTexture.Sample(wrapSampler, translateXY2);
    float4 pbr = MultiplyMaterials(pbr1, pbr2);
    
    //Generate z-normal for pbr-normal-conversion
    float pbrZ = sqrt(normalize(dot(pbr.xy, pbr.xy)));
    
    float3 wavesNormal = float3(pbr.b, pbr.a, pbrZ);
    
    float3 normalMapWaves1 = float3(normalTexture.Sample(wrapSampler, translateXYwaves).ba, pbrZ);
    float3 normalMapWaves2 = float3(normalTexture.Sample(wrapSampler, translateXY2waves).ba, pbrZ);

    float3 multipliedWaves = MultiplyNormals(normalMapWaves1, normalMapWaves2);
    
    wavesNormal = normalize(wavesNormal);
    wavesNormal = normalize(mul(wavesNormal, TBN));
    
    float3 normalMap = lerp(lerp(normalMap1, normalMap2, 0.5), lerp(normalMapWaves1, normalMapWaves2, 0.5), 0.5);
    float3 WhiteWaterTexture = lerp(lerp(WhiteWaterMap1, WhiteWaterMap2, 0.5), lerp(WhiteWaterMap1, normalMapWaves2, 0.5), 0.5);

    const float ao = normalMap.b;

    float3 pixelNormal = 1 - normalMap;
    pixelNormal = 2.0f * pixelNormal - 1;
    pixelNormal.z = sqrt(normalize(dot(pixelNormal.xy, pixelNormal.xy)));
    pixelNormal = normalize(pixelNormal);
    pixelNormal = normalize(mul(pixelNormal, TBN));
    
    //pixelNormal = normalize(wavesNormal * pixelNormal);
    
    const float metalness = 0.999f;
    const float roughness = 0.15f;
    const float emissive = pbr.b * 0;
    const float emissiveStr = pbr.a * 0;

    const float3 toEye = normalize(FB_CamPosition.xyz - input.WorldPosition.xyz);
    //const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness); original
    const float3 specularColor = lerp((float3) 0.04f, float3(0, 0, 0.01), metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, float3(0, 0.01, 0), 1 - metalness);
    
    //////////////////////// SPELA SHORELINE ///////////////////////////////
    

    

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
			roughness * 0,
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

    for (i = SLB_PointStartIndex; i < SLB_SpotStartIndex; i++)
    {
		//PBR TGA VER
        /*
		lightResult += EvaluatePointLight(
			diffuseColor,
			specularColor,
			pixelNormal,
			roughness * 0,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity * 100,
			lights[i].LB_MaxRange, 
			lights[i].LB_Pos,
			toEye,
			input.WorldPosition.xyz
		);
        */
		//VER 1
        
        float3 dir = pow(lights[i].LB_Pos - input.WorldPosition.xyz, 4);
        float dist = length(dir);
        float multiplier = saturate(dot(normalize(dir), pixelNormal));
        multiplier *= clamp(lights[i].LB_MaxRange - dist, 0, lights[i].LB_MaxRange) / lights[i].LB_MaxRange;
        multiplier = pow(multiplier, 2);
        
        if (lights[i].LB_CastShadows)
        {
            multiplier = 0;
        }
        
        //lightResult += lights[i].LB_Color.rgb * lights[i].LB_Intensity * 1;
        
    }

	//Calculate spot lights
    for (i = SLB_SpotStartIndex; i < SLB_IBLStartIndex; i++)
    {
        lightResult += EvaluateSpotLight(
			diffuseColor.rgb,
			specularColor.rgb,
			pixelNormal.rgb,
			roughness * 0.01,
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

    float3 viewDirection = normalize(input.ProjectedPosition.xyz);
    float3 lightDirection = normalize(toEye.xyz);
    float3 rotlightDirection = float3(lightDirection.x, lightDirection.y, lightDirection.z);
    
    float dotProduct = abs(sin(dot(toEye, input.Normal)));
    float dotProductofLight = abs(dot(rotlightDirection, input.Normal));
    float mixdotProduct = dotProduct * dotProductofLight;
    
    float3 cubeMapTexture = EvaluateAmbience(
			environmentTexture,
			pixelNormal.rgb,
			normalMap,
			toEye.xyz,
			roughness,
			ao,
			diffuseColor.rgb,
			specularColor.rgb
		);
    
    
    //Dot-examples : lerp(0,1,dotProduct) = invisible along horizon, visible straight down
    
    			//float3 debugNormal = pixelNormal;
    float3 debugNormal = input.Normal;
    const float signedLength = (debugNormal.r + debugNormal.g + debugNormal.b) / 3;
    if (signedLength < 0)
    {
        debugNormal = float3(1 - abs(debugNormal));
    }
    result.Color.rgb = debugNormal;
    
    //float MurkyWater = ((abs(sin(1 - mixdotProduct * 3.960 * normalMap.z)) * 0.170) * normalMap.x) * 1.810 + 0.900;
    float MurkyWater = ((abs(sin(1 - mixdotProduct * (3.960 * 1) * normalMap.z)) * 0.170) * normalMap.x) * 1.810 + (0.900 * 1);
    float ClearWater = 1 - mixdotProduct;
    
    float MurkyandClearAlpha = lerp(MurkyWater, ClearWater, MB_CustomFloat4.x) + creaturesCombined;
    
    float3 WaterReflections = lerp(cubeMapTexture + (pow(lightResult, 1)), float3(0, 0.05, 0), dotProduct);
    
    //result.Color.rgb = lightResult;
    //result.Color.rgb = lerp(cubeMapTexture, lerp((float3(0,0,0.01) * lightResult), lerp(0, 0, creaturesCombined), albedo.a), dotProduct);
    float3 WhiteWater = lerp(float3(1, 1, 1), float3(0, 0, 0), pow(normalize(normalMap), 2.330));
    
    result.Color.rgb = WaterReflections;
    //result.Color.rgb = depthTexture.Sample(wrapSampler, toEye.xy).rgb;
    //result.Color.rgb = debugNormal;
    
    result.Color.a = lerp(MurkyandClearAlpha, float(1.f), MB_CustomFloat4.y);
    
    //result.Color.a = depthTexture;
    
    //result.Color.a = ((abs(sin(1 - mixdotProduct * MB_CustomFloat4.x * normalMap.z)) * MB_CustomFloat4.y) * normalMap.x) * MB_CustomFloat4.z + MB_CustomFloat4.w; <- MURKY WATER
    //result.id = OB_id;
    //result.WorldPosition = input.WorldPosition;
    //result.Color.a = lerp(1 - result.Color.a, cubeMapTexture * 0.1, dotProduct);
    //result.Color.a = (albedo.a * multipliedWaves.b);
    //result.id = OB_id;


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
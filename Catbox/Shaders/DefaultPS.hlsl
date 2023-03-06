#include "Struct/ShaderStructs.hlsli"
#include "PBRFunctions.hlsli"
#include "ShaderFunctions.hlsli"

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D pbrTexture : register(t2);
Texture2D ambientOcclusionTexture : register(t5);
Texture2D ScreenUVTexture : register(t6);
Texture2D shadowMap : register(t7);
Texture2D SSAOTexture : register(t8);
StructuredBuffer<LightData> lights : register(t9);
TextureCube environmentTexture : register(t10);

PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	float4 albedo = albedoTexture.Sample(defaultSampler, input.AlbedoUV).rgba;
	albedo = albedo.rgba * MB_Color;
	if (albedo.a < 0.05f) discard;

	//tangent space matrix
	const float3x3 TBN = float3x3(
		normalize(input.Tangent),
		normalize(input.Binormal),
		normalize(input.Normal));

	//R = 0
	//G = normal Y
	//B = AO
	//A = normal X
    float3 normalMap = normalTexture.Sample(defaultSampler, input.AlbedoUV).agb;
	const float ao = normalMap.b;
	
	float3 pixelNormal = normalMap;
	pixelNormal = 2.0f * pixelNormal - 1;
	pixelNormal.z = sqrt(normalize(dot(pixelNormal.xy, pixelNormal.xy)));
	pixelNormal = normalize(pixelNormal);
	pixelNormal = normalize(mul(pixelNormal, TBN));

	float4 pbr = pbrTexture.Sample(defaultSampler, input.AlbedoUV);
	const float metalness = pbr.r;
	const float roughness = pbr.g;
	const float emissive = pbr.b;
	const float emissiveStr = pbr.a * MB_EmissiveStrength;
	//const float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.).rgb;

	const float3 toEye = normalize(FB_CamPosition.xyz - input.WorldPosition.xyz);
	const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
	const float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

	float2 screenUV = ScreenUVTexture.Sample(defaultSampler, input.AlbedoUV).rg;
	const float ssaoValue = SSAOTexture.Sample(defaultSampler, screenUV).r;

	result.id = OB_id;


	float4 clouds = SimplexNoise(input.WorldPosition.xz * 0.051 + (FB_TotalTime * 0.12));
	float4 clouds2 = SimplexNoise(input.WorldPosition.xz * 0.07 - (FB_TotalTime * 0.083));
	float4 cloudsX = lerp(clouds, clouds2, 0.5);

	albedo.rgb = lerp((albedo.rgb * MB_Color.rgb), (albedo.rgb * (clouds * 0.5)), 0.3);

	if (FB_DebugMode != 0)
	{
		result.Color.a = 1;
		//albedo
		if (FB_DebugMode == 1)
		{
			result.Color = albedo;
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
			const float3 normalMap = normalTexture.Sample(defaultSampler, input.AlbedoUV).agb;
            result.Color = float4(normalMap.r, normalMap.g, 1, 1);
        }
		//UVs
		else if (FB_DebugMode == 5)
		{
			result.Color = float4(input.AlbedoUV.r, input.AlbedoUV.g, 1, 1);
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
		//PBR TGA VER

		/*lightResult += EvaluatePointLight(
			diffuseColor,
			specularColor,
			pixelNormal,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity * 100,
			lights[i].LB_MaxRange,
			lights[i].LB_Pos,
			toEye,
			worldPosition.xyz
		);*/

		//VER 1

		float3 dir = lights[i].LB_Pos - input.WorldPosition.xyz;
		float dist = length(dir);
		float multiplier = saturate(dot(normalize(dir), pixelNormal));
		multiplier *= clamp(lights[i].LB_MaxRange - dist, 0, lights[i].LB_MaxRange) / lights[i].LB_MaxRange;
		multiplier = pow(multiplier, 2);

		if (lights[i].LB_CastShadows)
		{
			multiplier = 0;
		}

		lightResult += lights[i].LB_Color.rgb * lights[i].LB_Intensity * 3 * multiplier;
	}

	//Calculate spot lights
	for (int i = SLB_SpotStartIndex; i < SLB_IBLStartIndex; i++)
	{
		lightResult += EvaluateSpotLight(
			diffuseColor.rgb,
			specularColor.rgb,
			pixelNormal.rgb,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity,
			lights[i].LB_MaxRange,
			lights[i].LB_Pos.xyz,
			lights[i].LB_Forward.xyz,
			lights[i].LB_OuterRadius,
			lights[i].LB_InnerRadius,
			toEye.xyz,
			input.WorldPosition.xyz
		);
	}

	lightResult = lightResult + (albedo.rgb * emissive * emissiveStr * 5);
	result.LightResult = lightResult;

	//Calculate IBL light
	for (i = SLB_IBLStartIndex; i < SLB_LightsCount; i++)
	{
		float3 ambientLighting = EvaluateAmbience(
			environmentTexture,
			pixelNormal.rgb,
			input.Normal.rgb,
			toEye.xyz,
			roughness,
			ao,
			diffuseColor.rgb,
			specularColor.rgb
		);
		ambientLighting *= lights[i].LB_Intensity;

		//const float3 directLighting = EvaluateDirectionalLight(
		//	diffuseColor.rgb,
		//	specularColor.rgb,
		//	pixelNormal.rgb,
		//	roughness,
		//	lights[i].LB_Color.rgb,
		//	lights[i].LB_Intensity,
		//	-lights[i].LB_Forward.xyz,
		//	toEye.xyz
		//);

		lightResult += ambientLighting * ssaoValue;
	}

	result.Color.rgb = albedo.rgb * lightResult;
	result.Color.a = albedo.a;
	result.WorldPosition = input.WorldPosition;

	return result;
}
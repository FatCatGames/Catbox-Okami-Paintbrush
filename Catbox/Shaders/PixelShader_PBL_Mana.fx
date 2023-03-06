#include "PBRFunctions.fx"


float2 RotateAroundCenterUV(float2 texCenterOffset, float speed)
{
	float rotCos = cos(myTotalTime * speed);
	float rotSin = sin(myTotalTime * speed);

	float rotCosX = rotCos * texCenterOffset.x;
	float rotCosY = rotCos * texCenterOffset.y;

	float rotSinX = rotSin * texCenterOffset.x;
	float rotSinY = rotSin * texCenterOffset.y;

	float rotX = rotCosX - rotSinY;
	float rotY = rotSinX + rotCosY;
	float2 rotXY = float2(rotX, rotY) + 0.5;
	return rotXY;
}

float CircleFromPlane(float2 texCenterOffset)
{
	//Inverts texCenterOffset
	float circleLerp = lerp(1, 0, length(texCenterOffset));
	float circle = ceil(circleLerp - 0.5);

	return circle;
}

float HealthAmount(float2 texUV, float albedoRed)
{
	float normalizedHealth = max(min(myCustomValues.x, 1),0);
	float healthAmount = smoothstep(0, 1, (texUV.y + (albedoRed*0.15)) + (normalizedHealth-1));
	return healthAmount;
}

float4 CalculatePixelPBR(PixelInput input)
{
	//float3 normal = input.normal;
    float2 tex = input.tex[0].xy;
	
	float2 texCenterOffset = tex - 0.5;
	float circleLerp = lerp(1, 0, saturate(length(texCenterOffset)));
	float circle = ceil(circleLerp - 0.5);
	float circleLearpInverted = pow(circleLerp*-1+1, 3)*30;

	float2 rotXYPositiv = RotateAroundCenterUV(texCenterOffset, 1);
	float2 rotXYNegative = RotateAroundCenterUV(texCenterOffset, 0.5);
	
	//Sample emissive
	float emissive = shaderTextures[MATERIAL].Sample(SampleType, tex).b;

	float3 albedoPositive = GammaToLinear(shaderTextures[DIFFUSE].Sample(SampleType, rotXYPositiv + (circleLearpInverted*0.18)).r);
	float3 albedoNegative = GammaToLinear(shaderTextures[DIFFUSE].Sample(SampleType, rotXYNegative + (circleLearpInverted*0.12)).r);
	
	float3 albedoPanningGreen = GammaToLinear(shaderTextures[DIFFUSE].Sample(SampleType, float2(tex.x + myTotalTime * -0.3, tex.y + myTotalTime * 0.5) + (circleLearpInverted*0.05)).g);
	float3 albedoPanningBlue = GammaToLinear(shaderTextures[DIFFUSE].Sample(SampleType, float2(tex.x + myTotalTime * -0.3, tex.y + myTotalTime * 0.3) * 1.15).b);

	float3 albedoAlpha = GammaToLinear(shaderTextures[DIFFUSE].Sample(SampleType, float2(tex.x, tex.y)).a);
	float4 albedo = shaderTextures[DIFFUSE].Sample(SampleType, tex);

	albedo.xyz = (albedoPositive * albedoNegative)*5 + (albedoPanningGreen * pow(circleLerp, 5) * 5 * (myCustomValues.x*2)) + (albedoPanningBlue * 0.3);
	float alphaGradient = HealthAmount(tex, albedo.r);

	float upperLimitMask = lerp(1,0,min(floor(alphaGradient + 0.9995),1));
	float3 upperLimitMaskColor = upperLimitMask * float3(0.03, 0, 0);
	float3 maskColor = (alphaGradient + circleLearpInverted*0.15) * float3(0.5, 0.5, 0.1);
	float alpha = ceil(alphaGradient);

	float3 orbRed = float3(0.05, 0.0, 0.0) * maskColor + upperLimitMaskColor + (albedoAlpha * float3(0.5, 0, 0));
	float3 orbGreen = float3(0, emissive * 10, 0) * maskColor + upperLimitMaskColor;
	float3 orbBlue = float3(0, 0, emissive * 10) * maskColor + upperLimitMaskColor;

	return float4(LinearToGamma(orbRed),  alpha * circle);
}
	
/* PixelInput has a few values to play with:
	float4 position	<-- Pixel position
	float4 worldPos <-- vertex worldpos
	float3 normal	<-- pixel normal
	float3 tangent	<-- pixel tangent 
	float3 binormal <-- pixel binormal
	float2 tex[4]	<-- UV coords 1, 2, 3, 4
	float4 vertexColors[4]	<-- VertexColors, 1, 2, 3, 4
	float3x3 tangentToWorld	<-- a matrix representing the tangent to world space
*/
/*
	WE also have a coule of global values to play with, USAGE: just the variable name, eg: myCustomValues.x;
	float4 myCustomValues; <-- Four floats that can be manually set within the editor
	matrix World;	<-- The world matrix of the model
	matrix View;	<-- Camera matrix
	matrix Projection;	<-- Projection matrix
	matrix WorldViewProjection;	<-- A combined matrix 
	float myTotalTime;	<-- the total time since the start of the program
	float myStopTimer; <-- see shaderTimer in the editor
*/



float4 DoPBLPacked(PixelInput input) : SV_TARGET
{  
	float4 pixelColor = CalculatePixelPBR(input);
	return pixelColor;
}
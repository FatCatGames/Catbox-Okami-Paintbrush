SamplerState defaultSampler : register(s0);
SamplerState wrapSampler : register(s1);
SamplerState pointClampSampler : register(s2);
SamplerState pointWrapSampler : register(s3);


struct VertexInput
{
	float4 Position	 : POSITION;
	float4 VxColor	 : COLOR0;
	float3 NormalTex	 : COLOR1;
	float3 Roughness	 : COLOR2;
	float3 AO			 : COLOR3;
	float2 AlbedoUV	 : TEXCOORD0;
	float2 NormalUV	 : TEXCOORD1;
	float2 RoughnessUV : TEXCOORD2;
	float2 AOUV		 : TEXCOORD3;

	float3 Tangent	 : TANGENT;
	float3 Binormal	 : BINORMAL;
	float3 Normal	 : NORMAL;

	uint4  BoneIDs	 : BONEIDS;
	float4 BoneWeights : BONEWEIGHTS;

	float4x4 World	 : WORLD;
	uint InstanceID : SV_InstanceID;
};

struct SimpleVertexInput
{
	float4 Position	 : POSITION;
	float4 World	 : WORLD;
};

struct SimpleGeometryInput
{
	float4 Position : SV_POSITION;
	float2 UV: TEXCOORD;
};

struct VertexToPixel
{
	float4 ProjectedPosition    : SV_POSITION;
	float4 WorldPosition		  : POSITION;

	float4 VxColor     : COLOR0;
	float3 NormalTex	 : COLOR1;
	float3 Roughness	 : COLOR2;
	float3 AO			 : COLOR3;
	float2 AlbedoUV	 : TEXCOORD0;
	float2 NormalUV	 : TEXCOORD1;
	float2 RoughnessUV : TEXCOORD2;
	float2 AOUV		 : TEXCOORD3;

	float3 Tangent	 : TANGENT;
	float3 Binormal	 : BINORMAL;
	float3 Normal	 : NORMAL;
};

struct PixelOutput
{
	float4 Color : SV_TARGET0;
	uint id : SV_TARGET1;
	float4 WorldPosition : SV_TARGET2;
	float3 LightResult : SV_TARGET3;
};


struct DeferredVertexInput
{
	unsigned int Index : SV_VertexID;
};

struct DeferredVertexToPixel
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};


struct GBufferOutput
{
	float4 Albedo                : SV_TARGET0;
	float4 Normal                : SV_TARGET1;
	float4 Material              : SV_TARGET2;
	float4 VertexNormal          : SV_TARGET3;
	float4 WorldPosition         : SV_TARGET4;
	float AmbientOcclusion		 : SV_TARGET5;

	uint id						 : SV_TARGET7;
};


struct LightData
{
	float4x4 LB_View;
	float4x4 LB_Projection;

	float4 LB_Color; //16

	float LB_Intensity; //4
	float LB_MaxRange; //4
	float LB_InnerRadius; //4
	float LB_OuterRadius; //4

	float3 LB_Pos; //12
	int LB_LightType; //4
	float3 LB_Forward; //12
	int LB_Index; //4

	int LB_IsActive; //4
	int LB_CastShadows; //4
	float LB_NearPlane; //4
	float LB_FarPlane; //4
};



cbuffer FrameBuffer : register(b0)
{
	float4x4 FB_ToView;
	float4x4 FB_ToProjection;
	float3 FB_CamPosition;
	int FB_DebugMode;
	float FB_TotalTime;
	float FB_DeltaTime;
	float2 FB_Resolution;
	float4 FB_FrustumCorners[4];
}

cbuffer ObjectBuffer : register(b1)
{
	float4x4 OB_ToWorld;
	float3 OB_Scale;
	uint OB_id;
	float OB_SelectionMask;
	float OB_AliveTime;
	bool OB_isStatic;
	bool OB_isInstanced;
	bool OB_isTiling;
}

cbuffer MaterialBuffer : register(b2)
{
	float4 MB_Color;
	float MB_EmissiveStrength;
	float MB_CustomValue1;
	float MB_CustomValue2;
	float MB_CustomValue3;
	float4 MB_CustomFloat4;
}

cbuffer SkeletonBuffer : register(b3)
{
	float4x4 SB_BoneData[256];
}

cbuffer SceneLightsBuffer : register(b4)
{
	int SLB_PointStartIndex;
	int SLB_SpotStartIndex;
	int SLB_IBLStartIndex;
	int SLB_LightsCount;
}

//registers b10-b20 are reserved for post processing

Texture2D AlbedoTex : register(t0);
Texture2D NormalTex : register(t1);
Texture2D MaterialTex : register(t2);
Texture2D VertexNormalTex : register(t3);
Texture2D WorldPosTex : register(t4);
Texture2D AOTex : register(t5);
Texture2D TextureSlot7 : register(t6);
Texture2D TextureSlot8 : register(t7);
Texture2D NoiseTex : register(t8);
Texture2D TextureSlot10 : register(t9);
Texture2D TextureSlot11 : register(t10);
Texture2D DepthTex : register(t11);

#include "../ShaderFunctions.hlsli"
#include "../Struct/ShaderStructs.hlsli"
#include "../Color/ColorConversion.hlsli"
#include "UIShaderStructs.hlsli"

#define bluishColor float3(0.12941176, 0.701960784, 0.839215686)
Texture2D albedoTexture : register(t0);

cbuffer ResourceInfoBuffer : register(b10)
{
    float CoolDownTime;
    float FlashTime;
    float2 Padding;
};


PixelOutput main(UISpriteVertexToPixel input)
{
    PixelOutput result;

    //Counts down 1..0          Counts up 0..1
    float invCoolDownTime = 1 - CoolDownTime;
    
    float2 uv = input.uv;   
    result.Color = albedoTexture.Sample(defaultSampler, uv);
    float innerFrame = Rect(uv, 0.04, 0.96, 0.04, 0.96, 0.01);
    float outerFrame = Rect(uv, 0, 1, 0, 1, 0.03);
   
    //Desaturate pixel
    float desatSwitch = floor(invCoolDownTime + 0.001);
    desatSwitch = Remap(0, 1, 0.3, 1, desatSwitch) + fjongInEaseOut(FlashTime) * 0.5;
    result.Color.rgb = rgb2hsv(result.Color.rgb);
    result.Color.rgb = float3(result.Color.r, result.Color.g * desatSwitch, result.Color.b);
    result.Color.rgb = hsv2rgb(result.Color.rgb);

    float gradRect = 1 - Rect(uv, 0.04, 0.96, invCoolDownTime + 0.04, 0.96, 0.02) * ((easeOutCubic(1 - uv.y + 0.2)));
    float flashFrame = outerFrame - innerFrame;
    float flash = fjongInEaseOut(FlashTime) * flashFrame;
    float3 flashRGB = bluishColor * flash;
    
    result.Color.rgb = result.Color.rgb * gradRect + flashRGB;
    result.id = UI_id;
    
    return result;
} 
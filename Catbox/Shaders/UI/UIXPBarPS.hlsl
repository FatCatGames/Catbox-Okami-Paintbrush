#include "../ShaderFunctions.hlsli"
#include "../Struct/ShaderStructs.hlsli"
#include "UIShaderStructs.hlsli"


cbuffer ResourceInfoBuffer : register(b10)
{
    float Resource;
};


PixelOutput main(UISpriteVertexToPixel input)
{
    PixelOutput result;

    float2 uv = input.uv;
    float resource = Resource;
    float theBar = Rect(uv, 0, resource, 0, 1, 0.005);

    
    result.Color.rgb = UI_color.rgb * theBar;
    result.Color.a = UI_color.a;
    result.id = UI_id;
    
    return result;
}
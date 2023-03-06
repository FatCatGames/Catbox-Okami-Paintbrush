#include "Struct/ShaderStructs.hlsli"
#include "Struct/VideoShaderStructs.hlsli"

v2f main(VS_INPUT input)
{
    v2f output;
    output.Pos = mul(input.Pos, FB_CamPosition);
    output.Pos = mul(output.Pos, FB_ToView);
    output.Pos = mul(output.Pos, FB_ToProjection);
    output.Tex = input.Tex;
    output.Color = input.Color;
    return output;
}

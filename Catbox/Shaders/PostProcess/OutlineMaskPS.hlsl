#include "../Struct/ShaderStructs.hlsli"
#include "../Struct/PostProcessingShaderStructs.hlsli"

Texture2D outlineMask : register(t0);

PostProcessPixelOutput main(VertexToPixel input)
{
    PostProcessPixelOutput output;
    output.Color = float4(OB_id, 0, 0, 0);
    return output;
}
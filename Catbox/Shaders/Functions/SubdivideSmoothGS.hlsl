#include "../Struct/ShaderStructs.hlsli"

[maxvertexcount(3)]
void main(triangle SimpleGeometryInput input[3], inout TriangleStream<SimpleGeometryInput> Output)
{
    
    // Subdivide first triangle
    SimpleGeometryInput output1 = (SimpleGeometryInput) 0;
    output1.Position = (input[0].Position + input[1].Position) / 2;
    Output.Append(output1);

    SimpleGeometryInput output2 = (SimpleGeometryInput) 0;
    output2.Position = (input[1].Position + input[2].Position) / 2;
    Output.Append(output2);

    SimpleGeometryInput output3 = (SimpleGeometryInput) 0;
    output3.Position = (input[2].Position + input[0].Position) / 2;
    Output.Append(output3);

    Output.Append(input[0]);
    Output.Append(output1);
    Output.Append(output3);

    Output.Append(output1);
    Output.Append(input[1]);
    Output.Append(output2);

    Output.Append(output3);
    Output.Append(output2);
    Output.Append(input[2]);

    
    
    
    
    
    // Subdivide second triangle
    
    
    /*
    const float2 offsets[4] =
    {
        { -1,  1 },
        {  1,  1 },
        { -1, -1 },
        {  1, -1 }
    };

    const float2 uvs[4] =
    {
        { 0.f, 0.f },
        { 1.f, 0.f },
        { 0.f, 1.f },
        { 1.f, 1.f }
    };
    */
    /*
    const SimpleVertexInput inputVertex = input[0];
    for (unsigned int index = 0; index < 3; ++index)
    {
        SimpleGeometryInput result;
        result.Position = inputVertex.Position;
        result.Position.xy += offsets[index] * OB_Scale.xy * 0.1f;
        result.Position = result.Position;
        result.UV = uvs[index];
        output.Append(result);
    }
    */
}
#include "../../Catbox/Shaders/Struct/ShaderStructs.hlsli"

cbuffer PaintingData : register(b10)
{
	int xPos;
	int yPos;
	int radius;
	float padding;
};


PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	for (int x = xPos - radius; x < xPos + radius; x++)
	{
		for (int y = yPos - radius; y < yPos + radius; x++)
		{
			//pixel at x, y should be colored black
		}
	}

	result.Color.rgba = float4(1,0,0,1);
	result.id = OB_id;
	return result;
}
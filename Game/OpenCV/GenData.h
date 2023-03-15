#pragma once

class Texture;
class GenData
{
public:
	static void GenerateData();
	static BrushSymbol GetSymbol(ID3D11Texture2D* aTexture, int width, int height);
};
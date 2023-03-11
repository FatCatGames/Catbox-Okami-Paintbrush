#pragma once

class Texture;
class GenData
{
public:
	static void GenerateData();
	static void GetSymbol(ID3D11Texture2D* aTexture, int width, int height);
};
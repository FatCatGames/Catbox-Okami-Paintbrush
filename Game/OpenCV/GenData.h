#pragma once

class Texture;
class GenData
{
public:
	static void GenerateData();
	static std::string GetSymbol(ID3D11Texture2D* aTexture, int width, int height);
};
#pragma once

class Texture;
class GenData
{
public:
	//static void GenerateDatakNN();
	static void GenerateDataSVM();
	static void GetSymbol(BrushSymbol& anOutSymbol, ID3D11Texture2D* aTexture, int width, int height, const std::function<void(bool aSucceeded)>& aCallback);
};
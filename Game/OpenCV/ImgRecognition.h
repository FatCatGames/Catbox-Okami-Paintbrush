#pragma once

class Texture;
class ImgRecognition
{
public:
	static void GenerateDatakNN();
	static void GenerateDataSVM();
	static void GetSymbolSVM(BrushSymbol& anOutSymbol, ID3D11Texture2D* aTexture, int width, int height, const std::function<void(bool aSucceeded)>& aCallback);
	static void GetSymbolkNN(BrushSymbol& anOutSymbol, ID3D11Texture2D* aTexture, int width, int height, const std::function<void(bool aSucceeded)>& aCallback);
};
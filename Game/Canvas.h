#pragma once
#include "ComponentTools\Event.h"

class CanvasPS;
class Canvas : public Component
{
public:
	static Canvas* GetInstance() { return Instance; }
	Canvas();
	void Awake() override;
	void Paint(int anXPos, int anYPos, int aRadius, float aRadiusModifier, const Color& aColor);
	void Clear();
	Texture& GetPaintingTex();
	Texture& GetScreenTex() { return myScreenTex; }
	Texture& GetPaperTex() { return *myPaperTex; }
	void Save();
	void ImageRecognitionCallback(bool aSucceeded);
	void Generate();
	void StartPainting();
	bool GetCanPaint() { return myCanPaint; }

private:
	static Canvas* Instance;
	std::shared_ptr<PixelShader> myCanvasPS;

	CanvasPS* myShader;
	bool myCanPaint = false;
	std::shared_ptr<Texture> myPaperTex;
	Texture myPaintingTex;
	Texture myPaintingDisplayTex;
	Texture myScreenTex;
	//Texture myStagingTex;
	const int myWidth = 1920;
	const int myHeight = 1080;
	BrushSymbol mySymbol;
};
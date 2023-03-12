#pragma once
#include "ComponentTools\Event.h"

class CanvasPS;
class Canvas : public Component
{
public:
	static Canvas* GetInstance() { return Instance; }
	Canvas();
	~Canvas();
	void Awake() override;
	void Paint(int anXPos, int anYPos, int aRadius, const Color& aColor);
	void Clear();
	Texture& GetPaintingTex() { return myPaintingTex; }
	Texture& GetScreenTex() { return myScreenTex; }
	Texture& GetPaperTex() { return *myPaperTex; }
	void Save();
	void Generate();
	void StartPainting();
	void Render();

private:
	static Canvas* Instance;
	std::shared_ptr<PixelShader> myCanvasPS;

	CanvasPS* myShader;
	bool myIsPainting = false;
	std::shared_ptr<Texture> myPaperTex;
	Texture myPaintingTex;
	Texture myScreenTex;
	//Texture myStagingTex;
	const int myWidth = 1920;
	const int myHeight = 1080;
	Listener myPostRenderListener;
};
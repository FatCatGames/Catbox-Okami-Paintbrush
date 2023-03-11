#pragma once

class CanvasPS;
class Canvas : public Component
{
public:
	static Canvas* GetInstance() { return Instance; }
	Canvas();
	void Awake() override;
	void Paint(int anXPos, int anYPos, int aRadius, const Color& aColor);
	void Clear();
	Texture& GetTexture() { return myPaintingTex; }
	void Save();
	void Generate();

private:
	static Canvas* Instance;
	CanvasPS* myShader;
	Texture myPaintingTex;
	//Texture myStagingTex;
	const int myWidth = 1920;
	const int myHeight = 1080;
};
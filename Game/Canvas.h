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

private:
	static Canvas* Instance;
	CanvasPS* myShader;
};
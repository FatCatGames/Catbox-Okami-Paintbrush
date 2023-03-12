#pragma once

class PaintingScene : public Component
{
public:
	static PaintingScene* GetInstance() { return Instance; }
	PaintingScene();

private:
	static PaintingScene* Instance;

};
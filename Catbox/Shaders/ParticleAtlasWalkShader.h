#pragma once
#include "Assets\Shader.h"

class ParticleAtlasWalkShader : public PixelShader
{
public:
	struct ParticleAtlasWalkBufferData
	{
		int Rows = 8;
		int Columns = 8;
		int Sprite = 1;  //Sets start of range in sheet
		int NoSprites = 18; //Sets end of range in sheet
	} myParticleAtlasWalkData;

	ParticleAtlasWalkShader();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;
	void SaveData() override;
	void LoadData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<ParticleAtlasWalkBufferData> myParticleAtlasWalkDataBuffer;
};
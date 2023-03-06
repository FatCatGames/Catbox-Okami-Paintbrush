#pragma once
#include "Assets\Shader.h"

class FadeoutShader : public PixelShader
{
public:
	struct FadeoutBufferData
	{
		float lifeTime = 0;
		float ySpeed = 0;
		alignas(4) bool isFrac = false;
		alignas(4) bool isAccelerate = false;
		/*float startDelay = 5;
		float stopDelay = 500;*/

		//alignas(4) bool loop = true;
		//Vector2i padding;
	} myFadeoutData;

	FadeoutShader();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;
	void SaveData() override;
	void LoadData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<FadeoutBufferData> myFadeoutDataBuffer;
};
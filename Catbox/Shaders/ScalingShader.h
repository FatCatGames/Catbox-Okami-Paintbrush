#pragma once
#include "Assets\Shader.h"

class ScalingShader : public VertexShader
{
public:
	struct ScalingBufferData
	{
		float xScale = 1;
		float yScale = 1;
		float zScale = 1;
		alignas(4) bool isSmoothStop = false;
		alignas(4) bool isSmoothStart = false;
		alignas(4) bool isBellCurve = false;
		Vector2f padding;
		/*float startDelay = 5;
		float stopDelay = 500;*/

		//alignas(4) bool loop = true;
		//Vector2i padding;
	} myScalingData;

	ScalingShader();
	std::shared_ptr<VertexShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;
	void SaveData() override;
	void LoadData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<ScalingBufferData> myScalingBufferData;
}; 
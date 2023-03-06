#pragma once
#include "stdafx.h"
#include "Components/UI/UISprite.h"
#include "Assets/video_texture.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Material.h"

class VideoPlayer : public Component
{
public:
	enum VideoState 
	{
		Playing = 0,
		Stopped, 
		Paused, 
		NotStarted,
		COUNT
	};

	//void Init() override;
	//void Awake() override;

	//void Update() override;
	//void RunInEditor() override;

	//void ResetVideo();

	//void RenderInProperties() override;

	//void Play();
	//void Pause();
	//void Stop();

	VideoState GetVideoState() const { return myVideoState; };

	//void ShouldLoop(bool aFlag);

	//void OpenVideo(std::string aPath);

	//void Save(rapidjson::Value& aComponentData) override;
	//void Load(rapidjson::Value& aComponentData) override;

private:
	VideoState myVideoState = VideoState::NotStarted;
	/*VideoTexture myVideoTexture;
	std::shared_ptr<PixelShader> myPixelShader;
	bool myEditorUpdate = false;
	bool myShouldLoop = false;
	bool myCanSkip = true;
	std::string myPlayOrPause;
	std::string myVideoName = "Undefined";
	int myVideoNumber = 0;
	float myVideoAbortTimer = 0.1f;*/
};
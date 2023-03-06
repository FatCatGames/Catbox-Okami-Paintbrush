#include "stdafx.h"
#include "VideoPlayer.h"
//#include "../Game/GameManager.h"
//
//void VideoPlayer::Init()
//{
//	if (this->GetGameObject().GetComponent<UISprite>())
//	{
//		this->GetGameObject().GetComponent<UISprite>()->SetEnabled(false);
//	}
//}
//
//void VideoPlayer::Awake()
//{
//	if (myVideoState == VideoState::NotStarted || myVideoState == VideoState::Stopped)
//	{
//		VideoTexture::createAPI();
//		if (myVideoTexture.internal_data)
//		{
//			myVideoTexture.destroy();
//		}
//		if (!myVideoTexture.create(&myVideoName[0]))
//		{
//			myVideoName = "Undefined";
//			return;
//		}
//		if (!this->GetGameObject().GetComponent<UISprite>())
//		{
//			this->GetGameObject().AddComponent<UISprite>();
//			this->GetGameObject().GetComponent<UISprite>()->SetPixelShader(AssetRegistry::GetInstance()->GetAsset<PixelShader>("VideoPS"));
//
//			this->GetTransform()->SetLocalScale(1920, 1080, 1);
//		}
//		else if (!this->GetGameObject().GetComponent<UISprite>()->IsEnabled())
//		{
//			this->GetGameObject().GetComponent<UISprite>()->SetEnabled(true);
//		}
//	}
//	if (myVideoName != "Undefined")
//	{
//		Play();
//	}
//}
//
//void VideoPlayer::Update()
//{
//	myVideoAbortTimer -= deltaTime;
//	if ((Input::GetKeyPress(KeyCode::ESCAPE) && myVideoAbortTimer < 0.0f && !UIManager::GetInstance()->IntroIsDone()) && myCanSkip)
//	{
//		GameManager::GetInstance()->GetAudio().StopAllSound(false);
//		GameManager::GetInstance()->GetAudio().PlayMusic(AudioManager::eMusic::MainMenu);
//		myVideoState = VideoState::Stopped;
//		this->GetGameObject().GetComponent<UISprite>()->SetEnabled(false);
//		UIManager::GetInstance()->SetIsInIntro(false);
//		GameObject* vp = InstantiatePrefab("VideoPlayerMenu");
//		vp->GetComponent<VideoPlayer>()->Awake();
//		vp->GetComponent<UISprite>()->SetLayer(-5);
//		UIManager::GetInstance()->SetIntroIsDone();
//	}
//	if (myVideoTexture.internal_data != nullptr)
//	{
//		if (myVideoState == VideoState::Playing)
//		{
//			if (myVideoNumber == 1 && UIManager::GetInstance()->IntroIsDone())
//			{
//				myVideoState = VideoState::Stopped;
//				GameManager::GetInstance()->GetAudio().StopAllSound(false);
//				GameManager::GetInstance()->GetAudio().PlayMusic(AudioManager::eMusic::MainMenu);
//				UIManager::GetInstance()->ActivateAllMenuButtons();
//				this->GetGameObject().GetComponent<UISprite>()->SetEnabled(false);
//				GameObject* vp = InstantiatePrefab("VideoPlayerMenu");
//				vp->GetComponent<VideoPlayer>()->Awake();
//				vp->GetComponent<UISprite>()->SetLayer(-5);
//			}
//			if (myVideoTexture.hasFinished())
//			{
//				if(myShouldLoop == false)
//				{
//					myVideoState = VideoState::Stopped;
//					this->GetGameObject().GetComponent<UISprite>()->SetEnabled(false);	
//					if (myVideoNumber == 1)//If intro movie stoped playing
//					{
//						UIManager::GetInstance()->SetIsInIntro(false);
//						GameObject* vp = InstantiatePrefab("VideoPlayerMenu");
//						vp->GetComponent<VideoPlayer>()->Awake();
//						vp->GetComponent<UISprite>()->SetLayer(-5);
//						UIManager::GetInstance()->SetIntroIsDone();
//						GameManager::GetInstance()->GetAudio().StopAllSound(false);
//						GameManager::GetInstance()->GetAudio().PlaySoundEffect(AudioManager::eSoundEffects::MainMenu);
//					}
//				}
//				else
//				{
//					if (myVideoNumber == 3)//If Outro movie stoped playing //This is a Shity FiX!!!
//					{
//						myVideoState = VideoState::Stopped;
//					}
//					else
//					{
//						Play();
//					}
//				}
//			}
//			else
//			{
//				if (this->GetGameObject().GetComponent<UISprite>())
//				{
//					myVideoTexture.update(Engine::GetInstance()->GetDeltaTime());
//					std::shared_ptr<Texture> frame = std::make_shared<Texture>(*myVideoTexture.getTexture());
//					this->GetGameObject().GetComponent<UISprite>()->SetTexture(frame);
//
//				}
//			}
//		}
//	}
//	else
//	{
//		printerror("No video set or video does not exist");
//	}
//}
//
//void VideoPlayer::RunInEditor()
//{
//	if (myEditorUpdate)
//	{
//		Update();
//	}
//}
//
//void VideoPlayer::ResetVideo()
//{
//	myVideoTexture.ResetVideo();
//}
//
//void VideoPlayer::RenderInProperties()
//{
//	if (myVideoState != VideoState::Paused)
//	{
//		myPlayOrPause = "Play";
//	}
//	else
//	{
//		myPlayOrPause = "Resume";
//	}
//
//	if (ImGui::Button(&myPlayOrPause[0]))
//	{
//		myEditorUpdate = true;
//		Play();
//	}
//	if (ImGui::Button("Pause") && myVideoState == VideoState::Playing)
//	{
//		Pause();
//	}
//	if (ImGui::Button("Stop") && (myVideoState == VideoState::Playing || myVideoState == VideoState::Paused))
//	{
//		Stop();
//	}
//	if (ImGui::Checkbox("Should Loop", &myShouldLoop))
//	{
//		if (myVideoState == VideoState::Playing || myVideoState == VideoState::Paused)
//		{
//			ShouldLoop(myShouldLoop);
//		}
//	}
//	ImGui::Checkbox("Able to skip", &myCanSkip);
//	ImGui::InputText("VideoName", &myVideoName[0], 20);
//}
//
//void VideoPlayer::Play()
//{
//	if (myVideoTexture.internal_data != nullptr)
//	{
//		if (myVideoName == "IntroCutscene.MOV")
//			myVideoNumber = 1;
//		else if (myVideoName == "MainMenuScene.mp4")
//			myVideoNumber = 0;
//		else if (myVideoName == "OutroCutscene.MOV")
//			myVideoNumber = 3;
//
//		switch (myVideoNumber)
//		{
//		case 0:
//		    //NoSound		
//			break;
//		case 1:
//			GameManager::GetInstance()->GetAudio().PlaySoundEffect(AudioManager::eSoundEffects::IntroScene);
//			break;
//		case 2:
//			GameManager::GetInstance()->GetAudio().PlaySoundEffect(AudioManager::eSoundEffects::MainMenu);
//			break;
//		case 3:
//			
//			break;
//		default:
//			break;
//		}
//		ShouldLoop(myShouldLoop);
//		if (myVideoState != VideoState::Paused)
//		{
//			ResetVideo();
//		}
//		myVideoTexture.resume();
//		myVideoState = VideoState::Playing;
//	}
//	else
//	{
//		Awake();
//	}
//}
//
//void VideoPlayer::Pause()
//{
//	if (myVideoTexture.internal_data != nullptr)
//	{
//		myVideoState = VideoState::Paused;
//		myVideoTexture.pause();
//	}
//}
//
//void VideoPlayer::Stop()
//{
//	if (myVideoTexture.internal_data != nullptr)
//	{
//		myVideoState = VideoState::Stopped;
//		myVideoTexture.destroy();
//		this->GetGameObject().GetComponent<UISprite>()->SetEnabled(false);
//	}
//}
//
//void VideoPlayer::ShouldLoop(bool aFlag)
//{
//	myVideoTexture.ShouldLoop(aFlag);
//}
//
//void VideoPlayer::OpenVideo(std::string aPath)
//{
//	myVideoTexture.create(aPath.c_str());
//}
//
//void VideoPlayer::Save(rapidjson::Value& /*aComponentData*/)
//{
//	auto& wrapper = *RapidJsonWrapper::GetInstance();
//
//	wrapper.SaveValue<DataType::Bool>("Loop", myShouldLoop);
//	wrapper.SaveValue<DataType::Bool>("Skip", myCanSkip);
//	wrapper.SaveString("VideoName", myVideoName.c_str());
//}
//
//void VideoPlayer::Load(rapidjson::Value& aComponentData)
//{
//	if (aComponentData.HasMember("Loop"))
//	{
//		myShouldLoop = aComponentData["Loop"].GetBool();
//	}
//	if (aComponentData.HasMember("Skip"))
//	{
//		myCanSkip = aComponentData["Skip"].GetBool();
//	}
//	if (aComponentData.HasMember("VideoName"))
//	{
//		myVideoName = aComponentData["VideoName"].GetString();
//	}
//}

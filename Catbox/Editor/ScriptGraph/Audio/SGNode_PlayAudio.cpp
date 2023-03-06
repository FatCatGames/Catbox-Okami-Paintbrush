#include "stdafx.h"
#include "SGNode_PlayAudio.h"
#include "Audio/AudioManager.h"

void SGNode_PlayAudio::Init()
{
	SetTitle("Play Audio");
	SetCategory("Audio");
	CreateExecPin("OutPut", PinDirection::Output, true);
	CreateExecPin("Input", PinDirection::Input, true);

	CreateDataPin<std::string>("Audio", PinDirection::Input);
}

size_t SGNode_PlayAudio::DoOperation()
{
	std::string aData = "";
	GetPinData<std::string>("Audio", aData);
	for  (auto& audio : Engine::GetInstance()->GetAudioManager()->GetMonoList())
	{
		if (audio.myShortName == aData) 
		{
			std::string aAudioPath = audio.myEventPath;
			Engine::GetInstance()->GetAudioManager()->PlaySoundEffectByFullPath(aAudioPath);
			break;
		}
	}
	return ExitViaPin("OutPut");
}
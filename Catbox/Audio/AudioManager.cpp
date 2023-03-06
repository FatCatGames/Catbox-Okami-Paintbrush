#pragma once
#include "stdafx.h"
#include "AudioManager.h"
#include <string>

AudioManager::AudioManager()
{
	Init();
}

AudioManager::~AudioManager()
{
}

void AudioManager::Init()
{
	SoundEngine::Init("Assets/Audio");
	SoundEngine::LoadBank("Master.strings.bank", 0);
	SoundEngine::LoadBank("Master.bank", 0);

	std::vector<std::string> events;
	SoundEngine::GetEvents(events);

	std::sort(events.begin(), events.end());

	char delimiter = '/';

	for (size_t i = 0; i < events.size(); i++)
	{
		std::string type = events[i].substr(0, events[i].find(delimiter));
		if (type == "event:")
		{
			SoundAsset temp;
			temp.myEventPath = events[i];
			int eventDepth = std::count(events[i].begin(), events[i].end(), delimiter);
			int lastPos = events[i].find_last_of(delimiter);
			int firstPos = events[i].find_first_of(delimiter) + 1;
			int secondPos = events[i].find_first_of(delimiter, firstPos);
			temp.myShortName = events[i].substr(lastPos + 1);
			temp.myFirstFolder = events[i].substr(firstPos, secondPos - firstPos);
			if (eventDepth > 2)
			{
				int secondLastPos = events[i].find_last_of(delimiter, lastPos - 1);
				temp.myShortName = events[i].substr(secondLastPos + 1);
			}
			if (SoundEngine::GetImpl()->Is3DSound(events[i]))
			{				
				temp.myId = my3DEvents.size();
				my3DEvents.push_back(temp);
			}
			else
			{
				temp.myId = myEvents.size();
				myEvents.push_back(temp);
			}
		}
	}

	SetSFXVolume(mySFXVolume);
	SetMusicVolume(myMusicVolume);
}

void AudioManager::Update()
{
	SoundEngine::Update();
}

void AudioManager::StopAllSound(bool aInstant)
{
	if (aInstant)
	{
		SoundEngine::StopAllEvents("bus:/", FMOD_STUDIO_STOP_IMMEDIATE);
	}
	else
	{
		SoundEngine::StopAllEvents("bus:/", FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}
}

void AudioManager::PlaySoundEffectByID(int aSoundEffect)
{
	if (aSoundEffect < 0 || aSoundEffect >= myEvents.size())
	{
		printerror("AudioManager::PlaySoundEffectById() aSoundEffect was out of range");
		return;
	}
	SoundEngine::GetImpl()->PlayEventOneShot(myEvents[aSoundEffect].myEventPath);
}

void AudioManager::PlaySoundEffectByFullPath(std::string& aFullPath)
{
	SoundEngine::GetImpl()->PlayEventOneShot(aFullPath);
}

SoundEventInstanceHandle AudioManager::CreateEventInstance(std::string& aFullPath)
{
	return SoundEngine::GetImpl()->CreateEventInstance(aFullPath);
}

SoundEventInstanceHandle AudioManager::CreateEventInstanceByNormalID(int anID)
{
	return SoundEngine::GetImpl()->CreateEventInstance(myEvents[anID].myEventPath);
}

SoundEventInstanceHandle AudioManager::CreateEventInstanceBy3DID(int anID)
{
	return SoundEngine::GetImpl()->CreateEventInstance(my3DEvents[anID].myEventPath);
}

void AudioManager::PlayEvent(const SoundEventInstanceHandle& anEventInstanceHandle)
{
	SoundEngine::PlayEvent(anEventInstanceHandle);
}

void AudioManager::StopEvent(SoundEventInstanceHandle& anEventInstanceHandle, bool anInstantFlag)
{
	SoundEngine::StopEvent(anEventInstanceHandle, anInstantFlag);
}

void AudioManager::ReleaseEvent(SoundEventInstanceHandle& anEventInstanceHandle)
{
	SoundEngine::EventRelease(anEventInstanceHandle);
}

int AudioManager::GetAmountOfParameters(SoundEventInstanceHandle& anEventInstanceHandle)
{
	return SoundEngine::GetImpl()->GetAmountOfParameters(anEventInstanceHandle.GetEvent());
}

int AudioManager::GetAmountOfParameters(SoundAsset& aSoundAsset)
{
	return SoundEngine::GetImpl()->GetAmountOfParameters(aSoundAsset.myEventPath);
}

std::string AudioManager::GetParameterNameByIndex(SoundEventInstanceHandle& anEventInstanceHandle, int anIndex)
{
	return SoundEngine::GetImpl()->GetParametersNameFromIndex(anEventInstanceHandle.GetEvent(), anIndex);
}

void AudioManager::SetParameterByIndex(SoundEventInstanceHandle& anEventInstanceHandle, int anIndex, float aValue)
{
	SetParameterByName(anEventInstanceHandle, GetParameterNameByIndex(anEventInstanceHandle, anIndex), aValue);
}

void AudioManager::SetParameterByName(SoundEventInstanceHandle& anEventInstanceHandle, const std::string& aParameterName, float aValue)
{
	if (!SoundEngine::GetImpl()->SetEventParameter(anEventInstanceHandle, aParameterName, aValue))
	{
		printerror("Could not set " + aParameterName + " on " + anEventInstanceHandle.GetEvent() + ". Event is either wrong or the event is global and must be triggered by the correct function");
	}
}

void AudioManager::SetGlobalParameter(const std::string& aGlobalParameter, float aValue)
{
	SoundEngine::GetImpl()->SetGlobalParameter(aGlobalParameter, aValue);
}

void AudioManager::PlayMusic(int aSoundEffect)
{
	if (aSoundEffect != myCurrentSongID)
	{
		if (myCurrentSongID != -1)
		{
			StopMusic();
		}

		ourCurrentMusic = CreateEventInstance(myEvents[aSoundEffect].myEventPath);
		SoundEngine::PlayEvent(ourCurrentMusic);

		myCurrentSongID = aSoundEffect;
	}
}

void AudioManager::StopMusic()
{
	SoundEngine::StopEvent(ourCurrentMusic, FMOD_STUDIO_STOP_IMMEDIATE);
	SoundEngine::EventRelease(ourCurrentMusic);
	myCurrentSongID = -1;
}

bool AudioManager::SetListenerPosition(Vector3f aPos, Vector3f aForward)
{
	aForward.y = 0;
	aForward.Normalize();

	std::array<float, 3> aPosit = { aPos.x, aPos.y, aPos.z };
	std::array<float, 3> afor = { aForward.x, aForward.y, aForward.z };

	auto theImpl = SoundEngine::GetImpl();
	std::array<float, 3> pos, vel, fwd, up;

	//Listener 0 always exists by default. No need to create an extra one because that would result in duplicate sounds at position 0,0,0
	if (theImpl->GetListenerTransform(0, pos, vel, fwd, up))
	{
		return theImpl->SetListenerAttributes(0, aPosit, vel, afor, up);
	}
	return false;
}

void AudioManager::Set3DSoundPosition(SoundEventInstanceHandle& aHandle, Vector3f aPos)
{
	SoundEngine::SetEvent3DParameters(aHandle, { aPos.x, aPos.y, aPos.z });
}

const std::vector<AudioManager::SoundAsset>& AudioManager::Get3DList()
{
	return my3DEvents;
}

AudioManager::SoundAsset* AudioManager::Get3DSoundAssetByPath(const std::string& aFullPath)
{
	if (aFullPath == "Nothing")
	{
		return nullptr;
	}

	for (size_t i = 0; i < my3DEvents.size(); i++)
	{
		if (aFullPath == my3DEvents[i].myEventPath)
			return &my3DEvents[i];
	}

	printerror("Audio3DComponent could not find sound effect: " + aFullPath);
	return nullptr;
}

const std::vector<AudioManager::SoundAsset>& AudioManager::GetMonoList()
{
	return myEvents;
}

AudioManager::SoundAsset* AudioManager::GetMonoSoundAssetByPath(const std::string& aFullPath)
{
	if (aFullPath == "Nothing")
	{
		return nullptr;
	}

	for (size_t i = 0; i < myEvents.size(); i++)
	{
		if (aFullPath == myEvents[i].myEventPath)
			return &myEvents[i];
	}

	printerror("AudioMonoComponent could not find sound effect: " + aFullPath);
	return nullptr;
}

void AudioManager::SetSFXVolume(int aValue)
{
	mySFXVolume = Catbox::Clamp(aValue, 0, 10);
	SoundEngine::SetBusVolume(mySFXBus, mySFXVolume / 10.f);
	SoundEngine::SetBusVolume(myUIBus, mySFXVolume / 10.f);
	//SoundEngine::SetBusVolume("bus:/", myMasterVolume / 9.f); // original line. 
}

int AudioManager::GetSFXVolume()
{
	return mySFXVolume;
}

void AudioManager::SetMusicVolume(int aValue)
{
	myMusicVolume = Catbox::Clamp(aValue, 0,10);
	SoundEngine::SetBusVolume(myMusicBus, myMusicVolume / 10.f);
}

int AudioManager::GetMusicVolume()
{
	return myMusicVolume;
}

//void AudioManager::CreateListener()
//{
//	return;
//	//yes, this function is pointless and will never be used in a game that only needs one listener
//	auto theImpl = SoundEngine::GetImpl();
//	const int numListeners = theImpl->GetNumListeners(); // We always have at least one listener. Don't create more without a good reason.
//	if (numListeners < 8) //Max listeners is 8
//	{
//		SoundEngine::Listener newListener;
//		newListener.FmodId = numListeners;
//		newListener.ListenerId = ++SoundEngine::nextListenerId;
//
//		assert(theImpl->AddListener(newListener.Location, newListener.Velocity, newListener.Forward, newListener.Up));
//		SoundEngine::myListeners[newListener.ListenerId] = newListener;
//	}
//}

void AudioManager::SetSoundPausedState(bool aFlag)
{
	SoundPaused = aFlag;
	SoundEngine::GetImpl()->SetBusPaused("bus:/SFX (sidechain source)", SoundPaused);
}

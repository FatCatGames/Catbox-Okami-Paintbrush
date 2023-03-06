#pragma once
#include "../../External/fmod/SoundEngine.h"
#include "../../External/fmod/SoundEngine.hpp"
#include <random>
#include <vector>
#include <unordered_map>

class AudioManager
{
public:

	struct SoundAsset
	{
		std::string myEventPath;
		std::string myShortName;
		std::string myFirstFolder;
		size_t myId = -1;
	};

	AudioManager();
	~AudioManager();

	void Init();
	void Update();

	//Stops all sound. If true it just cuts off and if false it fades out.
	void StopAllSound(bool aInstant = true);

	//Oneshots. Press Play then lose all control of the sound.
	
	//Plays a non-spatial sound.
	void PlaySoundEffectByID(int aSoundEffect);
	//Plays a non-spatial sound or a 3D sound at (0, 0, 0).
	void PlaySoundEffectByFullPath(std::string& aFullPath);

	//Sounds that needs to be controlled.
	
	//Creates a SoundEventInstanceHandle regardless of 3D or not.
	SoundEventInstanceHandle CreateEventInstance(std::string& aFullPath);
	//Creates a SoundEventInstanceHandle from non-3D sound ID.
	SoundEventInstanceHandle CreateEventInstanceByNormalID(int anID);
	//Creates a SoundEventInstanceHandle from 3D sound ID.
	SoundEventInstanceHandle CreateEventInstanceBy3DID(int anID);
	//Play registered SoundEvent through SoundEventInstanceHandle
	void PlayEvent(const SoundEventInstanceHandle& anEventInstanceHandle);
	//Stops sound from event. True on the bool results in sound being instantly killed while false results in a fade out.
	void StopEvent(SoundEventInstanceHandle& anEventInstanceHandle, bool aInstantStopFlag = true);
	//Release SoundEventInstanceHandle from SoundEngine. Please call this when before destroying a SoundEventInstanceHandle.
	void ReleaseEvent(SoundEventInstanceHandle& anEventInstanceHandle);

	//Edit Parameter of a sound effect. All Parameter functions are currently untested.
	int GetAmountOfParameters(SoundEventInstanceHandle& anEventInstanceHandle);
	int GetAmountOfParameters(SoundAsset& anEventInstanceHandle);
	std::string GetParameterNameByIndex(SoundEventInstanceHandle& anEventInstanceHandle, int anIndex);
	void SetParameterByIndex(SoundEventInstanceHandle& anEventInstanceHandle, int anIndex, float aValue);
	void SetParameterByName(SoundEventInstanceHandle& anEventInstanceHandle, const std::string& aParameterName, float aValue);

	//Sets a global parameter that can affect all sounds. Will not go back to default on its own.
	void SetGlobalParameter(const std::string& aGlobalParameter, float aValue);

	//Plays a sound that will stop if PlayMusic is called with a different sound. Expected usage is to play music with a looping song.
	void PlayMusic(int aSoundEffect);
	//Stops the sound triggered through PlayMusic().
	void StopMusic();

	//Set the position where 3D sound will listen from and a direction it is looking. (Camera/Player position + Camera.Forward).
	bool SetListenerPosition(Vector3f aPos, Vector3f aforward);
	//Moves the location of a 3D sound by using the SoundEventInstanceHandle as an ID.
	void Set3DSoundPosition(SoundEventInstanceHandle& aHandle, Vector3f aPos);

	//A list to use in the Audio3DComponent.
	const std::vector<SoundAsset>& Get3DList();
	SoundAsset* Get3DSoundAssetByPath(const std::string& aFullPath);
	//A non 3D (Mono) sound list to use in the AudioMonoComponent
	const std::vector<SoundAsset>& GetMonoList();
	SoundAsset* GetMonoSoundAssetByPath(const std::string& aFullPath);

	//Set Volume between 0 - 9 (9 was Henriks request. Feel free to change)
	void SetSFXVolume(int aValue);
	int GetSFXVolume();
	void SetMusicVolume(int aValue);
	int GetMusicVolume();

	//Just in case Busses are named differently in the soundbank
	void SetNewMusicBusPath(std::string& aNewBusPath) { myMusicBus = aNewBusPath; }
	void SetNewSFXBusPath(std::string& aNewBusPath) { mySFXBus = aNewBusPath; }
	void SetNewUIBusPath(std::string& aNewBusPath) { myUIBus = aNewBusPath; }

	//Set if in-game sound effects should pause. Does not pause UI sounds.
	void SetSoundPausedState(bool aFlag);

private:
	int mySFXVolume = 4;
	int myMusicVolume = 4;
	int myCurrentSongID = -1;
	SoundEventInstanceHandle ourCurrentMusic;
	std::string myMusicBus = "bus:/Music";
	std::string mySFXBus = "bus:/SFX";
	std::string myUIBus = "bus:/UI";
	std::vector<SoundAsset> my3DEvents;
	std::vector<SoundAsset> myEvents;
	bool SoundPaused = false;
};
#pragma once

#define AUDIOMANAGER MainSingleton::GetInstance()->GetAudioManager()

//EVENTS
#define EVENT_TestSound "event:/DEBUG/2D_Debug"
#define EVENT_Test3DSound "event:/DEBUG/3D_Debug"

#define EVENT_Music_MainMusic "event:/MUSIC/MainMusic"

#define EVENT_Player_Run "event:/SFX/Player/Run"
#define EVENT_Player_Dash "event:/SFX/Player/Dash"
#define EVENT_Player_Attack "event:/SFX/Player/Attack"
#define EVENT_Player_Hit "event:/SFX/Player/Hit"

#define EVENT_UI_ButtonBack "event:/SFX/UI/ButtonBack"
#define EVENT_UI_ButtonClick "event:/SFX/UI/ButtonClick"
#define EVENT_UI_SliderChange "event:/SFX/UI/SliderChange"

#define EVENT_Enemy_Flight "event:/SFX/Enemy/Bee/Flight"
#define EVENT_Enemy_Attack "event:/SFX/Enemy/Bee/Flight" // måste bytas till

//BUSSES
#define BUS_MASTER "bus:/Master"
#define BUS_SFX "bus:/Master/SFX"
#define BUS_MUSIC "bus:/Master/Music"

//PARAMETERS
#define PARAMETER_Debug_Volume "parameter:/Volume"
#define PARAMETER_Debug_3DVolume "parameter:/Volume3D"

//GLOBAL PARAMETERS
#define GLOBALPARAMETER_Debug_GlobalPitch "parameter:/GlobalPitch"

//SNAPSHOTS
#define SNAPSHOT_Cave_Reverb "snapshot:/Cave_Reverb"
#define SNAPSHOT_Pause "snapshot:/Pause"
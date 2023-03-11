project "Catbox"
	location "."
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	files {
		"**.h",
		"**.cpp",
		"**.hpp",
		"**.hlsl",
		"**.hlsli"
	}

	targetdir "../Bin"
	pchheader "stdafx.h"
	pchsource "stdafx.cpp"


	includedirs 
	{
		"Components/",
		"CommonUtilities/",
		"../External/",
		"../External/ImGui/",
		"../External/DDSImporter/",
		"../External/FbxImporter",
		"../MuninGraph/ScriptGraph",
		"../MuninGraph/",
		"../External/physX/",
		".",
		"../External/Recast & Detour/DebugUtils", 
		"../External/Recast & Detour/Detour",
		"../External/Recast & Detour/Recast",
		"../External/Recast & Detour/DetourCrowd",
		"../External/Recast & Detour/DetourTileCache"
	}

	links
	{
		"External.lib",
"rpcrt4.lib",
"MuninGraph.lib"
	}



filter { "files:**.hlsl" }
   flags "ExcludeFromBuild"
   shadermodel "5.0"
   shaderobjectfileoutput "../Bin/Resources/BuiltIn/Shaders/%{file.basename}.cso"
filter { "files:**PS.hlsl" }
   removeflags "ExcludeFromBuild"
   shadertype "Pixel"
   shaderentry "main"
filter { "files:**VS.hlsl" }
   removeflags "ExcludeFromBuild"
   shadertype "Vertex"
   shaderentry "main"
filter { "files:**GS.hlsl" }
   removeflags "ExcludeFromBuild"
   shadertype "Geometry"
   shaderentry "main"



filter "configurations:Debug"
	targetdir "../Output/Debug"
	defines "_DEBUG"
	staticruntime "off"
	runtime "Debug"
	symbols "on"
	links
	{
		"TGAFBX.lib",
		"SoundEngine-FMod-Debug.lib",
		"LowLevel_static_64.lib",
		"LowLevelAABB_static_64.lib",
		"LowLevelDynamics_static_64.lib",
		"PhysX_64.lib",
		"PhysXCharacterKinematic_static_64.lib",
		"PhysXCommon_64.lib",
		"PhysXCooking_64.lib",
		"PhysXExtensions_static_64.lib",
		"PhysXFoundation_64.lib",
		"PhysXPvdSDK_static_64.lib",
		"PhysXTask_static_64.lib",
		"PhysXVehicle_static_64.lib",
		"SceneQuery_static_64.lib",
		"SimulationController_static_64.lib",
		"Recast.lib",
		"Detour.lib",
		"DetourTileCache.lib",
		"DebugUtils.lib",
		"opencv_world470d.lib"
	}
	libdirs
	{
		"../External/Lib/Debug",
		"../MuninGraph/Lib/Debug"
	}


filter "configurations:Release"
	targetdir "../Bin"
	defines "_RELEASE"
	staticruntime "off"
	runtime "Release"
	defines {"NDEBUG"}
	links
	{
		"TGAFBX.lib",
		"SoundEngine-FMod-Release.lib",
		"LowLevel_static_64.lib",
		"LowLevelAABB_static_64.lib",
		"LowLevelDynamics_static_64.lib",
		"PhysX_64.lib",
		"PhysXCharacterKinematic_static_64.lib",
		"PhysXCommon_64.lib",
		"PhysXCooking_64.lib",
		"PhysXExtensions_static_64.lib",
		"PhysXFoundation_64.lib",
		"PhysXPvdSDK_static_64.lib",
		"PhysXTask_static_64.lib",
		"PhysXVehicle_static_64.lib",
		"SceneQuery_static_64.lib",
		"SimulationController_static_64.lib",		
		"Recast.lib",
		"Detour.lib",
		"DetourTileCache.lib",
		"DebugUtils.lib",
		"opencv_world470.lib"
	}
	libdirs
	{
		"../External/Lib/Release",
		"../MuninGraph/Lib/Release"
	}

filter "configurations:Game"
	targetdir "../Bin"
	defines "_GAME"
	staticruntime "off"
	runtime "Release"
	defines {"NDEBUG"}
	links
	{
		"TGAFBX.lib",
		"SoundEngine-FMod-Release.lib",
		"LowLevel_static_64.lib",
		"LowLevelAABB_static_64.lib",
		"LowLevelDynamics_static_64.lib",
		"PhysX_64.lib",
		"PhysXCharacterKinematic_static_64.lib",
		"PhysXCommon_64.lib",
		"PhysXCooking_64.lib",
		"PhysXExtensions_static_64.lib",
		"PhysXFoundation_64.lib",
		"PhysXPvdSDK_static_64.lib",
		"PhysXTask_static_64.lib",
		"PhysXVehicle_static_64.lib",
		"SceneQuery_static_64.lib",
		"SimulationController_static_64.lib",		
		"Recast.lib",
		"Detour.lib",
		"DetourTileCache.lib",
		"DebugUtils.lib",
		"opencv_world470.lib"
	}
	libdirs
	{
		"../External/Lib/Game",
		"../MuninGraph/Lib/Game"
	}
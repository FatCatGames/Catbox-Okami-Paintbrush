project "Game"
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

libdirs { 
		"../Lib/Debug/",
		"$(projectdir)lib/",
		"../External/Lib",
		}

	links
	{
	"d3d11.lib",
	"dxguid.lib",
	"Catbox"
	}

	includedirs {
		"../Catbox/",
		"../External/physX/",
		"../External/",
		"../Game/",
		"../External/Recast & Detour/DebugUtils", 
		"../External/Recast & Detour/Detour",
		"../External/Recast & Detour/Recast",
		"../External/Recast & Detour/DetourCrowd",
		"../External/Recast & Detour/DetourTileCache"
	}

	pchheader "Game.pch.h"
	pchsource "Game.pch.cpp"

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

filter "configurations:Release"
	targetdir "../Bin"
	defines "_RELEASE"
	defines {"NDEBUG"}
	staticruntime "off"
	runtime "Release"

filter "configurations:Game"
	targetdir "../Bin"
	defines "_GAME"
	defines {"NDEBUG"}
	staticruntime "off"
	runtime "Release"


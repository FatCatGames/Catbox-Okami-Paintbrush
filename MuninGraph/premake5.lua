project "MuninGraph"
	location "."
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir "Lib"
	pchheader "MuninGraph.pch.h"
	pchsource "MuninGraph.pch.cpp"

	files {
		"**.h",
		"**.cpp",
		"**.hpp"
	}

	includedirs
	{
		"../External/Imgui",
		"Graph",
		"ScriptGraph",
		"../MuninGraph/",
		"."
	}
	

filter "configurations:Debug"
	targetdir "Lib/Debug"
	staticruntime "off"
	runtime "Debug"

filter "configurations:Release"
	targetdir "Lib/Release"
	staticruntime "off"
	runtime "Release"

filter "configurations:Game"
	targetdir "Lib/Game"
	staticruntime "off"
	runtime "Release"
project "CatNet"
	location "."
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	files {
		"**.h",
		"**.cpp",
		"**.hpp"
	}

	libdirs { "../Lib/Debug/" }
	targetdir "../Bin"
	targetname "CatNet"

	links
	{
		"ws2_32.lib"
	}

	filter { 'system:console' }

filter "configurations:Debug"
	targetdir "../Output/Debug"
	staticruntime "off"
	runtime "Debug"
	targetsuffix "-Debug"

filter "configurations:Release"
	targetdir "../Bin"
	staticruntime "off"
	runtime "Release"
	targetsuffix ""

filter "configurations:Game"
	targetdir "../Bin"
	staticruntime "off"
	runtime "Release"
	targetsuffix ""

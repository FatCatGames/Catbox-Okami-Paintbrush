project "External"
	location "."
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir "Lib"

	files {
		"**.h",
		"**.cpp",
		"**.hpp"
	}

	includedirs
	{
		"Imgui",
		"rapidjson",
		"../CatNet/",
		"Recast & Detour/Detour"
	}

	links
	{
		"ws2_32.lib"
	}
	
filter "configurations:Debug"
	targetdir "Lib/Debug"
	staticruntime "off"
	runtime "Debug"

filter "configurations:Release"
	targetdir "Lib/Release"
	staticruntime "off"
	runtime "Release"
	defines {"NDEBUG"}

filter "configurations:Game"
	targetdir "Lib/Game"
	staticruntime "off"
	runtime "Release"
	defines {"NDEBUG"}

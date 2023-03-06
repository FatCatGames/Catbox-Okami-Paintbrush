project "Launcher"
	location "."
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"

	
	files {
		"**.h",
		"**.cpp",
		"**.hpp"
	}

	libdirs { "../Lib/Debug/" }
	targetdir "../Bin"
	debugdir "../Bin"
	targetname "CatboxLauncher"
	filter { 'system:windows' }
	files { 'Launcher.rc', '**.ico' }
	vpaths { ['Launcher/*'] = { '*.rc', '**.ico' }}
  	filter {}

	links
	{
	"Catbox",
	"Game",
	"d3d11.lib",
	"dxguid.lib",
	"dbghelp.lib"
	}

	includedirs {
		"../Catbox/",
		"../Catbox/Graphics/Rendering/DX11"
	}


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
	targetname "Eraticate"
	defines "_GAME"
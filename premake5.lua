include "Premake/extensions.lua"

workspace "Catbox"
	location "."
	startproject "Launcher"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Game"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

targetdir "../Bin/"

include "Launcher"
include "Catbox"
include "Game"
include "External"
include "MuninGraph"
include "CatNet"
#pragma once


#ifndef STDAFX
#define STDAFX


#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "../External/ImGui/imgui.h"
#include "CommonUtilities/Vector.h"
#include "Engine.h"
#include "Editor\Editor.h"
#include "Graphics\Rendering\GraphicsEngine.h"
#include "Debugging\DebugDrawer.h"
#include "GameObjects/Component.h"
#include "GameObjects/GameObject.h"
#include "Assets/AssetRegistry.h"
#include "Assets/Prefab.h"
#include "Editor\Debugging\MessageHandler.h"
#include "CommonUtilities/ImGuiWrapper.h"

#define deltaTime Engine::GetInstance()->GetDeltaTime()
#define EDITORMODE Engine::GetInstance()->IsEditorMode()
#define PLAYMODE Engine::GetInstance()->IsGameRunning()
#define print(msg) MessageHandler::debugMessages.AddMessage(msg)
#define printerror(msg) MessageHandler::debugMessages.errors.push_back(msg)

#endif
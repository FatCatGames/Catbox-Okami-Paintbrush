#include "stdafx.h"
#include "DebuggingWindow.h"
#include "Graphics\Rendering\Renderers\ForwardRenderer.h"

void DebuggingWindow::Render()
{
	if (!BeginWindow("Debug Rendering", true)) return;


	int debugMode = (int)myDebugMode;
	const char* items[] = { "Default", "Albedo", "Vertex colors", "Vertex normals", "Normal map", "UVs", "Ambient occlusion", "Roughness", "Metallic", "Emissive", "Wireframe"};
	if (ImGui::Combo("Render Mode", &debugMode, items, IM_ARRAYSIZE(items)))
	{
		myDebugMode = (DebugRenderMode)debugMode;
		Engine::GetInstance()->GetGraphicsEngine()->GetForwardRenderer().SetDebugRenderMode(debugMode);
	}

	EndWindow();
}
#pragma once
#include "EditorWindow.h"

class DebuggingWindow : public EditorWindow
{
public:
	enum class DebugRenderMode
	{
		NoDebug,
		Albedo,
		VertexColors,
		VertexNormals,
		NormalMap,
		UVs
	};

	void Render() override;

private:
	DebugRenderMode myDebugMode;
};


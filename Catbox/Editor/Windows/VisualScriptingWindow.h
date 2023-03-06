#pragma once
#include "Editor/Windows/EditorWindow.h"
#include "Editor/ScriptGraph/ScriptGraphEditorBase.h"

class Blueprint;
class VisualScriptingWindow : public EditorWindow
{
public:
private:
	void Render() override;
	bool myShouldRender = false;
};


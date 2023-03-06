#pragma once
#include "Editor/Windows/EditorWindow.h"
#include "Editor/ScriptGraph/ScriptGraphEditorBase.h"


class AnimationControllerWindow : public EditorWindow
{
public:
private:
	void Render() override;
	bool myShouldRender = false;
};


#pragma once
#include "EditorWindow.h"

class ConsoleWindow : public EditorWindow
{
public:


private:
	void Render() override;
	bool myCollapseToggle = true;
};


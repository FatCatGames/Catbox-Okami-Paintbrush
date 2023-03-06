#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_DebugText)
{
public:

	void Init() override;
	size_t DoOperation() override;

	ImVec4 GetNodeHeaderColor() const override;
};

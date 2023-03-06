#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_MathAdd)
{
public:

	void Init() override;

	size_t DoOperation() override;
};

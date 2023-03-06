#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_Vec3fSplitter)
{
public:
	void Init();
	size_t DoOperation() override;
};


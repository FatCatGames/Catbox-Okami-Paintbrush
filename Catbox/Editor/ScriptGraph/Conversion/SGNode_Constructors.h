#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_Vec3fConstructor)
{
public:
	void Init();
	size_t DoOperation() override;
};


#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_CreateGameObject)
{
public:
	void Init();
	size_t DoOperation() override;
};


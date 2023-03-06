#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"


BeginScriptGraphNode(SGNode_GetKeyPress)
{
public:
	void Init() override;

	size_t DoOperation() override;
};



BeginScriptGraphNode(SGNode_GetKeyHeld)
{
public:
	void Init() override;

	size_t DoOperation() override;
};


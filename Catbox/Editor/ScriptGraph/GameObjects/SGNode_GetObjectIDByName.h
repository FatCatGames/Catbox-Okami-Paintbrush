#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_GetObjectIDByName)
{
public:
	void Init();
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_GetChildByName)
{
public:
	void Init();
	size_t DoOperation() override;
};
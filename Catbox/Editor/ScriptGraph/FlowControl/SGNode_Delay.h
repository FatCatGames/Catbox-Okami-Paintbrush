#pragma once
#include "MuninGraph.h"
#include "ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_Delay)
{
public:
	void Init() override;

	size_t DoOperation() override;
private:
};


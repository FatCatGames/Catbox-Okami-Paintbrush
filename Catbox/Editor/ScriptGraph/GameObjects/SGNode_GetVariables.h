#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"

#define CreateVariableNode(T) BeginScriptGraphNode(T) { public: void Init() override; size_t DoOperation() override; };

CreateVariableNode(SGNode_GetBool)
CreateVariableNode(SGNode_GetInt)
CreateVariableNode(SGNode_GetFloat)
CreateVariableNode(SGNode_GetString)
CreateVariableNode(SGNode_GetVector3)
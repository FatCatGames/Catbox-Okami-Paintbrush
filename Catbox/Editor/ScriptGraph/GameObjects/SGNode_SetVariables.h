#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"

#define CreateVariableNode(T) BeginScriptGraphNode(T) { public: void Init() override; size_t DoOperation() override; };

CreateVariableNode(SGNode_SetBool)
CreateVariableNode(SGNode_SetInt)
CreateVariableNode(SGNode_SetFloat)
CreateVariableNode(SGNode_SetString)
CreateVariableNode(SGNode_SetVector3)
#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"


BeginScriptGraphNode(SGNode_Vec3Add)
{
public:
	void Init() override;
	size_t DoOperation() override;

};

BeginScriptGraphNode(SGNode_Vec3Multiply)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_Vec3ScalarMultiply)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_Vec3Subtract)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_Vec3Divide)
{
public:
	void Init() override;
	size_t DoOperation() override;
};


BeginScriptGraphNode(SGNode_MathLerpVec3)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_MathAdd)
{
public:
	void Init() override;
	size_t DoOperation() override;

};

BeginScriptGraphNode(SGNode_MathMultiply)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_MathSubtract)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_MathDivide)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_MathRemainder)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_MathCos)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_MathSin)
{
public:
	void Init() override;
	size_t DoOperation() override;
};
BeginScriptGraphNode(SGNode_MathAbs)
{
public:
	void Init() override;
	size_t DoOperation() override;
};

BeginScriptGraphNode(SGNode_MathAtan2)
{
public:
	void Init() override;
	size_t DoOperation() override;
};
BeginScriptGraphNode(SGNode_MathMin)
{
public:
	void Init() override;
	size_t DoOperation() override;
};
BeginScriptGraphNode(SGNode_MathMax)
{
public:
	void Init() override;
	size_t DoOperation() override;
};
BeginScriptGraphNode(SGNode_MathDistance2D)
{
public:
	void Init() override;
	size_t DoOperation() override;
};


#pragma once
#include "MuninScriptGraph.h"
#include "SGNode_Conversions.h"

BeginScriptGraphNode(SGNode_FloatToString)
{
public:

	void Init() override;

	size_t DoOperation() override;
	FORCEINLINE bool IsSimpleNode() const override { return true; }
};


BeginScriptGraphNode(SGNode_Vector3fToString)
{
public:

	void Init() override;

	size_t DoOperation() override;
	FORCEINLINE bool IsSimpleNode() const override { return true; }
};
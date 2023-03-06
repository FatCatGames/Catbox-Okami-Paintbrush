#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

BeginScriptGraphNode(SGNode_EventBeginPlay)
{
public:

	void Init() override;

	[[nodiscard]] ImVec4 GetNodeHeaderColor() const override;

	size_t DoOperation() override;

	FORCEINLINE bool IsEntryNode() const override { return true; }
	FORCEINLINE bool IsInternalOnly() const override { return false; }
	FORCEINLINE unsigned MaxInstancesPerGraph() const override { return 1; }
};

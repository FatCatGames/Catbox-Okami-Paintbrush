#pragma once
#include "MuninScriptGraph.h"
#include "ScriptGraph/ScriptGraphNode.h"



class SG_BaseCollisionNode : public ScriptGraphNode
{
public:
	[[nodiscard]] ImVec4 GetNodeHeaderColor() const override { return ImVec4(255, 0, 0, 128); };
	void Init() override {};
	inline size_t DoOperation() override { return ExitViaPin("Out"); }

	FORCEINLINE bool IsEntryNode() const override { return true; }
	FORCEINLINE bool IsInternalOnly() const override { return false; }
	FORCEINLINE unsigned MaxInstancesPerGraph() const override { return 1; }
};

#define BeginCollisionNode(T) class T : public SG_BaseCollisionNode, public RegisterScriptNode<T>, public Munin::ObjectUUID<T>{public:void Init() override;};


BeginCollisionNode(SGNode_OnCollisionEnter)
BeginCollisionNode(SGNode_OnCollisionStay)
BeginCollisionNode(SGNode_OnCollisionExit)

BeginCollisionNode(SGNode_OnOverlapBegin)
BeginCollisionNode(SGNode_OnOverlap)
BeginCollisionNode(SGNode_OnOverlapEnd)
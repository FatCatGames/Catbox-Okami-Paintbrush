#pragma once

#include "ScriptGraph/ScriptGraphNode.h"
#include "ScriptGraph/ScriptGraphTypes.h"

struct ScriptGraphVariable;

class VariableNodeBase
{
	friend class ScriptGraphSchema;
protected:
	std::shared_ptr<ScriptGraphVariable> myVariable;

public:

	virtual ~VariableNodeBase() = default;

	virtual void SetNodeVariable(const std::shared_ptr<ScriptGraphVariable>& aVariable) = 0;
};

BeginScriptGraphNode(SGNode_SetVariable), public VariableNodeBase
{

public:

	void Init() override;

	void SetNodeVariable(const std::shared_ptr<ScriptGraphVariable>& aVariable) override;
	size_t DoOperation() override;

	ImVec4 GetNodeHeaderColor() const override;

	bool IsInternalOnly() const override { return true; }
};

BeginScriptGraphNode(SGNode_GetVariable), public VariableNodeBase
{
public:

	void Init() override;

	void SetNodeVariable(const std::shared_ptr<ScriptGraphVariable>& aVariable) override;
	size_t DoOperation() override;
	ImVec4 GetNodeHeaderColor() const override;

	bool IsInternalOnly() const override { return true; }
};
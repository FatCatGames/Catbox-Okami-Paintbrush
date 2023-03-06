#pragma once
#include "ScriptGraphPin.h"
#include "../Graph/NodeGraphNode.h"
#include "ScriptGraph.h"
#include "ScriptGraphSchema.h"

class ScriptGraph;

struct ScriptGraphNodePayload
{
	friend class ScriptGraphNode;

private:

	std::unordered_map<std::string, ScriptGraphDataObject> Data;

public:

	template<typename T>
	void SetVariable(const std::string& aString, const T& aValue)
	{
		if (const auto it = Data.find(aString); it != Data.end())
		{
			if (it->second.TypeData->GetType() != typeid(T))
			{
				it->second = ScriptGraphDataObject::Create<T>();
			}
			it->second.SetData(aValue);
		}
		else
		{
			Data.insert({ aString, ScriptGraphDataObject::Create(aValue) });
		}
	}

	template<typename T>
	bool TryGetVariable(const std::string& aString, T& outValue) const
	{
		if (const auto it = Data.find(aString); it != Data.end())
		{
			if (it->second.TypeData->GetType() == typeid(T))
			{
				outValue = it->second.GetData<T>();
				return true;
			}
		}

		return false;
	}
};


class ScriptGraphNode : public NodeGraphNode<ScriptGraphPin, ScriptGraph, ScriptGraphSchema>, public std::enable_shared_from_this<ScriptGraphNode>
{
	typedef NodeGraphNode<ScriptGraphPin, ScriptGraph, ScriptGraphSchema> ParentClass;

	bool isExecNode = false;

	std::string myDescription;
	std::string myCategory = "Default";
	std::string myAcessiblity = "Public";
	std::string myErrorMessage;
	ImVec4 aPos;
	bool hasErrored = false;

protected:

	template<typename DataType>
	void CreateDataPin(const std::string& aLabel, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		AddPin(ScriptGraphPin::CreateDataPin<DataType>(AsSharedPtr(), aLabel, ScriptGraphPinType::Data, PinIcon::Circle, aDirection, hideLabelOnNode));
	}

	void CreateVariablePin(const std::string& aLabel, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		AddPin(ScriptGraphPin::CreatePin(AsSharedPtr(), aLabel, ScriptGraphPinType::Variable, PinIcon::Circle, aDirection, hideLabelOnNode));
	}

	void CreateExecPin(const std::string& aLabel, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		AddPin(ScriptGraphPin::CreatePin(AsSharedPtr(), aLabel, ScriptGraphPinType::Exec, PinIcon::Exec, aDirection, hideLabelOnNode));
		isExecNode = true;
	}

	size_t ExitViaPin(const std::string& aPinLabel) override;

	size_t ExitWithError(const std::string& anErrorMessage);

	void SetDescription(const std::string& aDescription);
	void SetCategory(const std::string& aCategory);
	void SetAcessibility(const std::string& aAcess);

	std::shared_ptr<ScriptGraphNode> AsSharedPtr() { return shared_from_this(); }

public:

	virtual void Init() = 0;
	virtual size_t Exit() override;

	/**
	 * Called when this node is asked to Execute. This will ONLY happen if the node has at least one Exec Input Pin.
	 * By default this function will call DoOperation on the node but you may modify this behavior as you please.
	 * @param anEntryPinUID The Pin that caused this node to Execute (i.e. an Input Exec pin we have). If we have no input Exec pins this will be 0!
	 * @returns The Pin Index of the pin we want to exit on after performing our operation or 0 if we don't have a pin to exit on.
	 */
	virtual size_t Exec(size_t anEntryPinUID);

	virtual void DeliverPayload(const ScriptGraphNodePayload& aPayload);

	virtual ~ScriptGraphNode() override = default;

	FORCEINLINE bool IsExecNode() const { return isExecNode; }
	FORCEINLINE virtual bool IsEntryNode() const { return false; }
	FORCEINLINE virtual bool IsInternalOnly() const { return false; }
	FORCEINLINE virtual bool IsSimpleNode() const { return false; }

	FORCEINLINE const std::string& GetDescription() const { return myDescription; }
	FORCEINLINE const std::string& GetCategory() const { return myCategory; }
	FORCEINLINE const std::string& GetAcessibility() const { return myAcessiblity; }

	FORCEINLINE ImVec4 GetNodeHeaderColor() const override { return ImVec4(80, 124, 153, 255); };

	// Controls how many instances of this node may coexist in the same graph.
	// If > 0 this will be enforced.
	FORCEINLINE virtual unsigned MaxInstancesPerGraph() const { return 0; }

	FORCEINLINE bool HasError() const { return hasErrored; }
	FORCEINLINE const std::string& GetErrorMessage() const { return myErrorMessage; }

	int myObjectID = -1;
};

template<typename N>
struct RegisterScriptNode
{
	static inline bool IsRegistered = ScriptGraphSchema::RegisterNodeType<N>();
};

#define BeginScriptGraphNode(T) class T : public ScriptGraphNode, public RegisterScriptNode<T>, public Munin::ObjectUUID<T>
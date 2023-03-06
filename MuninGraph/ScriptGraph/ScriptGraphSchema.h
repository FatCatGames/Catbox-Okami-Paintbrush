#pragma once
#include <functional>

#include "../Graph/NodeGraphSchema.h"
#include "ScriptGraph.h"
#include "ScriptGraphTypes.h"
#include "ScriptGraphVariable.h"

class SGNode_SetVariable;
class SGNode_GetVariable;
class ScriptGraph;
class ScriptGraphNode;
class ScriptGraphPin;

struct ScriptGraphNodeType
{
	std::type_index Type = typeid(std::nullptr_t);
	std::string TypeName;
	std::function<std::shared_ptr<ScriptGraphNode>()> New;
	std::shared_ptr<ScriptGraphNode> DefaultObject;

private:
	static void SetupNodeType( ScriptGraphNodeType& aNodeType);

public:

	template<typename T>
	static ScriptGraphNodeType Create()
	{
		ScriptGraphNodeType result;
		result.Type = typeid(T);
		result.New = []() { auto ptr = std::make_shared<T>(); ptr->Init(); return ptr; };
		SetupNodeType(result);
		return result;
	}
};

class ScriptGraphSchema : public NodeGraphSchema
{
	std::shared_ptr<ScriptGraph> myGraph;
	std::unordered_map<std::string, unsigned> myNodeTypeCounts;
	std::vector<std::string> myGraphEntryPoints;

	// Prevents static init order fiasco from happening.
	// NOTE: This could potentially happen with registered types as well.
	static std::unordered_map<std::string, const ScriptGraphNodeType>& MyNodeTypesMap()
	{
		static std::unordered_map<std::string, const ScriptGraphNodeType> myMap;
		return myMap;
	}

	static std::unordered_map<std::string, std::string>& MyNodeNameToTypeNameMap()
	{
		static std::unordered_map<std::string, std::string> myMap;
		return myMap;
	}

	static std::unordered_map<std::string, const ScriptGraphNodeType> RegisterNodeTypes();
	static std::unordered_map<std::string, std::string> GetNodeNames();
	static std::shared_ptr<ScriptGraph> CreateScriptGraphInternal(bool createEmpty);

	// To avoid including ScriptGraphNode.h
	static bool RegisterNodeTypeInternal(const ScriptGraphNodeType& type);

public:

	template<typename N>
	static bool RegisterNodeType()
	{
		ScriptGraphNodeType type = ScriptGraphNodeType::Create<N>();
		auto It = MyNodeTypesMap().insert({ type.TypeName, std::move(type) });
		return RegisterNodeTypeInternal(It.first->second);
	}

	static std::shared_ptr<ScriptGraph> CreateScriptGraph();
	static bool SerializeScriptGraph(const std::shared_ptr<ScriptGraph>& aGraph, std::string& outResult);
	static bool DeserializeScriptGraph(std::shared_ptr<ScriptGraph>& outGraph, const std::string& inData, bool editorMode);

	#pragma warning(push)
	#pragma warning(disable: 4172)
	template<typename C>
	static const ScriptGraphNodeType& GetNodeTypeByClass()
	{
		const std::type_index classType = typeid(C);
		for(const auto& [nodeTypeName, nodeType] : MyNodeTypesMap())
		{
			if(nodeType.Type == classType)
			{
				return nodeType;
			}
		}

		assert(false && "Type not found!");
		return ScriptGraphNodeType();
	}
	#pragma warning(pop)

private:

	ScriptGraphPin& GetMutablePin(size_t aPinUID);
	bool RegisterNode(std::shared_ptr<ScriptGraphNode> aNode);
	void RegisterEntryPointNode(std::shared_ptr<ScriptGraphNode> aNode, const std::string& aEntryHandle);
	void CreateEdgeInternal(ScriptGraphPin& aSourcePin, ScriptGraphPin& aTargetPin) const;

	void RegenerateEntryPointList();

public:

	ScriptGraphSchema(const std::shared_ptr<void>& aGraph);

	template<typename T>
	std::shared_ptr<T> AddNode()
	{
		std::shared_ptr<ScriptGraphNode> newNode = std::make_shared<T>();
		if(RegisterNode(newNode))
		{
			return std::dynamic_pointer_cast<T>(newNode);
		}

		return nullptr;
	}

	std::shared_ptr<ScriptGraphNode> AddNode(const std::string& aType);
	std::shared_ptr<ScriptGraphNode> AddNode(const std::shared_ptr<ScriptGraphNode>& aNode);

	template<typename T>
	void AddVariable(const std::string& aVariableName, const T& aDefaultValue = T())
	{
		std::shared_ptr<ScriptGraphVariable> newVariable = std::make_shared<ScriptGraphVariable>();
		newVariable->Data = ScriptGraphDataObject::Create<T>();
		newVariable->DefaultData = ScriptGraphDataObject::Create<T>();
		memcpy_s(newVariable->DefaultData.Ptr, newVariable->DefaultData.TypeData->GetTypeSize(), &aDefaultValue, sizeof(T));
		memcpy_s(newVariable->Data.Ptr, newVariable->DefaultData.TypeData->GetTypeSize(), &aDefaultValue, sizeof(T));
		newVariable->Name = aVariableName;
		myGraph->myVariables.insert({ aVariableName, newVariable });
	}

	void AddVariable(const std::string& aVariableName, const ScriptGraphDataObject& aDataObject)
	{
		std::shared_ptr<ScriptGraphVariable> newVariable = std::make_shared<ScriptGraphVariable>();
		newVariable->Data = aDataObject;
		newVariable->DefaultData = aDataObject;
		newVariable->Name = aVariableName;
		myGraph->myVariables.insert({ aVariableName, newVariable });
	}

	void RemoveNode(size_t aNodeUID);
	void RemoveVariable(const std::string& aVariableName);

	std::shared_ptr<SGNode_GetVariable> AddGetVariableNode(const std::string& aVariableName);
	std::shared_ptr<SGNode_SetVariable> AddSetVariableNode(const std::string& aVariableName);

	template<typename T>
	void RegisterEntryPoint(const std::string& aEntryHandle)
	{
		std::shared_ptr<ScriptGraphNode> newNode = AddNode<T>();
		RegisterEntryPointNode(newNode, aEntryHandle);
	}

	//~ Begin GraphEditorSchema Interface
	bool CanCreateEdge(size_t aSourcePinUID, size_t aTargetPinUID, std::string& outMesssage) const override;
	bool CreateEdge(size_t aSourcePinUID, size_t aTargetPinUID) override;
	bool DisconnectPin(size_t aPinUID) override;
	bool RemoveEdge(size_t aEdgeUID) override;
	//~ End GraphEditorSchema Interface

	FORCEINLINE std::shared_ptr<ScriptGraph> GetGraph() const { return myGraph; }
	FORCEINLINE const std::vector<std::string>& GetEntryPoints() const { return myGraphEntryPoints; }

	FORCEINLINE const std::unordered_map<std::string, std::shared_ptr<ScriptGraphVariable>>& GetVariables() const { return myGraph->myVariables; }

	FORCEINLINE static const std::unordered_map<std::string, const ScriptGraphNodeType>& GetSupportedNodeTypes()
	{
		return MyNodeTypesMap();
	}
};

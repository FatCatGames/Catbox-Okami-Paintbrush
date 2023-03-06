#pragma once
#include <functional>

#include "../Graph/NodeGraphSchema.h"
#include "AnimationGraph.h"
#include "ScriptGraphTypes.h"
#include "ScriptGraphVariable.h"

class Animation;
class AnimationController;
class AnimationGraph;
class AnimationGraphNode;
class AnimationGraphPin;

struct AnimationGraphNodeType
{
	std::type_index Type = typeid(std::nullptr_t);
	std::string TypeName;
	std::function<std::shared_ptr<AnimationGraphNode>()> New;
	std::shared_ptr<AnimationGraphNode> DefaultObject;

private:
	static void SetupNodeType( AnimationGraphNodeType& aNodeType);

public:

};

class AnimationGraphSchema : public NodeGraphSchema
{
	std::shared_ptr<AnimationGraph> myGraph;
	static std::shared_ptr<AnimationGraph> CreateAnimationGraphInternal(AnimationController* aController);

public:

	static std::shared_ptr<AnimationGraph> CreateAnimationGraph(AnimationController* aController);
	static bool SerializeAnimationGraph(AnimationController* controller, std::string& outResult);
	static bool DeserializeAnimationGraph(AnimationController* controller, const std::string& inData);


	void RegisterEntryPointNode(std::shared_ptr<AnimationGraphNode> aNode);
	void RegisterAnyStateNode(AnimationGraphNode* aNode);
	void UnregisterAnyStateNode(AnimationGraphNode* aNode);


private:
	AnimationGraphPin& GetMutablePin(size_t aPinUID);
	bool RegisterNode(std::shared_ptr<AnimationGraphNode> aNode);
	void CreateEdgeInternal(AnimationGraphPin& aSourcePin, AnimationGraphPin& aTargetPin) const;

public:

	AnimationGraphSchema(const std::shared_ptr<void>& aGraph);

	template<typename T>
	std::shared_ptr<T> AddNode()
	{
		std::shared_ptr<AnimationGraphNode> newNode = std::make_shared<T>();
		if(RegisterNode(newNode))
		{
			return std::dynamic_pointer_cast<T>(newNode);
		}

		return nullptr;
	}


	std::shared_ptr<AnimationGraphNode> AddNode(const std::shared_ptr<AnimationGraphNode>& aNode);

	template<typename T>
	void AddVariable(const std::string& aVariableName, const T& aDefaultValue = T())
	{
		std::shared_ptr<AnimationGraphVariable> newVariable = std::make_shared<AnimationGraphVariable>();
		newVariable->Data = AnimationGraphDataObject::Create<T>();
		newVariable->DefaultData = AnimationGraphDataObject::Create<T>();
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
		myGraph->myVariables->insert({ aVariableName, newVariable });
	}

	void RemoveNode(size_t aNodeUID);
	void RemoveVariable(const std::string& aVariableName);

	//~ Begin GraphEditorSchema Interface
	bool CanCreateEdge(size_t aSourcePinUID, size_t aTargetPinUID, std::string& outMesssage) const override;
	bool CreateEdge(size_t aSourcePinUID, size_t aTargetPinUID) override;
	bool DisconnectPin(size_t aPinUID) override;
	bool RemoveEdge(size_t aEdgeUID) override;
	//~ End GraphEditorSchema Interface

	FORCEINLINE std::shared_ptr<AnimationGraph> GetGraph() const { return myGraph; }
	FORCEINLINE const std::unordered_map<std::string, std::shared_ptr<ScriptGraphVariable>>& GetVariables() const { return *myGraph->myVariables; }
};

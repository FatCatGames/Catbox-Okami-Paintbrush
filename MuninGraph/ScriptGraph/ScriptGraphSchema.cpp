#include "MuninGraph.pch.h"
#include "ScriptGraphSchema.h"

#include <fstream>

#include "ScriptGraph.h"
#include "ScriptGraphNode.h"
#include "ScriptGraphTypes.h"

#include "..\External\nlohmann\json.hpp"
#include "ScriptGraphVariable.h"
#include "Nodes/SGNode_Variable.h"
#include "Nodes/Events/SGNode_EventBeginPlay.h"
#include "Nodes/Events/SGNode_EventTick.h"

// Macro for easily registering node types with this Schema.
#define RegisterNodeType(T) { ScriptGraphNodeType type = ScriptGraphNodeType::Create<T>(); nodeTypes.insert({ #T, std::move(type) });}

void ScriptGraphNodeType::SetupNodeType(ScriptGraphNodeType& aNodeType)
{
	if (!aNodeType.DefaultObject)
	{
		aNodeType.DefaultObject = aNodeType.New();
		aNodeType.DefaultObject->Init();
	}
	const auto uuidObj = AsUUIDAwareSharedPtr(aNodeType.DefaultObject);
	aNodeType.TypeName = uuidObj->GetTypeName();
}

std::unordered_map<std::string, const ScriptGraphNodeType> ScriptGraphSchema::RegisterNodeTypes()
{
	std::unordered_map<std::string, const ScriptGraphNodeType> nodeTypes;
	//#include "ScriptGraphNodes.inl"
	return std::move(nodeTypes);
}

std::unordered_map<std::string, std::string> ScriptGraphSchema::GetNodeNames()
{
	std::unordered_map<std::string, std::string> namesToTypes;

	for (const auto& [type, nodeType] : MyNodeTypesMap())
	{
		const std::shared_ptr<ScriptGraphNode> tempNode = nodeType.New();
		if (!tempNode->IsInternalOnly())
		{
			namesToTypes.insert({ tempNode->GetNodeTitle(), type });
		}
	}

	return namesToTypes;
}

std::shared_ptr<ScriptGraph> ScriptGraphSchema::CreateScriptGraphInternal(bool createEmpty)
{
	std::shared_ptr<ScriptGraph> aGraph = std::make_shared<ScriptGraph>();

	if (!createEmpty)
	{
		const std::unique_ptr<ScriptGraphSchema> graphSchema = aGraph->GetGraphSchema();
		graphSchema->RegisterEntryPoint<SGNode_EventBeginPlay>("Begin Play");
		graphSchema->RegisterEntryPoint<SGNode_EventTick>("Tick");
	}

	return aGraph;
}

bool ScriptGraphSchema::RegisterNodeTypeInternal(const ScriptGraphNodeType& type)
{
	const std::shared_ptr<ScriptGraphNode> defaultObject = type.DefaultObject;
	if (!defaultObject->IsInternalOnly())
		MyNodeNameToTypeNameMap().insert({ defaultObject->GetNodeTitle(), type.TypeName });

	return true;
}

std::shared_ptr<ScriptGraph> ScriptGraphSchema::CreateScriptGraph()
{
	return CreateScriptGraphInternal(false);
}

bool ScriptGraphSchema::SerializeScriptGraph(const std::shared_ptr<ScriptGraph>& aGraph, std::string& outResult)
{
	using namespace nlohmann;

	json graphJson;

	graphJson["name"] = "ScriptGraph";

	graphJson["variables"] = json::array();
	for (auto& [varName, variable] : aGraph->myVariables)
	{
		json varJson;
		varJson["name"] = varName;
		varJson["type"] = variable->GetTypeData()->GetTypeName();

		const std::type_index varTypeId = variable->GetTypeData()->GetType();

		std::vector<uint8_t> varData;
		ScriptGraphDataTypeRegistry::Serialize(varTypeId, variable->DefaultData.Ptr, varData);
		varJson["value"] = varData;

		graphJson["variables"].push_back(varJson);
	}

	graphJson["nodes"] = json::array();

	for (const auto& [nodeUID, node] : aGraph->myNodes)
	{
		// UUID keeps track of the type, which is handy.
		const auto uuidAwareBase = AsUUIDAwarePtr(node.get());

		json nodeJson;

		// We save the type of node, the UID (will be unique in the scope of
		// the file we save this graph to, but nowhere else), and the node pins.
		nodeJson["type"] = uuidAwareBase->GetTypeName();
		nodeJson["UID"] = uuidAwareBase->GetUID();
		nodeJson["entryHandle"] = node->IsEntryNode() ? aGraph->myNodeUIDToEntryHandle.find(uuidAwareBase->GetUID())->second : "";

		const ImVec2 nodePos = ImNodeEd::GetNodePosition(uuidAwareBase->GetUID());
		nodeJson["x"] = nodePos.x;
		nodeJson["y"] = nodePos.y;
		const float nodeZ = ImNodeEd::GetNodeZPosition(uuidAwareBase->GetUID());
		nodeJson["z"] = nodeZ;

		if (const std::shared_ptr<VariableNodeBase> variableNode = std::dynamic_pointer_cast<VariableNodeBase>(node))
		{
			nodeJson["variable"] = variableNode->myVariable->Name;
		}

		nodeJson["pins"] = json::array();

		// We only store Data pins since we only need to keep track of data values
		// on them at present.
		for (auto& [pinUID, pin] : node->GetPins())
		{
			if (pin.GetType() == ScriptGraphPinType::Data || pin.GetType() == ScriptGraphPinType::Variable)
			{
				json pinJson;
				pinJson["name"] = pin.GetLabel();

				std::vector<uint8_t> pinData;
				pinData.resize(pin.myData.TypeData->GetTypeSize());
				ScriptGraphDataTypeRegistry::Serialize(pin.GetDataType(), pin.myData.Ptr, pinData);
				pinJson["value"] = pinData;

				nodeJson["pins"].push_back(pinJson);
			}
		}

		graphJson["nodes"].push_back(nodeJson);
	}

	graphJson["edges"] = json::array();

	// For edges we need to know the UID of the involved nodes, same UID we saved
	// in the nodes section above, and we also need to know which pin. Pins are
	// stored using labels on that node making them easy to find.
	for (auto& [edgeUID, edge] : aGraph->myEdges)
	{
		const ScriptGraphPin& sourcePin = aGraph->GetPinFromUID(edge.FromUID);
		const auto sourceNodeUUID = AsUUIDAwareSharedPtr(sourcePin.GetOwner());

		const ScriptGraphPin& targetPin = aGraph->GetPinFromUID(edge.ToUID);
		const auto targetNodeUUID = AsUUIDAwareSharedPtr(targetPin.GetOwner());

		json edgeJson;
		edgeJson["sourceUID"] = sourceNodeUUID->GetUID();
		edgeJson["sourcePin"] = sourcePin.GetLabel();

		edgeJson["targetUID"] = targetNodeUUID->GetUID();
		edgeJson["targetPin"] = targetPin.GetLabel();

		graphJson["edges"].push_back(edgeJson);
	}

	outResult = graphJson.dump();

	return true;
}

bool ScriptGraphSchema::DeserializeScriptGraph(std::shared_ptr<ScriptGraph>& outGraph, const std::string& inData, bool editorMode)
{
	outGraph = CreateScriptGraphInternal(true);
	std::unique_ptr<ScriptGraphSchema> outGraphSchema = outGraph->GetGraphSchema();

	using namespace nlohmann;
	json graphJson = json::parse(inData);

	outGraph->myEdges.clear();
	outGraph->myPins.clear();
	outGraph->myEntryPoints.clear();
	outGraph->myNodeUIDToEntryHandle.clear();
	outGraph->myNodes.clear();
	outGraph->myVariables.clear();

	// Load all the variables
	for (const auto& it : graphJson["variables"])
	{
		const std::string varName = it.at("name");
		const std::string varType = it.at("type");
		const std::vector<uint8_t> varData = it.at("value");

		const std::type_index varTypeId = ScriptGraphDataTypeRegistry::GetTypeFromString(varType);

		std::shared_ptr<ScriptGraphVariable> newVariable = std::make_shared<ScriptGraphVariable>();
		newVariable->Data = ScriptGraphDataTypeRegistry::GetDataObjectOfType(varTypeId);
		newVariable->Name = varName;
		newVariable->DefaultData = ScriptGraphDataTypeRegistry::GetDataObjectOfType(varTypeId);

		ScriptGraphDataTypeRegistry::Deserialize(varTypeId, newVariable->DefaultData.Ptr, varData);
		newVariable->ResetVariable();

		outGraph->myVariables.insert({ varName, newVariable });
	}

	std::unordered_map<size_t, std::shared_ptr<ScriptGraphNode>> fileUIDToNode;

	// Create all the nodes.
	for (const auto& it : graphJson["nodes"])
	{
		const std::string nodeType = it.at("type");
		const auto typeData = MyNodeTypesMap().find(nodeType);
		std::shared_ptr<ScriptGraphNode> newNode = typeData->second.New();

		if (std::shared_ptr<VariableNodeBase> varNode = std::dynamic_pointer_cast<VariableNodeBase>(newNode))
		{
			const std::string nodeVarName = it.at("variable");
			std::shared_ptr<ScriptGraphVariable> nodeVar = outGraph->myVariables.find(nodeVarName)->second;
			varNode->SetNodeVariable(nodeVar);

			for (auto& [pinUID, pin] : newNode->myPins)
			{
				if (pin.GetType() == ScriptGraphPinType::Variable)
				{
					pin.InitVariableBlock(nodeVar->Data.TypeData->GetType());
				}
			}
		}

		for (const auto& pinIt : it.at("pins"))
		{
			const std::string pinName = pinIt.at("name");
			const std::vector<uint8_t> varData = pinIt.at("value");
			if (auto pinLbl = newNode->myPinLabelToUID.find(pinName); pinLbl != newNode->myPinLabelToUID.end())
			{
				if (auto pinObj = newNode->myPins.find(pinLbl->second); pinObj != newNode->myPins.end())
				{
					ScriptGraphDataTypeRegistry::Deserialize(pinObj->second.GetDataType(), pinObj->second.myData.Ptr, varData);
					//memcpy_s(pinObj->second.myData.Ptr, pinObj->second.myData.TypeData->GetTypeSize(), varData.data(), varData.size());
				}
			}
		}

		if (editorMode) 
		{
			const auto uuidAwareBase = AsUUIDAwareSharedPtr(newNode);

			const float x = it.at("x");
			const float y = it.at("y");
			const float z = it.at("z");

			ImNodeEd::SetNodePosition(uuidAwareBase->GetUID(), { x, y });
			ImNodeEd::SetNodeZPosition(uuidAwareBase->GetUID(), z);
		}

		// Here we should read all the pins and restore their values if we can.
		// This gets fun if we have a lot of custom types to account for :).
		// TODO: read constant pin data. Needs to be saved too. Won't work for pointers of course
		// since they're just an integer.

		if (!outGraphSchema->RegisterNode(newNode))
		{
			assert(false && "The graph has invalid numbers of a node!");
		}

		if (newNode->IsEntryNode())
		{
			outGraphSchema->RegisterEntryPointNode(newNode, it.at("entryHandle"));
		}

		fileUIDToNode.insert({ it.at("UID"), newNode });
	}

	// And all the edges
	for (const auto& it : graphJson["edges"])
	{
		const size_t sourceUID = it.at("sourceUID");
		const std::string sourcePinLabel = it.at("sourcePin");
		const size_t targetUID = it.at("targetUID");
		const std::string targetPinLabel = it.at("targetPin");

		// Find the source node owning the source pin.
		if (auto srcIt = fileUIDToNode.find(sourceUID); srcIt != fileUIDToNode.end())
		{
			// And find the target node owning the target pin.
			if (auto tgtIt = fileUIDToNode.find(targetUID); srcIt != fileUIDToNode.end())
			{
				const std::shared_ptr<ScriptGraphNode> sourceNode = srcIt->second;
				const std::shared_ptr<ScriptGraphNode> targetNode = tgtIt->second;

				// Get the actual pins from the nodes.
				const ScriptGraphPin* srcPin = sourceNode->GetPin(sourcePinLabel);
				const ScriptGraphPin* tgtPin = targetNode->GetPin(targetPinLabel);

				if (!srcPin || !tgtPin) continue;

				// And create a link.
				outGraphSchema->CreateEdge(srcPin->GetUID(), tgtPin->GetUID());
			}
		}
	}

	return true;
}

ScriptGraphPin& ScriptGraphSchema::GetMutablePin(size_t aPinUID)
{
	return myGraph->myPins.find(aPinUID)->second;
}

bool ScriptGraphSchema::RegisterNode(std::shared_ptr<ScriptGraphNode> aNode)
{
	assert(aNode && "The requested node is not of the correct base node type!");

	if (aNode->GetPins().empty())
		aNode->Init();

	const auto uidAwareNode = AsUUIDAwarePtr(aNode.get());

	const unsigned maxInstances = aNode->MaxInstancesPerGraph();
	if (maxInstances > 0)
	{
		if (auto countIt = myNodeTypeCounts.find(uidAwareNode->GetTypeName()); countIt != myNodeTypeCounts.end())
		{
			if (maxInstances < countIt->second + 1)
			{
				return false; // No making more copies of this node!
			}

			countIt->second++;
		}
		else
		{
			myNodeTypeCounts.insert({ uidAwareNode->GetTypeName(), 1 });
		}
	}

	aNode->myOwner = myGraph;

	myGraph->myNodes.insert({ uidAwareNode->GetUID(), aNode });

	if (aNode->IsEntryNode())
	{
		RegisterEntryPointNode(aNode, aNode->GetNodeTitle());
	}

	for (auto& [key, value] : aNode->myPins)
	{
		myGraph->myPins.insert({ key, value });
	}

	return true;
}

void ScriptGraphSchema::RegisterEntryPointNode(std::shared_ptr<ScriptGraphNode> aNode, const std::string& aEntryHandle)
{
	myGraph->myEntryPoints.insert({ aEntryHandle, aNode });
	const auto uuidAwareBase = AsUUIDAwarePtr(aNode.get());
	myGraph->myNodeUIDToEntryHandle.insert({ uuidAwareBase->GetUID(), aEntryHandle });
	myGraphEntryPoints.push_back(aEntryHandle);
}

void ScriptGraphSchema::CreateEdgeInternal(ScriptGraphPin& aSourcePin, ScriptGraphPin& aTargetPin) const
{
	const size_t newEdgeUID = myGraph->myNextEdgeId++;
	const float edgeThickness = aSourcePin.GetType() == ScriptGraphPinType::Exec ? 3.0f : 1.0f;

	myGraph->myEdges.insert(
		{
			newEdgeUID,
			{
				newEdgeUID,
				aSourcePin.GetUID(),
				aTargetPin.GetUID(),
				ScriptGraphDataTypeRegistry::GetDataTypeColor(aSourcePin.GetDataType()),
				edgeThickness
			}
		});

	aSourcePin.AddPinEdge(newEdgeUID);
	aTargetPin.AddPinEdge(newEdgeUID);

	//assert(aTargetPin.GetNumConnections() == 1);
	//assert(aSourcePin.GetNumConnections() >= 1);
}

void ScriptGraphSchema::RegenerateEntryPointList()
{
	myGraphEntryPoints.clear();
	for (const auto& [handle, node] : myGraph->myEntryPoints)
	{
		myGraphEntryPoints.push_back(handle);
	}
}

ScriptGraphSchema::ScriptGraphSchema(const std::shared_ptr<void>& aGraph)
{
	myGraph = std::static_pointer_cast<ScriptGraph>(aGraph);
	for (const auto& [nodeUID, node] : myGraph->myNodes)
	{
		const auto uidAwareBase = AsUUIDAwarePtr(node.get());
		if (auto countIt = myNodeTypeCounts.find(uidAwareBase->GetTypeName()); countIt != myNodeTypeCounts.end())
		{
			countIt->second++;
		}
		else
		{
			myNodeTypeCounts.insert({ uidAwareBase->GetTypeName(), 1 });
		}
	}

	RegenerateEntryPointList();
}

std::shared_ptr<ScriptGraphNode> ScriptGraphSchema::AddNode(const std::string& aType)
{
	const auto typeData = MyNodeTypesMap().find(aType);
	std::shared_ptr<ScriptGraphNode> newNode = typeData->second.New();
	if (RegisterNode(newNode))
	{
		return newNode;
	}

	return nullptr;
}

std::shared_ptr<ScriptGraphNode> ScriptGraphSchema::AddNode(const std::shared_ptr<ScriptGraphNode>& aNode)
{
	if (RegisterNode(aNode))
	{
		return aNode;
	}

	return nullptr;
}

void ScriptGraphSchema::RemoveNode(size_t aNodeUID)
{
	if (const auto nodeIt = myGraph->myNodes.find(aNodeUID); nodeIt != myGraph->myNodes.end())
	{
		for (auto& [pinUID, pin] : nodeIt->second->myPins)
		{
			// Copy edges since that list will change.
			const std::vector<size_t> pinEdges = pin.myEdges;
			for (const size_t edgeUID : pinEdges)
			{
				RemoveEdge(edgeUID);
			}

			myGraph->myPins.erase(pinUID);
		}

		if (nodeIt->second->IsEntryNode())
		{
			const auto uuidAwareNode = AsUUIDAwareSharedPtr(nodeIt->second);
			for (auto entryIt = myGraph->myEntryPoints.begin(); entryIt != myGraph->myEntryPoints.end(); ++entryIt)
			{
				const auto uuidAwarePtr = AsUUIDAwareSharedPtr(entryIt->second);
				if (uuidAwarePtr->GetUUID() == uuidAwareNode->GetUUID())
				{
					myGraph->myEntryPoints.erase(entryIt);
					break;
				}
			}

			RegenerateEntryPointList();
		}

		const auto uidAwareNode = AsUUIDAwareSharedPtr(nodeIt->second);
		if (const auto countIt = myNodeTypeCounts.find(uidAwareNode->GetTypeName()); countIt != myNodeTypeCounts.end())
		{
			countIt->second--;
			if (countIt->second <= 0)
			{
				myNodeTypeCounts.erase(countIt);
			}
		}

		myGraph->myNodes.erase(nodeIt);
	}
}

void ScriptGraphSchema::RemoveVariable(const std::string& aVariableName)
{
	// Need to plow through all nodes that use this variable and eat them.
	std::vector<size_t> nodeUIDsToDelete;
	for (const auto& [nodeUID, node] : myGraph->myNodes)
	{
		if (std::shared_ptr<VariableNodeBase> varNode = std::dynamic_pointer_cast<VariableNodeBase>(node))
		{
			if (varNode->myVariable->Name == aVariableName)
				nodeUIDsToDelete.push_back(nodeUID);
		}
	}

	for (const size_t& uid : nodeUIDsToDelete)
	{
		RemoveNode(uid);
	}

	myGraph->myVariables.erase(aVariableName);
}

std::shared_ptr<SGNode_GetVariable> ScriptGraphSchema::AddGetVariableNode(const std::string& aVariableName)
{
	auto varIt = myGraph->myVariables.find(aVariableName);
	assert(varIt != myGraph->myVariables.end() && "No such variable!");

	std::shared_ptr<SGNode_GetVariable> node = std::make_shared<SGNode_GetVariable>();
	node->Init();
	node->SetNodeVariable(varIt->second);

	for (auto& [pinUID, pin] : node->myPins)
	{
		if (pin.GetType() == ScriptGraphPinType::Variable)
		{
			pin.InitVariableBlock(varIt->second->Data.TypeData->GetType());
		}
	}

	RegisterNode(node);

	return node;
}

std::shared_ptr<SGNode_SetVariable> ScriptGraphSchema::AddSetVariableNode(const std::string& aVariableName)
{
	auto varIt = myGraph->myVariables.find(aVariableName);
	assert(varIt != myGraph->myVariables.end() && "No such variable!");

	std::shared_ptr<SGNode_SetVariable> node = std::make_shared<SGNode_SetVariable>();
	node->Init();
	node->SetNodeVariable(varIt->second);

	for (auto& [pinUID, pin] : node->myPins)
	{
		if (pin.GetType() == ScriptGraphPinType::Variable)
		{
			pin.InitVariableBlock(varIt->second->Data.TypeData->GetType());
		}
	}

	RegisterNode(node);

	return node;
}

bool ScriptGraphSchema::CanCreateEdge(size_t aSourcePinUID, size_t aTargetPinUID, std::string& outMesssage) const
{
	if (aSourcePinUID == aTargetPinUID)
	{
		return false;
	}

	const ScriptGraphPin& pinA = myGraph->GetPinFromUID(aSourcePinUID);
	const ScriptGraphPin& pinB = myGraph->GetPinFromUID(aTargetPinUID);

	const ScriptGraphPin& sourcePin = pinA.GetPinDirection() == PinDirection::Output ? pinA : pinB;
	const ScriptGraphPin& targetPin = pinB.GetPinDirection() == PinDirection::Input ? pinB : pinA;

	const auto& sourceOwner = sourcePin.GetOwner();
	const auto sourceUID = AsUUIDAwareSharedPtr(sourceOwner)->GetUID();

	const auto& targetOwner = targetPin.GetOwner();
	const auto targetUID = AsUUIDAwareSharedPtr(targetOwner)->GetUID();

	if (sourceUID == targetUID)
	{
		outMesssage = "Cannot link two pins that are on the same node";
		return false;
	}

	if (sourcePin.GetUID() == targetPin.GetUID())
	{
		outMesssage = "Cannot link a pin to itself.";
		return false;
	}

	if (sourcePin.GetPinDirection() == targetPin.GetPinDirection())
	{
		outMesssage = "Cannot link a pin to one of the same direction.";
		return false;
	}

	if (sourcePin.GetType() == ScriptGraphPinType::Exec && targetPin.GetType() == ScriptGraphPinType::Exec)
	{
		return true;
	}

	if (sourcePin.GetType() != ScriptGraphPinType::Exec && targetPin.GetType() != ScriptGraphPinType::Exec)
	{
		if (sourcePin.GetDataType() != targetPin.GetDataType())
		{
			outMesssage = std::string(sourcePin.GetDataType().name()) + " is not compatible with " + std::string(targetPin.GetDataType().name());
			return false;
		}

		return true;
	}

	return false;
}

bool ScriptGraphSchema::CreateEdge(size_t aSourcePinUID, size_t aTargetPinUID)
{
	std::string outMessage;
	if (CanCreateEdge(aSourcePinUID, aTargetPinUID, outMessage))
	{

		ScriptGraphPin& pinA = GetMutablePin(aSourcePinUID);
		ScriptGraphPin& pinB = GetMutablePin(aTargetPinUID);

		ScriptGraphPin& sourcePin = pinA.GetPinDirection() == PinDirection::Output ? pinA : pinB;
		ScriptGraphPin& targetPin = pinB.GetPinDirection() == PinDirection::Input ? pinB : pinA;

		size_t sourceUID = pinA.GetPinDirection() == PinDirection::Output ? aSourcePinUID : aTargetPinUID;
		size_t targetUID = pinB.GetPinDirection() == PinDirection::Input ? aTargetPinUID : aSourcePinUID;

		// Exec can have only one connection right now. Remove all connections from the pin.
		if (sourcePin.GetType() == ScriptGraphPinType::Exec && sourcePin.IsPinConnected())
		{
			DisconnectPin(sourceUID);
		}

		if (targetPin.IsPinConnected())
		{
			DisconnectPin(targetUID);
		}

		CreateEdgeInternal(sourcePin, targetPin);

		return true;
	}

	return false;
}

bool ScriptGraphSchema::DisconnectPin(size_t aPinUID)
{
	ScriptGraphPin& pin = GetMutablePin(aPinUID);

	const std::vector<size_t> pinEdgeIds = pin.GetEdges();
	for (auto it = pinEdgeIds.begin(); it != pinEdgeIds.end(); ++it)
	{
		RemoveEdge(*it);
	}

	return true;
}

bool ScriptGraphSchema::RemoveEdge(size_t aEdgeUID)
{
	auto it = myGraph->myEdges.find(aEdgeUID);
	assert(it != myGraph->myEdges.end());

	ScriptGraphPin& fromPin = GetMutablePin(it->second.FromUID);
	ScriptGraphPin& toPin = GetMutablePin(it->second.ToUID);

	fromPin.RemovePinEdge(aEdgeUID);
	toPin.RemovePinEdge(aEdgeUID);

	it = myGraph->myEdges.erase(it);

	return true;
}

//void ScriptGraphSchema::DrawPinIcon(const ScriptGraphPin* aPin, ImVec4 aPinColor, bool isConnected) const
//{
//	const PinIcon icon = aPin->GetType() == ScriptGraphPinType::Exec ? PinIcon::Exec : PinIcon::Circle;
//
//	const float iconSize = 24.0f;
//	const ImVec2 sizeRect = ImVec2(iconSize, iconSize);
//	if(ImGui::IsRectVisible(sizeRect))
//	{
//		ImVec2 cursorPos = ImGui::GetCursorPos();
//		ImDrawList* drawList = ImGui::GetWindowDrawList();
//
//		ImRect drawRect = { cursorPos, { cursorPos.x + iconSize, cursorPos.y + iconSize } };
//		const float outlineScale = drawRect.GetWidth() / (iconSize * 2.0f);
//		const int circleExtraSegments = static_cast<int>(round(2 * outlineScale));
//
//		unsigned color = IM_COL32(aPinColor.x, aPinColor.y, aPinColor.z, aPinColor.w);
//
//		const ImColor innerColor(32, 32, 32, 255);
//
//		switch(icon)
//		{
//			case PinIcon::Exec:
//				{
//					const auto origin_scale = drawRect.GetWidth() / iconSize;
//
//					const auto offset_x = 1.0f * origin_scale;
//					const auto offset_y = 0.0f * origin_scale;
//					const auto margin = (isConnected ? 2.0f : 2.0f) * origin_scale;
//					const auto rounding = 0.1f * origin_scale;
//					const auto tip_round = 0.7f;
//
//					const float canvasX = drawRect.GetTL().x + margin + offset_x;
//					const float canvasY = drawRect.GetTL().y + margin + offset_y;
//					const float canvasW = canvasX + drawRect.GetWidth() - margin * 2.0f;
//					const float canvasH = canvasY + drawRect.GetWidth() - margin * 2.0f;
//
//					const auto canvas = ImRect(canvasX, canvasY, canvasW, canvasH);
//
//					const auto left = canvas.GetTL().x + canvas.GetWidth() * 0.5f * 0.3f;
//					const auto right = canvas.GetTL().x + canvas.GetWidth() - canvas.GetWidth() * 0.5f * 0.3f;
//					const auto top = canvas.GetTL().y + canvas.GetHeight() * 0.5f * 0.2f;
//					const auto bottom = canvas.GetTL().y + canvas.GetHeight() - canvas.GetHeight() * 0.5f * 0.2f;
//					const auto center_y = (top + bottom) * 0.5f;
//
//					const auto tip_top = ImVec2(canvas.GetTL().x + canvas.GetWidth() * 0.5f, top);
//					const auto tip_right = ImVec2(right, center_y);
//					const auto tip_bottom = ImVec2(canvas.GetTL().x + canvas.GetWidth() * 0.5f, bottom);
//
//					drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
//					drawList->PathBezierCurveTo(
//						ImVec2(left, top),
//						ImVec2(left, top),
//						ImVec2(left, top) + ImVec2(rounding, 0));
//					drawList->PathLineTo(tip_top);
//					drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
//					drawList->PathBezierCurveTo(
//						tip_right,
//						tip_right,
//						tip_bottom + (tip_right - tip_bottom) * tip_round);
//					drawList->PathLineTo(tip_bottom);
//					drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
//					drawList->PathBezierCurveTo(
//						ImVec2(left, bottom),
//						ImVec2(left, bottom),
//						ImVec2(left, bottom) - ImVec2(0, rounding));
//
//					if (!isConnected)
//					{
//						if (innerColor & 0xFF000000)
//							drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);
//
//						drawList->PathStroke(color, true, 2.0f * outlineScale);
//					}
//					else
//						drawList->PathFillConvex(color);
//				}
//				break;
//			case PinIcon::Circle:
//				{
//					const float triangleStart = (drawRect.GetTL().x + iconSize / 2) * 0.32f * drawRect.GetWidth();
//					drawRect.Min.x -= static_cast<int>(round(drawRect.GetWidth() * 0.25f * 0.25f));
//					const ImVec2 center = { drawRect.GetTL().x + sizeRect.x / 2, drawRect.GetTL().y + sizeRect.y / 2 };
//
//					if (!isConnected)
//					{
//						const auto r = 0.5f * drawRect.GetWidth() / 2.0f - 0.5f;
//
//						if (innerColor & 0xFF000000)
//							drawList->AddCircleFilled(center, r, innerColor, 12 + circleExtraSegments);
//						drawList->AddCircle(center, r, color, 12 + circleExtraSegments, 2.0f * outlineScale);
//					}
//					else
//						drawList->AddCircleFilled(center, 0.5f * drawRect.GetWidth() / 2.0f, color, 12 + circleExtraSegments);
//				}
//				break;
//		}
//	}
//
//	ImGui::Dummy(sizeRect);
//}
//
//void ScriptGraphSchema::RenderNode(const ScriptGraphNode* aNode)
//{
//	std::vector<const ScriptGraphPin*> inputPins;
//	std::vector<const ScriptGraphPin*> outputPins;
//
//	const ImVec2 headerTextSize = ImGui::CalcTextSize(aNode->GetNodeTitle().c_str());
//	ImVec2 leftMinSize = { 64, 0 };
//	ImVec2 rightMinSize = { 64, 0 };
//	for (auto& [uid, pin] : aNode->GetPins())
//	{
//		const ImVec2 currentTextSize = ImGui::CalcTextSize(pin.GetLabel().c_str());
//
//		if (pin.GetPinDirection() == PinDirection::Input)
//		{
//			if (currentTextSize.x > leftMinSize.x)
//				leftMinSize.x = currentTextSize.x;
//			if (currentTextSize.y > leftMinSize.y)
//				leftMinSize.y = currentTextSize.y;
//
//			inputPins.push_back(&pin);
//		}
//		else
//		{
//			if (currentTextSize.x > rightMinSize.x)
//				rightMinSize.x = currentTextSize.x;
//			if (currentTextSize.y > rightMinSize.y)
//				rightMinSize.y = currentTextSize.y;
//
//			outputPins.push_back(&pin);
//		}
//	}
//
//	if (leftMinSize.x > 0)
//		leftMinSize.x += 32;
//
//	if (rightMinSize.x > 0)
//		rightMinSize.x += 32;
//
//	ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodeRounding, 3.0f);
//	ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));
//
//	const auto uidAwareBase = AsUUIDAwarePtr(aNode);
//	ImNodeEd::NodeId currentNodeId = uidAwareBase->GetUID();
//	ImNodeEd::BeginNode(currentNodeId);
//	ImGui::PushID(static_cast<int>(uidAwareBase->GetUID()));
//	ImGui::TextUnformatted(aNode->GetNodeTitle().c_str());
//	const ImVec2 headerRectMin = ImGui::GetItemRectMin();
//	const ImVec2 headerRectMax = ImGui::GetItemRectMax();
//
//	const float bodyMinWidth = leftMinSize.x + rightMinSize.x + 16;
//	const ImVec2 nodeBodySize = { headerTextSize.x > bodyMinWidth ? headerTextSize.x : bodyMinWidth, 0 };
//
//	// TEMP FLIP FLOP TO DEBUG TABLE CELL SIZES
//	//const ImU32 ColA = IM_COL32(128, 0, 0, 128);
//	//const ImU32 ColB = IM_COL32(0, 128, 0, 128);
//
//	ImGui::BeginTable("body", 2, ImGuiTableFlags_SizingStretchProp, nodeBodySize);
//	const size_t numRows = inputPins.size() > outputPins.size() ? inputPins.size() : outputPins.size();
//	for (size_t row = 0; row < numRows; row++)
//	{
//		ImGui::TableNextColumn();
//		//ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, row % 2 ? ColA : ColB);
//		if (row < inputPins.size())
//		{
//			const ScriptGraphPin* inputPin = inputPins[row];
//			RenderPin(inputPin);
//		}
//		ImGui::TableNextColumn();
//		//ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, row % 2 ? ColB : ColA);
//		if (row < outputPins.size())
//		{
//			const ScriptGraphPin* outputPin = outputPins[row];
//			RenderPin(outputPin);
//		}
//		ImGui::TableNextRow();
//	}
//	ImGui::EndTable();
//
//	const ImVec2 NodeBodySize = ImGui::GetItemRectSize();
//	const ImRect headerRect = ImRect(headerRectMin, { headerRectMax.x + NodeBodySize.x - ImGui::CalcTextSize(aNode->GetNodeTitle().c_str()).x, headerRectMax.y + 2 });
//	ImNodeEd::EndNode();
//
//	ImDrawList* nodeDrawList = ImNodeEd::GetNodeBackgroundDrawList(uidAwareBase->GetUID());
//	const float halfBorderWidth = ImNodeEd::GetStyle().NodeBorderWidth * 0.5f;
//
//	nodeDrawList->AddRectFilled(
//		ImVec2(headerRect.GetTL().x - (8 - halfBorderWidth), headerRect.GetTL().y - (4 - halfBorderWidth)),
//		ImVec2(headerRect.GetBR().x + (8 - halfBorderWidth), headerRect.GetBR().y),
//		ImGui::ColorConvertFloat4ToU32(aNode->GetNodeHeaderColor() / 255.0f),
//		ImNodeEd::GetStyle().NodeRounding,
//		1 | 2
//	);
//
//	const ImRect headerSeparatorRect = ImRect(headerRect.GetBL(), headerRect.GetBR());
//	nodeDrawList->AddLine(
//		ImVec2(headerSeparatorRect.GetTL().x + (-8 - halfBorderWidth) + 1, headerSeparatorRect.GetTL().y + -0.5f),
//		ImVec2(headerSeparatorRect.GetTR().x + (8 - halfBorderWidth) - 1, headerSeparatorRect.GetTR().y + -0.5f),
//		ImColor(64, 64, 64, 255), 1.0f);
//
//	ImGui::PopID();
//
//	ImNodeEd::Suspend();
//	if(ImNodeEd::ShowNodeContextMenu(&currentNodeId))
//	{
//		ImGui::OpenPopup("NodeContextPopup");
//		
//	}
//	ImNodeEd::Resume();
//}
//
//void ScriptGraphSchema::RenderPin(const ScriptGraphPin* aPin)
//{
//	if (aPin->GetPinDirection() == PinDirection::Input)
//	{
//		const bool CanConstructInPlace = ScriptGraphTypeData::IsTypeInPlaceConstructible(aPin->GetDataType());
//
//		ImNodeEd::BeginPin(aPin->GetUID(), static_cast<ImNodeEd::PinKind>(aPin->GetPinDirection()));
//		DrawPinIcon(aPin, ScriptGraphTypeData::GetTypeColor(aPin->GetDataType()), aPin->IsPinConnected());
//		ImNodeEd::EndPin();
//		ImGui::SameLine();
//
//		if(aPin->IsLabelVisible())
//		{
//			const float y = ImGui::GetCursorPosY();
//			ImGui::SetCursorPosY(y + 3);
//			ImGui::TextUnformatted(aPin->GetLabel().c_str());
//			ImGui::SetCursorPosY(y);
//		}
//
//		if(!aPin->IsPinConnected() && CanConstructInPlace)
//		{
//			ImGui::SameLine();
//			const float y = ImGui::GetCursorPosY();
//
//			ScriptGraphTypeData::RenderPinInput(aPin->GetDataType(), aPin->GetUUID(), aPin->myData.Ptr);
//
//			// Hax to force ImGui to move the cursor back to where it should be.
//			ImGui::SetCursorPosY(y + 3);
//			ImGui::Dummy({0, 0});
//		}		
//	}
//	else
//	{
//		if (aPin->IsLabelVisible())
//		{
//			ImGui::SetCursorPosX(
//				ImGui::GetCursorPosX()
//				+ ImGui::GetColumnWidth()
//				- ImGui::CalcTextSize(aPin->GetLabel().c_str()).x
//				- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x
//				- 14.0f
//			);
//
//			const float y = ImGui::GetCursorPosY();
//			ImGui::SetCursorPosY(y + 3);
//			ImGui::TextUnformatted(aPin->GetLabel().c_str());
//			ImGui::SameLine();
//			ImGui::SetCursorPosY(y);
//		}
//		else
//		{
//			ImGui::SetCursorPosX(
//				ImGui::GetCursorPosX()
//				+ ImGui::GetColumnWidth()
//				- ImGui::GetScrollX() - ImGui::GetStyle().ItemSpacing.x
//				- 14.0f
//			);
//		}
//
//		ImNodeEd::BeginPin(aPin->GetUID(), static_cast<ImNodeEd::PinKind>(aPin->GetPinDirection()));
//		DrawPinIcon(aPin, ScriptGraphTypeData::GetTypeColor(aPin->GetDataType()), aPin->IsPinConnected());
//		ImNodeEd::EndPin();
//	}
//}

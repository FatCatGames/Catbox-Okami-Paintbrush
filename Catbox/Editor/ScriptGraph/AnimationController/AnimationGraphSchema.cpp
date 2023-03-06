#include "stdafx.h"
#include "MuninGraph.pch.h"
#include "AnimationGraphSchema.h"

#include <fstream>

#include "AnimationGraph.h"
#include "AnimationGraphNode.h"

#include "..\External\nlohmann\json.hpp"
#include "ScriptGraphVariable.h"
#include "AnimationTransition.h"
#include "Graphics/Animations/Animation.h"
#include "Graphics/Animations/SkeletonData.h"
#include "Assets\AnimationController.h"

void AnimationGraphNodeType::SetupNodeType(AnimationGraphNodeType& aNodeType)
{
	if (!aNodeType.DefaultObject)
	{
		aNodeType.DefaultObject = aNodeType.New();
		aNodeType.DefaultObject->Init();
	}
	const auto uuidObj = AsUUIDAwareSharedPtr(aNodeType.DefaultObject);
	aNodeType.TypeName = uuidObj->GetTypeName();
}


std::shared_ptr<AnimationGraph> AnimationGraphSchema::CreateAnimationGraphInternal(AnimationController* aController)
{
	std::shared_ptr<AnimationGraph> aGraph = std::make_shared<AnimationGraph>();
	aGraph->SetController(aController);
	auto newNode = std::make_shared<AnimationGraphNode>();
	aGraph->GetGraphSchema()->AddNode(newNode);
	return aGraph;
}


std::shared_ptr<AnimationGraph> AnimationGraphSchema::CreateAnimationGraph(AnimationController* aController)
{
	return CreateAnimationGraphInternal(aController);
}


bool AnimationGraphSchema::SerializeAnimationGraph(AnimationController* controller, std::string& outResult)
{
	using namespace nlohmann;

	json graphJson;

	graphJson["variables"] = json::array();
	for (auto& [varName, variable] : controller->GetVariables())
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

	graphJson["layers"] = json::array();
	graphJson["sk"] = controller->GetSkeleton() ? controller->GetSkeleton()->GetName() : "";

	for (auto& [name, layer] : controller->GetLayers())
	{
		json layerJson;
		layerJson["name"] = name;
		layerJson["active"] = layer->isActive;
		layerJson["nodes"] = json::array();
		layerJson["boneMask"] = json::array();
		for (size_t i = 0; i < layer->myBoneMaskRootIDs.size(); i++)
		{
			layerJson["boneMask"].push_back(layer->myBoneMaskRootIDs[i]);
		}


		for (const auto& [nodeUID, node] : layer->myNodes)
		{
			// UUID keeps track of the type, which is handy.
			const auto uuidAwareBase = AsUUIDAwarePtr(node.get());

			json nodeJson;

			// We save the type of node, the UID (will be unique in the scope of
			// the file we save this graph to, but nowhere else), and the node pins.
			//nodeJson["type"] = uuidAwareBase->GetTypeName();
			nodeJson["UID"] = uuidAwareBase->GetUID();

			const ImVec2 nodePos = ImNodeEd::GetNodePosition(uuidAwareBase->GetUID());
			nodeJson["x"] = nodePos.x;
			nodeJson["y"] = nodePos.y;
			const float nodeZ = ImNodeEd::GetNodeZPosition(uuidAwareBase->GetUID());
			nodeJson["z"] = nodeZ;
			nodeJson["isEntry"] = node->IsEntryNode();
			nodeJson["isAnyState"] = node->IsAnyStateNode();
			if (node->IsAnyStateNode())
			{
				nodeJson["transToSelf"] = node->myCanTransitionToItself;
			}
			nodeJson["loop"] = node->myShouldLoop;
			nodeJson["animation"] = node->GetAnimation() ? node->GetAnimation()->GetName() : "";
			nodeJson["events"] = json::array();

			for (auto& event : node->myEvents)
			{
				json eventJson;
				eventJson["type"] = (int)event.myType;
				eventJson["frame"] = event.myFrame;

				if (event.myType == AnimationEventType::Prefab)
				{
					eventJson["prefabName"] = event.myPrefabData.prefab ? event.myPrefabData.prefab->GetName() : "";
					eventJson["worldSpace"] = event.myPrefabData.worldSpace;
					eventJson["x"] = event.myPrefabData.position.x;
					eventJson["y"] = event.myPrefabData.position.y;
					eventJson["z"] = event.myPrefabData.position.z;
					eventJson["bone"] = event.myPrefabData.bone ? event.myPrefabData.bone->name : "";
				}

				nodeJson["events"].push_back(eventJson);
			}

			nodeJson["pins"] = json::array();
			layerJson["nodes"].push_back(nodeJson);
		}

		layerJson["edges"] = json::array();

		// For edges we need to know the UID of the involved nodes, same UID we saved
		// in the nodes section above, and we also need to know which pin. Pins are
		// stored using labels on that node making them easy to find.
		for (auto& [edgeUID, edge] : layer->myEdges)
		{
			const AnimationGraphPin& sourcePin = layer->GetPinFromUID(edge.FromUID);
			const auto sourceNodeUUID = AsUUIDAwareSharedPtr(sourcePin.GetOwner());

			const AnimationGraphPin& targetPin = layer->GetPinFromUID(edge.ToUID);
			const auto targetNodeUUID = AsUUIDAwareSharedPtr(targetPin.GetOwner());

			json edgeJson;
			edgeJson["sourceUID"] = sourceNodeUUID->GetUID();
			edgeJson["sourcePin"] = sourcePin.GetLabel();

			edgeJson["targetUID"] = targetNodeUUID->GetUID();
			edgeJson["targetPin"] = targetPin.GetLabel();
			layer->myTransitions.find(edgeUID);
			edgeJson["conditions"] = json::array();
			auto& transition = layer->myTransitions.at(edgeUID);
			edgeJson["hasExitTime"] = transition.HasExitTime;

			for (auto& condition : transition.GetConditions())
			{
				json conditionJson;
				conditionJson["leftVar"] = condition.myLeftHandValue->Name;

				const std::type_index varTypeId = condition.myLeftHandValue->GetTypeData()->GetType();
				std::vector<uint8_t> varData;
				ScriptGraphDataTypeRegistry::Serialize(varTypeId, condition.myRightHandValue.Data.Ptr, varData);
				conditionJson["rightVar"] = varData;
				conditionJson["op"] = static_cast<int>(condition.myOp);
				edgeJson["conditions"].push_back(conditionJson);
			}

			layerJson["edges"].push_back(edgeJson);
		}

		graphJson["layers"].push_back(layerJson);
	}

	outResult = graphJson.dump();

	return true;
}

bool AnimationGraphSchema::DeserializeAnimationGraph(AnimationController* controller, const std::string& inData)
{
	using namespace nlohmann;
	json graphJson = json::parse(inData);
	controller->GetVariables().clear();
	controller->GetLayers().clear();
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
		controller->GetVariables().insert({ varName, newVariable });
	}

	std::string skName = graphJson["sk"];
	if (!skName.empty())
	{
		controller->SetSkeleton(AssetRegistry::GetInstance()->GetAsset<Skeleton>(skName));
	}

	for (const auto& layer : graphJson["layers"])
	{
		std::string layerName = layer.at("name");
		std::shared_ptr<AnimationGraph> graph = controller->AddLayer(layerName);
		std::unique_ptr<AnimationGraphSchema> outGraphSchema = graph->GetGraphSchema();

		graph->myEdges.clear();
		graph->myTransitions.clear();
		graph->myPins.clear();
		graph->myEntryPoint = nullptr;
		graph->myNodeUIDToEntryHandle.clear();
		graph->myNodes.clear();
		graph->myBoneMaskRootIDs.clear();

		for (size_t i = 0; i < graph->myBoneMask.size(); i++)
		{
			graph->myBoneMask[i] = false;
		}

		auto& sk = controller->GetSkeleton();


		if (layer.contains("boneMask"))
		{
			for (int rootID : layer["boneMask"].get<std::vector<int>>())
			{
				graph->myBoneMaskRootIDs.push_back(rootID);
				graph->RegenerateBoneMask(sk);
			}
		}

		if (layer.contains("active"))
		{
			graph->isActive = layer["active"];
		}

		std::unordered_map<size_t, std::shared_ptr<AnimationGraphNode>> fileUIDToNode;

		// Create all the nodes.
		for (const auto& it : layer["nodes"])
		{
			std::shared_ptr<AnimationGraphNode> newNode = std::make_shared<AnimationGraphNode>();

			if (it.contains("isAnyState"))
			{
				if (it.at("isAnyState"))
				{
					outGraphSchema->RegisterAnyStateNode(newNode.get());
					if (it.contains("transToSelf"))
					{
						newNode->myCanTransitionToItself = it.at("transToSelf");
					}
				}
			}
			if (it.contains("isEntry"))
			{
				newNode->SetIsEntryNode(it.at("isEntry"));
			}
			if (it.contains("loop"))
			{
				newNode->myShouldLoop = it.at("loop");
			}
			if (it.contains("events"))
			{
				for (const auto& eventIt : it["events"])
				{
					AnimationEvent event;
					event.myFrame = eventIt.at("frame");
					event.myType = (AnimationEventType)eventIt.at("type");

					if (event.myType == AnimationEventType::Prefab)
					{
						event.myPrefabData.prefab = AssetRegistry::GetInstance()->GetAsset<Prefab>(eventIt.at("prefabName"));
						event.myPrefabData.worldSpace = eventIt.at("worldSpace");
						event.myPrefabData.position.x = eventIt.at("x");
						event.myPrefabData.position.y = eventIt.at("y");
						event.myPrefabData.position.z = eventIt.at("z");
						if (eventIt.contains("bone")) {
							std::string boneName = eventIt.at("bone");
							auto& sk = controller->GetSkeleton();
							if (sk->boneNameToIndex.find(boneName) != sk->boneNameToIndex.end())
							{
								event.myPrefabData.bone = &sk->bones[sk->boneNameToIndex[boneName]];
							}
							else printerror("Bone " + boneName + " removed or renamed. Failed to fully load animation event");
						}
					}

					event.SetIndex(newNode->myEvents.size() + 1);
					newNode->myEvents.push_back(event);
				}
			}
			if (!graph->myCurrentNode)
			{
				graph->myCurrentNode = newNode;
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
					}
				}
			}

			if (EDITORMODE && ImNodeEd::GetCurrentEditor())
			{
				const auto uuidAwareBase = AsUUIDAwareSharedPtr(newNode);

				const float x = it.at("x");
				const float y = it.at("y");
				const float z = it.at("z");

				ImNodeEd::SetNodePosition(uuidAwareBase->GetUID(), { x, y });
				ImNodeEd::SetNodeZPosition(uuidAwareBase->GetUID(), z);
			}


			std::string animationName = it.at("animation");
			if (!animationName.empty())
			{
				newNode->SetAnimation(AssetRegistry::GetInstance()->GetAsset<Animation>(animationName));
			}

			outGraphSchema->RegisterNode(newNode);

			fileUIDToNode.insert({ it.at("UID"), newNode });
		}

		// And all the edges
		for (const auto& it : layer["edges"])
		{
			const size_t sourceUID = it.at("sourceUID");
			const std::string sourcePinLabel = it.at("sourcePin");
			const size_t targetUID = it.at("targetUID");
			const std::string targetPinLabel = it.at("targetPin");
			bool hasExitTime = false;
			if (it.contains("hasExitTime"))
			{
				hasExitTime = it.at("hasExitTime");
			}

			// Find the source node owning the source pin.
			if (auto srcIt = fileUIDToNode.find(sourceUID); srcIt != fileUIDToNode.end())
			{
				// And find the target node owning the target pin.
				if (auto tgtIt = fileUIDToNode.find(targetUID); srcIt != fileUIDToNode.end())
				{
					const std::shared_ptr<AnimationGraphNode> sourceNode = srcIt->second;
					const std::shared_ptr<AnimationGraphNode> targetNode = tgtIt->second;

					// Get the actual pins from the nodes.
					const AnimationGraphPin* srcPin = sourceNode->GetPin(sourcePinLabel);
					if (!srcPin) continue;

					const AnimationGraphPin* tgtPin = targetNode->GetPin(targetPinLabel);
					if (tgtPin)
					{
						int edgeUID = outGraphSchema->myGraph->myNextEdgeId;
						outGraphSchema->CreateEdge(srcPin->GetUID(), tgtPin->GetUID());
						auto& transitions = outGraphSchema->myGraph->GetTransitions();
						auto& transition = transitions.at(edgeUID);
						transition.HasExitTime = hasExitTime;


						auto& variables = outGraphSchema->GetVariables();
						for (const auto& conditionIt : it.at("conditions"))
						{
							if (variables.find(conditionIt.at("leftVar")) != variables.end())
							{
								auto& leftVar = variables.at(conditionIt.at("leftVar"));
								auto& condition = transition.AddCondition(leftVar);
								const std::vector<uint8_t> varData = conditionIt.at("rightVar");
								ScriptGraphDataTypeRegistry::Deserialize(leftVar->GetTypeData()->GetType(), condition.myRightHandValue.Data.Ptr, varData);
								condition.myOp = (ComparisonOp)conditionIt.at("op");
							}
							else
							{
								std::string errorMsg = "Variable ";
								errorMsg += conditionIt.at("leftVar");
								errorMsg += " not found!";
								printerror(errorMsg);
							}
						}
					}
				}
			}
		}
	}

	return true;
}


AnimationGraphPin& AnimationGraphSchema::GetMutablePin(size_t aPinUID)
{
	return myGraph->myPins.find(aPinUID)->second;
}

bool AnimationGraphSchema::RegisterNode(std::shared_ptr<AnimationGraphNode> aNode)
{
	assert(aNode && "The requested node is not of the correct base node type!");

	if (aNode->GetPins().empty())
		aNode->Init();

	const auto uidAwareNode = AsUUIDAwarePtr(aNode.get());

	aNode->myOwner = myGraph;

	myGraph->myNodes.insert({ uidAwareNode->GetUID(), aNode });

	if (aNode->IsEntryNode() || !myGraph->GetEntryPoint())
	{
		RegisterEntryPointNode(aNode);
	}

	for (auto& [key, value] : aNode->myPins)
	{
		myGraph->myPins.insert({ key, value });
	}

	return true;
}

void AnimationGraphSchema::RegisterAnyStateNode(AnimationGraphNode* aNode)
{
	aNode->SetIsAnyState(true);
	myGraph->myAnyStateNodes.insert({ aNode->GetUID(), aNode });
}

void AnimationGraphSchema::UnregisterAnyStateNode(AnimationGraphNode* aNode)
{
	aNode->SetIsAnyState(false);
	if (myGraph->myAnyStateNodes.find(aNode->GetUID()) != myGraph->myAnyStateNodes.end())
	{
		myGraph->myAnyStateNodes.erase(aNode->GetUID());
	}
}

void AnimationGraphSchema::RegisterEntryPointNode(std::shared_ptr<AnimationGraphNode> aNode)
{
	aNode->SetIsEntryNode(true);
	myGraph->myCurrentNode = aNode;

	if (myGraph->myEntryPoint)
	{
		myGraph->myEntryPoint->SetIsEntryNode(false);
	}
	myGraph->myEntryPoint = aNode;
	const auto uuidAwareBase = AsUUIDAwarePtr(aNode.get());
	//myGraph->myNodeUIDToEntryHandle.insert({ uuidAwareBase->GetUID(), aEntryHandle });
}

void AnimationGraphSchema::CreateEdgeInternal(AnimationGraphPin& aSourcePin, AnimationGraphPin& aTargetPin) const
{
	const size_t newEdgeUID = myGraph->myNextEdgeId++;
	const float edgeThickness = 3.0f;

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

	myGraph->myTransitions.insert(
		{
			newEdgeUID, AnimationTransition(newEdgeUID, aTargetPin.GetOwner())
		});

	aSourcePin.GetOwner()->myTransitions[newEdgeUID] = &myGraph->myTransitions[newEdgeUID];

	aSourcePin.AddPinEdge(newEdgeUID);
	aTargetPin.AddPinEdge(newEdgeUID);
}


AnimationGraphSchema::AnimationGraphSchema(const std::shared_ptr<void>& aGraph)
{
	myGraph = std::static_pointer_cast<AnimationGraph>(aGraph);
}


std::shared_ptr<AnimationGraphNode> AnimationGraphSchema::AddNode(const std::shared_ptr<AnimationGraphNode>& aNode)
{
	if (RegisterNode(aNode))
	{
		return aNode;
	}

	return nullptr;
}

void AnimationGraphSchema::RemoveNode(size_t aNodeUID)
{
	bool success = false;

	if (const auto nodeIt = myGraph->myNodes.find(aNodeUID); nodeIt != myGraph->myNodes.end())
	{
		if (nodeIt->second->IsAnyStateNode())
		{
			UnregisterAnyStateNode(nodeIt->second.get());
		}

		bool isEntry = nodeIt->second->IsEntryNode();
		if (isEntry)
		{
			if (myGraph->myEntryPoint)
			{
				myGraph->myEntryPoint->SetIsEntryNode(false);
			}
			myGraph->myEntryPoint = nullptr;

			for (auto& [pinUID, pin] : nodeIt->second->myPins)
			{
				if (pin.myEdges.size() > 0)
				{
					auto& edge = myGraph->GetEdges().at(pin.myEdges[0]);
					auto& otherPin = myGraph->myPins.at(edge.ToUID == pin.GetUID() ? edge.FromUID : edge.ToUID);
					auto newEntryNode = otherPin.GetOwner();
					if (newEntryNode->IsAnyStateNode()) continue;
					RegisterEntryPointNode(newEntryNode);
					success = true;
					break;
				}
			}
		}
		else success = true;


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

		myGraph->myNodes.erase(nodeIt);

		if (!success)
		{
			RegisterEntryPointNode(myGraph->GetNodes().begin()->second);
		}
	}
}

void AnimationGraphSchema::RemoveVariable(const std::string& aVariableName)
{
	myGraph->myVariables->erase(aVariableName);
}


bool AnimationGraphSchema::CanCreateEdge(size_t aSourcePinUID, size_t aTargetPinUID, std::string& outMesssage) const
{
	if (aSourcePinUID == aTargetPinUID)
	{
		return false;
	}

	const AnimationGraphPin& pinA = myGraph->GetPinFromUID(aSourcePinUID);
	const AnimationGraphPin& pinB = myGraph->GetPinFromUID(aTargetPinUID);

	const AnimationGraphPin& sourcePin = pinA.GetPinDirection() == PinDirection::Output ? pinA : pinB;
	const AnimationGraphPin& targetPin = pinB.GetPinDirection() == PinDirection::Input ? pinB : pinA;

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

	return true;
}

bool AnimationGraphSchema::CreateEdge(size_t aSourcePinUID, size_t aTargetPinUID)
{
	std::string outMessage;
	if (CanCreateEdge(aSourcePinUID, aTargetPinUID, outMessage))
	{
		AnimationGraphPin& pinA = GetMutablePin(aSourcePinUID);
		AnimationGraphPin& pinB = GetMutablePin(aTargetPinUID);

		AnimationGraphPin& sourcePin = pinA.GetPinDirection() == PinDirection::Output ? pinA : pinB;
		AnimationGraphPin& targetPin = pinB.GetPinDirection() == PinDirection::Input ? pinB : pinA;

		CreateEdgeInternal(sourcePin, targetPin);

		return true;
	}

	return false;
}

bool AnimationGraphSchema::DisconnectPin(size_t aPinUID)
{
	AnimationGraphPin& pin = GetMutablePin(aPinUID);

	const std::vector<size_t> pinEdgeIds = pin.GetEdges();
	for (auto it = pinEdgeIds.begin(); it != pinEdgeIds.end(); ++it)
	{
		RemoveEdge(*it);
	}

	return true;
}

bool AnimationGraphSchema::RemoveEdge(size_t aEdgeUID)
{
	auto it = myGraph->myEdges.find(aEdgeUID);
	assert(it != myGraph->myEdges.end());

	AnimationGraphPin& fromPin = GetMutablePin(it->second.FromUID);
	AnimationGraphPin& toPin = GetMutablePin(it->second.ToUID);

	fromPin.RemovePinEdge(aEdgeUID);
	toPin.RemovePinEdge(aEdgeUID);


	it = myGraph->myEdges.erase(it);

	auto it2 = myGraph->myTransitions.find(aEdgeUID);
	assert(it2 != myGraph->myTransitions.end());
	it2 = myGraph->myTransitions.erase(it2);

	fromPin.GetOwner()->myTransitions.erase(aEdgeUID);

	return true;
}
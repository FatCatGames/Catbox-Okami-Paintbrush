#include "stdafx.h"
#include "AnimationControllerEditor.h"
#include <imgui_node_editor.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>


#include "AnimationTransition.h"
#include "Graphics/Animations/Animation.h"
#include "Graphics/Animations/SkeletonData.h"
#include "Assets\AnimationController.h"
#include "ComponentTools\SceneManager.h"

inline ImVec4 operator/(const ImVec4& A, const float S)
{
	return { A.x / S, A.y / S, A.z / S, A.w / S };
}


static inline ImRect ImGui_GetItemRect()
{
	return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
{
	auto result = rect;
	result.Min.x -= x;
	result.Min.y -= y;
	result.Max.x += x;
	result.Max.y += y;
	return result;
}

namespace ed = ax::NodeEditor;

using namespace ax;

static ed::EditorContext* m_Editor = nullptr;
bool myVariableMenuToggle = false;
bool myLayerMenuToggle = false;


static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##Splitter");
	ImRect bb;
	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}


void AnimationControllerEditor::RenderNode(const AnimationGraphNode* aNode)
{
	const auto uidAwareBase = AsUUIDAwarePtr(aNode);
	ed::NodeId currentNodeId = uidAwareBase->GetUID();

	const float rounding = 5.0f;
	const float padding = 12.0f;

	const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

	if (aNode->IsAnyStateNode())
	{
		ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(90, 90, 120, 220));
	}

	else
	{
		if (aNode->IsEntryNode())
		{
			ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 150, 0, 200));
		}
		else
		{
			ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(100, 100, 100, 200));
		}
	}

	if (aNode->IsRunning())
	{
		ed::PushStyleVar(ed::StyleVar_NodeBorderWidth, 2);
		ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 200));
	}
	else
	{
		ed::PushStyleVar(ed::StyleVar_NodeBorderWidth, 1);
		ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
	}


	ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
	ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

	ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
	ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
	ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
	ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
	ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
	ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
	ed::PushStyleVar(ed::StyleVar_PinRadius, 5.0f);

	ed::BeginNode(currentNodeId);

	ImGui::PushID(static_cast<int>(uidAwareBase->GetUID()));

	ImRect inputsRect;
	float pinRectWidth = 100;
	float pinRectPadding = 30;
	float nodeWidth = pinRectPadding * 2 + pinRectWidth;
	int inputAlpha = 200;
	auto& inputPin = *aNode->GetPin("In");


	ImGui::Indent(pinRectPadding);
	ImGui::Dummy(ImVec2(pinRectWidth, padding));
	inputsRect = ImGui_GetItemRect();

	ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
	ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
	ed::PushStyleVar(ed::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);
	ed::BeginPin(inputPin.GetUID(), ed::PinKind::Input);
	ed::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
	ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
	ed::EndPin();
	ed::PopStyleVar(3);
	ImGui::Unindent(pinRectPadding);

	if (aNode->IsAnyStateNode())
	{
		ImGui::CenteredText("Any State", ImVec2(nodeWidth, 0));
	}
	else
	{
		ImGui::CenteredText(aNode->GetAnimation() ? aNode->GetAnimation()->GetName().c_str() : "Empty", ImVec2(nodeWidth, 0));
	}
	ImGui::Dummy(ImVec2(pinRectWidth + pinRectPadding * 2, 0));

	if (aNode->IsRunning() && aNode->GetAnimation() && myCurrentController)
	{
		int currentFrame = myGraph->myFrameIndex;
		float totalFrame = static_cast<float>(aNode->GetAnimation()->GetFrames().size());
		float percent = currentFrame / totalFrame;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 10);
		ImGui::Dummy(ImVec2(0, 3));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1, 1, 1, 1));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, pinBackground);
		ImGui::Indent(pinRectPadding / 2.f);
		ImGui::ProgressBar(percent, ImVec2(pinRectWidth + pinRectPadding, 5), "");
		ImGui::Unindent(pinRectPadding / 2.f);
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();
		ImGui::Dummy(ImVec2(0, 3));
	}

	ImRect outputsRect;
	int outputAlpha = 200;
	auto& outputPin = *aNode->GetPin("Out");

	ImGui::Indent(pinRectPadding);

	ed::BeginPin(outputPin.GetUID(), ed::PinKind::Output);
	ImGui::Dummy(ImVec2(pinRectWidth, padding));
	outputsRect = ImGui_GetItemRect();
	ImGui::Unindent(pinRectPadding);

	ed::PushStyleVar(ed::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);
	ed::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
	ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
	ed::EndPin();
	ed::PopStyleVar();


	ed::EndNode();
	ed::PopStyleVar(8);
	ed::PopStyleColor(4);

	auto drawList = ed::GetNodeBackgroundDrawList(currentNodeId);


	const auto    topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
	const auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;

	drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
		IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
	drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
		IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
	drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
		IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
	drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
		IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);

	ImGui::PopID();
}


void AnimationControllerEditor::Render()
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);
	ImGui::SetNextWindowSize({ 1280, 720 }, ImGuiCond_FirstUseEver);
	if (myGraph)
	{

		ImGui::SetNextItemWidth(50);
		if (ImGui::Button("Play"))
		{
			if (myCurrentController)
			{
				myCurrentController->Reset();
			}
			TriggerEntryPoint();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Open the Trigger Entry Point dialog.");
		}

		if (mySelectedObject)
		{
			ImGui::SameLine();
			if (ImGui::Button("Toggle Variables"))
			{
				myVariableMenuToggle = !myVariableMenuToggle;
				if (myVariableMenuToggle) myLayerMenuToggle = false;
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Layers"))
		{
			myLayerMenuToggle = !myLayerMenuToggle;
			if (myLayerMenuToggle) myVariableMenuToggle = false;
		}

		ImGui::SameLine();
		if (ImGui::Button("Edit Variables"))
		{
			ImGui::OpenPopup("Edit Variables");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Open the Edit Variables dialog.");
		}

		ImGui::SameLine();
		if (ImGui::Button("Zoom to Content"))
		{
			myState.initNavToContent = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			SaveGraph(myCurrentController, myCurrentGraphPath);
			if (mySelectedObject)
			{
				LoadGraph(reinterpret_cast<AnimationController*>(myCurrentController->GetRootAsset()), myCurrentGraphPath);
			}
			ImNodeEd::SetCurrentEditor(nodeEditorContext);
		}
		if (mySelectedObject)
		{
			ImGui::SameLine();
			ImGui::Text(("Viewing " + mySelectedObject->GetName() + " controller").c_str());
			ImGui::SameLine();
			if (ImGui::Button("Reload"))
			{
				LoadGraph(myCurrentController, myCurrentGraphPath);
				SetController(myCurrentController);
			}
		}

		if (ImNodeEd::Begin("A Node Editor"))
		{
			float x = ImGui::GetWindowContentRegionWidth();
			for (const auto& [nodeUID, node] : myGraph->GetNodes())
			{
				if (myState.errorIsErrorState && nodeUID == myState.errorNodeId)
				{
					ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodeBorderWidth, 10.0f);
					ImNodeEd::PushStyleColor(ImNodeEd::StyleColor_NodeBorder, ImVec4(0.75f, 0, 0, 1));
				}

				RenderNode(node.get());

				if (myState.errorIsErrorState && nodeUID == myState.errorNodeId)
				{
					ImNodeEd::PopStyleVar();
					ImNodeEd::PopStyleColor();

					if (ImNodeEd::GetHoveredNode().Get() == nodeUID)
					{
						ImNodeEd::Suspend();
						ImGui::SetTooltip(myState.errorMessage.c_str());
						ImNodeEd::Resume();
					}
				}
			}

			std::vector<const NodeGraphEdge*> selectedEdges;
			std::vector<size_t> selectedEdgeIDs;

			for (const auto& [edgeId, edge] : myGraph->GetEdges())
			{
				const ImVec4 normalizedColor = ImVec4(edge.Color.x / 255, edge.Color.y / 255, edge.Color.z / 255, edge.Color.w / 255);
				ImNodeEd::Link(edge.EdgeId, edge.FromUID, edge.ToUID, normalizedColor, edge.Thickness);

				if (ed::IsLinkSelected(edge.EdgeId))
				{
					selectedEdges.push_back(&edge);
					selectedEdgeIDs.push_back(edge.EdgeId);
				}
			}


			if (myState.flowShowFlow || !myGraph->GetLastExecutedPath().empty())
			{
				for (const auto& edgeId : myGraph->GetLastExecutedPath())
				{
					ImNodeEd::Flow(edgeId, ax::NodeEditor::FlowDirection::Forward);
				}
				myState.flowShowFlow = false;
				myGraph->ResetLastExecutedPath();
			}

			HandleEditorCreate();
			HandleEditorDelete();

			for (size_t i = 0; i < selectedEdges.size();)
			{
				auto it = myGraph->GetEdges().find(selectedEdgeIDs[i]);
				if (it == myGraph->GetEdges().end())
				{
					selectedEdges.erase(selectedEdges.begin() + i);
					continue;
				}
				++i;
			}

			ImNodeEd::Suspend();

			if (ImNodeEd::ShowBackgroundContextMenu())
			{
				myState.bgCtxtSearchFocus = true;
				ImGui::OpenPopup("BackgroundContextMenu");
			}
			else if (ImNodeEd::NodeId nodeId; ImNodeEd::ShowNodeContextMenu(&nodeId))
			{
				ImGui::OpenPopup("NodeContextMenu");
			}

			RightClickMenu();
			RenderPropertyMenu(selectedEdges);
			if (myVariableMenuToggle) RenderVariableMenu();
			if (myLayerMenuToggle) RenderLayerMenu();

			ImNodeEd::Resume();

			if (myState.initNavToContent)
			{
				ImNodeEd::NavigateToContent();
				myState.initNavToContent = false;
			}
			ImNodeEd::End();
		}

		EditVariables();
	}

	ImNodeEd::SetCurrentEditor(nullptr);
}

void AnimationControllerEditor::SetController(AnimationController* aController)
{
	myCurrentController = aController;
	myCurrentGraphPath = "";


	myGraph = aController->GetLayers().begin()->second;
	if (myGraph)
	{
		mySchema = myGraph->GetGraphSchema();
		myCurrentGraphPath = aController->GetPath().string();
	}
	else
	{
		mySchema = nullptr;
	}
}

void AnimationControllerEditor::ClearController()
{
	myCurrentController = nullptr;
	myGraph = nullptr;
	mySchema = nullptr;
	myCurrentGraphPath = "";
}


void AnimationControllerEditor::TriggerEntryPoint()
{
	auto graphEntryPoint = mySchema->GetGraph()->GetEntryPoint();
	myState.flowShowFlow = true;
	ClearErrorState();
	myGraph->Run();
}

void AnimationControllerEditor::EditVariables()
{
	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	ImGui::SetNextWindowSize({ 1024, 512 });
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.15f, 0.15f, 0.15f, 1));

	if (ImGui::BeginPopupModal("Edit Variables", 0, ImGuiWindowFlags_NoResize))
	{
		const auto& graphVariables = mySchema->GetVariables();
		ImGui::BeginTable("body", 4, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg);
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Type");
		ImGui::TableSetupColumn("Default");
		ImGui::TableSetupColumn("Actions");
		ImGui::TableHeadersRow();
		int varIdx = 0;
		for (const auto& [varName, variable] : graphVariables)
		{
			ImGui::TableNextColumn();
			auto typeColor = ScriptGraphDataTypeRegistry::GetDataTypeColor(variable->GetTypeData()->GetType()) / 255.0f;
			ImGui::TextColored(typeColor, varName.c_str());
			ImGui::TableNextColumn();
			ImGui::TextColored(typeColor, ScriptGraphDataTypeRegistry::GetFriendlyName(variable->GetTypeData()->GetType()).c_str());
			ImGui::TableNextColumn();
			if (myState.varInlineEditingIdx == varIdx && ScriptGraphDataTypeRegistry::IsTypeInPlaceConstructible(variable->GetTypeData()->GetType()))
			{
				ScriptGraphDataTypeRegistry::RenderEditInPlaceWidget(variable->GetTypeData()->GetType(), variable->DefaultData.GetUUID(), variable->DefaultData.Ptr);
			}
			else
			{
				ImGui::TextUnformatted(ScriptGraphDataTypeRegistry::GetString(variable->GetTypeData()->GetType(), variable->DefaultData.Ptr).c_str());
			}
			ImGui::TableNextColumn();
			ImGui::PushID(variable->Name.c_str());
			ImGui::BeginDisabled(myState.varInlineEditingIdx == varIdx);
			if (ImGui::Button("Delete"))
			{
				myState.varToDelete = variable->Name;
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			ImGui::BeginDisabled(!ScriptGraphDataTypeRegistry::IsTypeInPlaceConstructible(variable->GetTypeData()->GetType()));
			if (myState.varInlineEditingIdx == varIdx)
			{
				if (ImGui::Button("Save"))
				{
					variable->ResetVariable();
					myState.varInlineEditingIdx = -1;
				}
			}
			else
			{
				if (ImGui::Button("Edit"))
				{
					myState.varInlineEditingIdx = varIdx;
				}
			}
			ImGui::EndDisabled();
			ImGui::PopID();
			varIdx++;
		}
		ImGui::EndTable();

		ImGui::Separator();

		ImGui::TextUnformatted("Create New Variable");

		ImGui::PushItemWidth(200);
		ImGui::InputText("##newVar", &myState.varNewVarNameField);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		const std::vector<std::string>& typeNames = ScriptGraphDataTypeRegistry::GetRegisteredTypeNames();
		ImGui::PushItemWidth(150);
		if (ImGui::BeginCombo("##newVarType", myState.varNewVarTypeIdx >= 0 ? typeNames[myState.varNewVarTypeIdx].c_str() : nullptr, 0))
		{
			for (size_t i = 0; i < typeNames.size(); i++)
			{
				const bool isSelected = (myState.varNewVarTypeIdx == static_cast<int>(i));
				if (ImGui::Selectable(typeNames[i].c_str(), isSelected))
				{
					myState.varNewVarTypeIdx = static_cast<int>(i);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::BeginDisabled(myState.varNewVarNameField.empty() || myState.varNewVarTypeIdx < 0);

		if (!myState.varNewVarValue.TypeData || myState.varNewVarValue.TypeData != ScriptGraphDataTypeRegistry::GetTypeFromFriendlyName(typeNames[myState.varNewVarTypeIdx]))
		{
			myState.varNewVarValue = ScriptGraphDataTypeRegistry::GetDataObjectOfType(*ScriptGraphDataTypeRegistry::GetTypeFromFriendlyName(typeNames[myState.varNewVarTypeIdx]));
		}

		ImGui::TextUnformatted("Default Value:");
		ImGui::SameLine();

		if (ScriptGraphDataTypeRegistry::IsTypeInPlaceConstructible(*myState.varNewVarValue.TypeData))
		{
			const float y = ImGui::GetCursorPosY(); ImGui::SetCursorPosY(y + 2);
			ScriptGraphDataTypeRegistry::RenderEditInPlaceWidget(*myState.varNewVarValue.TypeData, myState.varNewVarValue.GetUUID(), myState.varNewVarValue.Ptr);
			ImGui::SetCursorPosY(y);
			ImGui::SameLine();
		}

		if (ImGui::Button("Create", ImVec2(100, 0)))
		{
			mySchema->AddVariable(myState.varNewVarNameField, myState.varNewVarValue);
			myState.varNewVarNameField.clear();
			myState.varNewVarValue = ScriptGraphDataObject();
		}
		ImGui::EndDisabled();

		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 100);
		if (ImGui::Button("Close", ImVec2(100, 0)))
		{
			ImGui::CloseCurrentPopup();
			UpdateVariableContextMenu();
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

	if (!myState.varToDelete.empty())
	{
		mySchema->RemoveVariable(myState.varToDelete);
		myState.varToDelete.clear();
	}
}

string searchTerm;

void AnimationControllerEditor::RenderPropertyMenu(std::vector<const NodeGraphEdge*> aSelectedEdges)
{
	std::vector<AnimationGraphNode*> selectedNodes;
	if (aSelectedEdges.empty())
	{
		for (auto [nodeid, node] : myGraph->GetNodes())
		{
			if (ed::IsNodeSelected(nodeid))
			{
				selectedNodes.push_back(node.get());
			}
		}
	}

	if (aSelectedEdges.empty() && selectedNodes.empty()) return;

	float x = ImGui::GetWindowContentRegionWidth();

	ImVec2 currentPos = ImGui::GetWindowPos();
	ImVec2 currentSize = ImGui::GetWindowSize();

	float yOffset = 20;
	float xOffset = 10;
	ImVec2 paneWidth = ImVec2(310, ImGui::GetWindowContentRegionMax().y - yOffset);

	ImGui::SetNextWindowSize(paneWidth, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(currentPos.x + currentSize.x - paneWidth.x, currentPos.y + yOffset));
	ImGui::Begin("##AnimationProperties", nullptr, ImGuiWindowFlags_NoTitleBar);


	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetCursorScreenPos(),
		ImGui::GetCursorScreenPos() + paneWidth,
		ImColor(ImGui::GetStyle().Colors[ImGuiCol_WindowBg]));

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 30);
	ImGui::Indent(xOffset);

	if (!selectedNodes.empty())
	{
		if (!selectedNodes[0]->IsAnyStateNode())
		{
			if (ImGui::Button("Set as Entry State"))
			{
				mySchema->RegisterEntryPointNode(myGraph->GetNodes().at(selectedNodes[0]->GetUID()));
				if (myCurrentController)
				{
					myCurrentController->Reset();
				}
			}
		}

		std::shared_ptr<Animation> anim = selectedNodes[0]->GetAnimation();
		for (auto& node : selectedNodes)
		{
			if (node->GetAnimation() != anim)
			{
				anim = nullptr;
				break;
			}
		}

		if (myCurrentController->GetSkeleton())
		{
			int index = selectedNodes[0]->IsAnyStateNode() ? 1 : 0;
			auto& animations = myCurrentController->GetSkeleton()->allAnimations;
			std::vector<const char*> assetNames(animations.size() + 2);
			assetNames[0] = "Empty";
			assetNames[1] = "Any State";

			int counter = 2;
			for (auto& animation : animations)
			{
				if (animation == anim) index = counter;
				assetNames[counter] = animation->GetName().c_str();
				counter++;
			}

			if (ImGui::BeginCombo("Animation", assetNames[index]))
			{
				if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
					ImGui::SetKeyboardFocusHere(0);
				ImGui::InputText("Search", &searchTerm);


				for (int i = 0; i < assetNames.size(); i++)
				{
					if (Catbox::ToLowerString(assetNames[i]).find(Catbox::ToLowerString(searchTerm)) != std::string::npos)
					{
						if (ImGui::Selectable(assetNames[i], false))
						{
							index = i;
							for (auto& node : selectedNodes)
							{
								if (i == 1)
								{
									if (!node->IsEntryNode())
									{
										mySchema->RegisterAnyStateNode(node);
									}
								}
								else
								{
									if (node->IsAnyStateNode())
									{
										mySchema->UnregisterAnyStateNode(node);
									}
									if (i == 0) node->SetAnimation(nullptr);
									else node->SetAnimation(animations[i - 2]);
								}
							}

							if (myCurrentController)
							{
								myCurrentController->Reset();
							}
						}
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Checkbox("Looping", &selectedNodes[0]->myShouldLoop))
			{
				for (auto& node : selectedNodes)
				{
					node->myShouldLoop = selectedNodes[0]->myShouldLoop;
				}
			}

			if (selectedNodes[0]->IsAnyStateNode())
			{
				if (ImGui::Checkbox("Can Transition To Itself", &selectedNodes[0]->myCanTransitionToItself))
				{
					for (auto& node : selectedNodes)
					{
						node->myCanTransitionToItself = selectedNodes[0]->myCanTransitionToItself;
					}
				}
			}


		}
		else
		{
			ImGui::Text("Assign a skeleton to the controller");
		}

		ImGui::Spacing();
		ImGui::Separator();

		if (selectedNodes.size() > 1)
		{
			ImGui::Text("Animation event editing not supported for multiple nodes");
		}
		else
		{
			ImGui::Spacing();
			ImGui::Text("Events");
			ImGui::Spacing();
			auto& events = selectedNodes[0]->myEvents;
			for (size_t i = 0; i < events.size(); )
			{
				auto& event = events[i];
				ImGui::Separator();
				ImGui::Spacing();
				if (ImGui::Button(("X##" + std::to_string(i)).c_str()))
				{
					events.erase(events.begin() + i);
					for (size_t n = i; n < events.size(); n++)
					{
						events[i].SetIndex(n);
					}
					continue;
				}
				ImGui::SameLine();
				event.RenderInProperties(myCurrentController);
				++i;
			}
		}

		ImGui::Spacing();
		if (ImGui::Button("Add Event"))
		{
			for (auto& node : selectedNodes)
			{
				node->myEvents.emplace_back();
				node->myEvents.back().SetIndex(node->myEvents.size());
			}
		}
	}
	else
	{
		if (aSelectedEdges.size() > 1)
		{
			ImGui::Text("Editing of multiple links not supported yet, request this feature to Fat cat if needed.");
			ImGui::EndChild();
			return;
		}

		auto id = aSelectedEdges[0]->EdgeId;
		std::string s = std::to_string(aSelectedEdges.size());
		s += aSelectedEdges.size() > 1 ? " links selected" : " link selected";
		ImGui::Text(s.c_str());

		myGraph->GetTransitions().at(id).Render(*mySchema);
	}

	ImGui::Unindent(xOffset);

	ImGui::End();
}


void AnimationControllerEditor::RenderVariableMenu()
{
	float x = ImGui::GetWindowContentRegionWidth();

	ImVec2 currentPos = ImGui::GetWindowPos();

	float yOffset = 50;
	float xOffset = 7;

	ImVec2 paneWidth = ImVec2(150, ImGui::GetWindowContentRegionMax().y - yOffset);
	ImGui::SetNextWindowPos(ImVec2(currentPos.x + xOffset, currentPos.y + yOffset));

	ImGui::BeginChild("##AnimationVariables", paneWidth);
	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetCursorScreenPos(),
		ImGui::GetCursorScreenPos() + paneWidth,
		ImColor(ImGui::GetStyle().Colors[ImGuiCol_WindowBg]));

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

	for (auto& [name, variable] : mySchema->GetVariables())
	{
		ImGui::TextUnformatted(name.c_str());
		ImGui::SameLine();
		ScriptGraphDataTypeRegistry::RenderEditInPlaceWidget(variable->GetTypeData()->GetType(), variable->Data.GetUUID(), variable->Data.Ptr);
	}

	ImGui::EndChild();
}

std::string newLayerName = "";
bool AnimationControllerEditor::RenderLayerMenu()
{
	bool switchLayer = false;
	float x = ImGui::GetWindowContentRegionWidth();

	ImVec2 currentPos = ImGui::GetWindowPos();

	float yOffset = 50;
	float xOffset = 7;

	ImVec2 paneWidth = ImVec2(250, ImGui::GetWindowContentRegionMax().y - yOffset);
	ImGui::SetNextWindowPos(ImVec2(currentPos.x + xOffset, currentPos.y + yOffset));

	ImGui::BeginChild("##AnimationLayers", paneWidth);

	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetCursorScreenPos(),
		ImGui::GetCursorScreenPos() + paneWidth,
		ImColor(ImGui::GetStyle().Colors[ImGuiCol_WindowBg]));

	ImGui::Spacing();
	ImGui::Text("Layers");
	ImGui::Spacing();

	int index = -1;
	std::vector<const char*> layerNames;
	int i = 0;
	for (auto& [name, layer] : myCurrentController->GetLayers())
	{
		layerNames.push_back(name.c_str());
		if (layer == myGraph) index = i;
		++i;
	}

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1));
	if (ImGui::BeginListBox("##Layer", ImVec2(230, 100)))
	{
		ImGui::PopStyleColor();

		i = 0;
		for (auto& [name, layer] : myCurrentController->GetLayers())
		{
			if (ImGui::Selectable(layerNames[i], i == index))
			{
				SetGraph(myCurrentController->GetLayers().at(layerNames[i]), myCurrentGraphPath);
				//myCurrentController->SetActiveLayer(layerNames[i]);
			}
			ImGui::SameLine();
			std::string checkboxid = "##active";
			checkboxid += name;
			ImGui::Checkbox(checkboxid.c_str(), &layer->isActive);
			++i;
		}
		ImGui::EndListBox();
	}
	else ImGui::PopStyleColor();

	if (ImGui::Button("Remove Layer") && index != -1 && layerNames.size() > 1)
	{
		myGraph->GetController()->RemoveLayer(std::string(layerNames[index]));
		if (myCurrentController->GetLayers().size() > 0)
		{
			SetGraph(myCurrentController->GetLayers().begin()->second, myCurrentGraphPath);
		}
		else
		{
			myGraph = nullptr;
			mySchema = nullptr;
			myCurrentGraphPath = "";
		}
	}

	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##NewLayerName", &newLayerName);
	ImGui::SameLine();
	if (ImGui::Button("New Layer") && !newLayerName.empty())
	{
		myGraph->GetController()->AddLayer(newLayerName);
		newLayerName.clear();
	}

	ShowBoneList(0);

	ImGui::EndChild();
	return switchLayer;
}


void AnimationControllerEditor::HandleEditorCreate()
{
	if (ImNodeEd::BeginCreate())
	{
		ImNodeEd::PinId startLinkId, endLinkId;

		// This returns True constantly while trying to create a link, even before we've selected an end pin.
		if (ImNodeEd::QueryNewLink(&startLinkId, &endLinkId))
		{
			if (startLinkId && endLinkId)
			{

				std::string canCreateEdgeFeedback;
				if (!mySchema->CanCreateEdge(startLinkId.Get(), endLinkId.Get(), canCreateEdgeFeedback))
				{
					ImNodeEd::RejectNewItem(ImColor(255, 0, 0, 255));
				}

				// This is true if we've made a link between two pins. I.e. when we release LMB to make a link.
				if (ImNodeEd::AcceptNewItem())
				{
					mySchema->CreateEdge(startLinkId.Get(), endLinkId.Get());
				}
			}
		}
	}
	ImNodeEd::EndCreate();
}

void AnimationControllerEditor::CreateNode()
{
	std::shared_ptr<AnimationGraphNode> newNode = mySchema->AddNode(std::make_shared<AnimationGraphNode>());
	if (newNode)
	{
		const auto uuidAwareNewNode = AsUUIDAwareSharedPtr(newNode);
		const ImVec2 mousePos = ed::ScreenToCanvas(ImGui::GetMousePos());
		ed::SetNodePosition(uuidAwareNewNode->GetUID(), mousePos);
	}
}


void AnimationControllerEditor::Update(float aDeltaTime)
{
	if (myState.isTicking)
	{
		myState.flowShowFlow = true;
		//myGraph->Tick(aDeltaTime);
	}
	if (Input::GetKeyHeld(KeyCode::CTRL))
	{
		if (Input::GetKeyPress(KeyCode::C))
		{
			//Copy();
		}
		else if (Input::GetKeyPress(KeyCode::V))
		{
			//Paste();
		}
	}
}


int index = 0;
void AnimationControllerEditor::ShowBoneList(int /*aId*/)
{
	auto& skeleton = myCurrentController->GetSkeleton();
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1));
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Bone mask");
	ImGui::BeginListBox("##Bone mask", ImVec2(230, 100));
	ImGui::PopStyleColor();
	for (size_t i = 0; i < myGraph->myBoneMaskRootIDs.size(); i++)
	{
		ImGui::SetNextItemWidth(190);
		if (ImGui::BeginCombo(("##BoneRoot" + std::to_string(i)).c_str(), skeleton->boneNames[myGraph->myBoneMaskRootIDs[i]].c_str()))
		{
			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			ImGui::InputText("Search", &searchTerm);
			for (int j = 0; j < skeleton->boneNames.size(); j++)
			{
				bool isSelected = (skeleton->boneNames[index] == skeleton->boneNames[j]);

				if (Catbox::ToLowerString(skeleton->boneNames[j]).find(Catbox::ToLowerString(searchTerm)) != std::string::npos)
				{
					if (ImGui::Selectable((skeleton->boneNames[j]).c_str(), false))
					{
						index = j;
						myGraph->myBoneMaskRootIDs[i] = skeleton->boneNameToIndex.at(skeleton->boneNames[index]);
						myGraph->RegenerateBoneMask(skeleton);
						searchTerm.clear();
					}
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();
		if (ImGui::Button(("X##" + std::to_string(i)).c_str(), ImVec2(19, 19)))
		{
			myGraph->myBoneMaskRootIDs.erase(myGraph->myBoneMaskRootIDs.begin() + i--);
			myGraph->RegenerateBoneMask(skeleton);
		}
	}
	ImGui::EndListBox();

	if (ImGui::Button("Add new mask root"))
	{
		myGraph->myBoneMaskRootIDs.push_back(0);
	}
}


void AnimationControllerEditor::RightClickMenu()
{
	const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

	ImGui::PushStyleColor(ImGuiCol_PopupBg, pinBackground);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 12));

	if (!ImNodeEd::GetHoveredNode().Get())
	{
		if (ImGui::BeginPopup("BackgroundContextMenu"))
		{
			if (ImGui::TreeNodeEx("New State      ", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding))
			{
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					CreateNode();
					ImGui::CloseCurrentPopup();
				}
				ImGui::TreePop();
			}

			ImGui::EndPopup();
		}
	}


	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(1);
}

void AnimationControllerEditor::Init()
{
	nodeEditorContext = ImNodeEd::CreateEditor(&nodeEditorCfg);
	myState.initNavToContent = true;


	myOnSceneLoadedListener.action = [this] {ClearController(); };
	Engine::GetInstance()->GetSceneManager()->AddOnSceneLoadedListener(myOnSceneLoadedListener);

}

void AnimationControllerEditor::LoadGraph(AnimationController* aController, const std::string& aPath)
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);

	std::ifstream file(aPath);

	const std::string inGraph = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();

	AnimationGraphSchema::DeserializeAnimationGraph(aController, inGraph);
}


void AnimationControllerEditor::SaveGraph(AnimationController* aController, const std::string& aPath)
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);
	std::string outGraph;
	AnimationGraphSchema::SerializeAnimationGraph(aController, outGraph);
	DWORD attributes = GetFileAttributesA((LPCSTR)aPath.c_str());
	if (attributes & FILE_ATTRIBUTE_READONLY)
	{
		SetFileAttributesA((LPCSTR)aPath.c_str(), attributes & ~FILE_ATTRIBUTE_READONLY);
	}
	std::ofstream file(aPath);
	file << outGraph;
	file.close();
	ImNodeEd::SetCurrentEditor(nullptr);
}

void AnimationControllerEditor::SetGraph(std::shared_ptr<AnimationGraph>& aGraph, const std::string& aPath)
{
	if (aGraph == myGraph) return;
	if (myCurrentController && aPath != myCurrentGraphPath)
	{
		SaveGraph(myCurrentController, myCurrentGraphPath);
	}

	ImNodeEd::SetCurrentEditor(nodeEditorContext);
	myCurrentGraphPath = aPath;
	myGraph = aGraph;
	mySchema = aGraph->GetGraphSchema();
	UpdateVariableContextMenu();
}

void AnimationControllerEditor::SetSelectedObject(GameObject* anObject)
{
	mySelectedObject = anObject;
}

void AnimationControllerEditor::ClearSelectedObject()
{
	mySelectedObject = nullptr;
	myVariableMenuToggle = false;
}


void AnimationControllerEditor::SaveCurrent()
{
	if (myCurrentController && !myCurrentGraphPath.empty())
	{
		SaveGraph(myCurrentController, myCurrentGraphPath);
	}
}


void AnimationControllerEditor::HandleEditorDelete()
{
	if (ImGui::GetIO().WantCaptureKeyboard) return;
	if (ImNodeEd::BeginDelete())
	{
		ImNodeEd::LinkId deletedLinkId;
		while (ImNodeEd::QueryDeletedLink(&deletedLinkId))
		{
			if (ImNodeEd::AcceptDeletedItem())
			{
				mySchema->RemoveEdge(deletedLinkId.Get());
			}
		}

		if (myGraph->GetNodes().size() > 1 + myGraph->GetAnyStateNodes().size())
		{
			ImNodeEd::NodeId deletedNodeId = 0;
			while (ImNodeEd::QueryDeletedNode(&deletedNodeId))
			{
				if (ImNodeEd::AcceptDeletedItem())
				{
					mySchema->RemoveNode(deletedNodeId.Get());
				}
			}
		}
	}
	ImNodeEd::EndDelete();
}
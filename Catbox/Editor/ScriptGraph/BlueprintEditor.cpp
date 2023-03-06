#include "stdafx.h"
#include "BlueprintEditor.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include "imgui_stdlib.h"


#include <fstream>
#include <iostream>
#include <sstream>

#include "ScriptGraph/ScriptGraphTypes.h"
#include "ScriptGraph/Nodes/SGNode_Variable.h"
#include "ScriptGraph/Nodes/Math/SGNode_MathOps.h"

#include "RegisterExternalTypes.h"

inline ImVec2 operator+(const ImVec2& A, const ImVec2& B)
{
	return { A.x + B.x, A.y + B.y };
}

inline ImVec2 operator-(const ImVec2& A, const ImVec2& B)
{
	return { A.x - B.x, A.y - B.y };
}

inline ImVec2 operator*(const ImVec2& A, const ImVec2& B)
{
	return { A.x * B.x, A.y * B.y };
}

inline ImVec2 operator*(const ImVec2& A, const float S)
{
	return { A.x * S, A.y * S };
}

inline ImVec4 operator/(const ImVec4& A, const float S)
{
	return { A.x / S, A.y / S, A.z / S, A.w / S };
}

void BlueprintEditor::RenderNode(const ScriptGraphNode* aNode)
{
	std::vector<const ScriptGraphPin*> inputPins;
	std::vector<const ScriptGraphPin*> outputPins;

	ImVec2 headerTextSize = { 0, 0 };
	if (!aNode->IsSimpleNode() || aNode->IsExecNode())
	{
		headerTextSize = ImGui::CalcTextSize(aNode->GetNodeTitle().c_str());
	}

	ImVec2 leftMinSize = { 64, 0 };
	ImVec2 rightMinSize = { 64, 0 };
	for (auto& [uid, pin] : aNode->GetPins())
	{
		const ImVec2 currentTextSize = ImGui::CalcTextSize(pin.GetLabel().c_str());

		if (pin.GetPinDirection() == PinDirection::Input)
		{
			if (currentTextSize.x > leftMinSize.x)
				leftMinSize.x = currentTextSize.x;
			if (currentTextSize.y > leftMinSize.y)
				leftMinSize.y = currentTextSize.y;

			inputPins.push_back(&pin);
		}
		else
		{
			if (currentTextSize.x > rightMinSize.x)
				rightMinSize.x = currentTextSize.x;
			if (currentTextSize.y > rightMinSize.y)
				rightMinSize.y = currentTextSize.y;

			outputPins.push_back(&pin);
		}
	}

	std::sort(inputPins.begin(), inputPins.end(), [](const ScriptGraphPin* A, const ScriptGraphPin* B) { return A->GetUID() < B->GetUID(); });
	std::sort(outputPins.begin(), outputPins.end(), [](const ScriptGraphPin* A, const ScriptGraphPin* B) { return A->GetUID() < B->GetUID(); });

	if (leftMinSize.x > 0)
		leftMinSize.x += 32;

	if (rightMinSize.x > 0)
		rightMinSize.x += 32;

	ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodeRounding, 3.0f);
	ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

	const auto uidAwareBase = AsUUIDAwarePtr(aNode);
	ImNodeEd::NodeId currentNodeId = uidAwareBase->GetUID();
	ImNodeEd::BeginNode(currentNodeId);
	ImGui::PushID(static_cast<int>(uidAwareBase->GetUID()));

	if (!aNode->IsSimpleNode() || aNode->IsExecNode())
	{
		ImGui::TextUnformatted(aNode->GetNodeTitle().c_str());
	}
	else
	{
		ImGui::Dummy({ 0, 4 });
	}

	const ImVec2 headerRectMin = ImGui::GetItemRectMin();
	const ImVec2 headerRectMax = ImGui::GetItemRectMax();

	const float bodyMinWidth = leftMinSize.x + rightMinSize.x + 16;
	const ImVec2 nodeBodySize = { headerTextSize.x > bodyMinWidth ? headerTextSize.x : bodyMinWidth, 0 };

	// TEMP FLIP FLOP TO DEBUG TABLE CELL SIZES
	//const ImU32 ColA = IM_COL32(128, 0, 0, 128);
	//const ImU32 ColB = IM_COL32(0, 128, 0, 128);

	ImGui::BeginTable("body", 2, ImGuiTableFlags_SizingStretchProp, nodeBodySize);
	const size_t numRows = inputPins.size() > outputPins.size() ? inputPins.size() : outputPins.size();
	for (size_t row = 0; row < numRows; row++)
	{
		ImGui::TableNextColumn();
		//ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, row % 2 ? ColA : ColB);
		if (row < inputPins.size())
		{
			const ScriptGraphPin* inputPin = inputPins[row];
			RenderPin(inputPin);
		}
		ImGui::TableNextColumn();
		//ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, row % 2 ? ColB : ColA);
		if (row < outputPins.size())
		{
			const ScriptGraphPin* outputPin = outputPins[row];
			RenderPin(outputPin);
		}
		ImGui::TableNextRow();
	}
	ImGui::EndTable();

	const ImVec2 NodeBodySize = ImGui::GetItemRectSize();
	const ImRect headerRect = ImRect(headerRectMin, { headerRectMax.x + NodeBodySize.x - headerTextSize.x, headerRectMax.y + 2 });
	ImNodeEd::EndNode();

	if (!aNode->IsSimpleNode() || aNode->IsExecNode())
	{
		ImDrawList* nodeDrawList = ImNodeEd::GetNodeBackgroundDrawList(uidAwareBase->GetUID());
		const float halfBorderWidth = ImNodeEd::GetStyle().NodeBorderWidth * 0.5f;

		nodeDrawList->AddRectFilled(
			ImVec2(headerRect.GetTL().x - (8 - halfBorderWidth), headerRect.GetTL().y - (4 - halfBorderWidth)),
			ImVec2(headerRect.GetBR().x + (8 - halfBorderWidth), headerRect.GetBR().y),
			ImGui::ColorConvertFloat4ToU32(aNode->GetNodeHeaderColor() / 255.0f),
			ImNodeEd::GetStyle().NodeRounding,
			1 | 2
		);

		const ImRect headerSeparatorRect = ImRect(headerRect.GetBL(), headerRect.GetBR());
		nodeDrawList->AddLine(
			ImVec2(headerSeparatorRect.GetTL().x + (-8 - halfBorderWidth) + 1, headerSeparatorRect.GetTL().y + -0.5f),
			ImVec2(headerSeparatorRect.GetTR().x + (8 - halfBorderWidth) - 1, headerSeparatorRect.GetTR().y + -0.5f),
			ImColor(64, 64, 64, 255), 1.0f);
	}

	ImGui::PopID();

	ImNodeEd::PopStyleVar();
	ImNodeEd::PopStyleVar();

}

void BlueprintEditor::RenderPin(const ScriptGraphPin* aPin)
{
	if (aPin->GetPinDirection() == PinDirection::Input)
	{
		const bool CanConstructInPlace = ScriptGraphDataTypeRegistry::IsTypeInPlaceConstructible(aPin->GetDataType());

		ImNodeEd::BeginPin(aPin->GetUID(), static_cast<ImNodeEd::PinKind>(aPin->GetPinDirection()));
		DrawPinIcon(aPin, ScriptGraphDataTypeRegistry::GetDataTypeColor(aPin->GetDataType()), aPin->IsPinConnected());
		ImNodeEd::EndPin();
		ImGui::SameLine();

		if (aPin->IsLabelVisible())
		{
			const float y = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(y + 3);
			ImGui::TextUnformatted(aPin->GetLabel().c_str());
			ImGui::SetCursorPosY(y);
		}

		if (!aPin->IsPinConnected() && CanConstructInPlace)
		{
			ImGui::SameLine();
			const float y = ImGui::GetCursorPosY();

			ScriptGraphDataTypeRegistry::RenderEditInPlaceWidget(aPin->GetDataType(), aPin->GetUUID(), aPin->GetPtr());

			// Hax to force ImGui to move the cursor back to where it should be.
			ImGui::SetCursorPosY(y + 3);
			ImGui::Dummy({ 0, 0 });
		}
	}
	else
	{
		if (aPin->IsLabelVisible())
		{
			ImGui::SetCursorPosX(
				ImGui::GetCursorPosX()
				+ ImGui::GetColumnWidth()
				- ImGui::CalcTextSize(aPin->GetLabel().c_str()).x
				- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x
				- 14.0f
			);

			const float y = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(y + 3);
			ImGui::TextUnformatted(aPin->GetLabel().c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosY(y);
		}
		else
		{
			ImGui::SetCursorPosX(
				ImGui::GetCursorPosX()
				+ ImGui::GetColumnWidth()
				- ImGui::GetScrollX() - ImGui::GetStyle().ItemSpacing.x
				- 14.0f
			);
		}

		ImNodeEd::BeginPin(aPin->GetUID(), static_cast<ImNodeEd::PinKind>(aPin->GetPinDirection()));
		DrawPinIcon(aPin, ScriptGraphDataTypeRegistry::GetDataTypeColor(aPin->GetDataType()), aPin->IsPinConnected());
		ImNodeEd::EndPin();
	}
}

void BlueprintEditor::DrawPinIcon(const ScriptGraphPin* aPin, ImVec4 aPinColor, bool isConnected) const
{
	const PinIcon icon = aPin->GetType() == ScriptGraphPinType::Exec ? PinIcon::Exec : PinIcon::Circle;

	const float iconSize = 24.0f;
	const ImVec2 sizeRect = ImVec2(iconSize, iconSize);
	if (ImGui::IsRectVisible(sizeRect))
	{
		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImRect drawRect = { cursorPos, { cursorPos.x + iconSize, cursorPos.y + iconSize } };
		const float outlineScale = drawRect.GetWidth() / (iconSize * 2.0f);
		const int circleExtraSegments = static_cast<int>(round(2 * outlineScale));

		unsigned color = IM_COL32(aPinColor.x, aPinColor.y, aPinColor.z, aPinColor.w);

		const ImColor innerColor(32, 32, 32, 255);

		switch (icon)
		{
		case PinIcon::Exec:
		{
			const auto origin_scale = drawRect.GetWidth() / iconSize;

			const auto offset_x = 1.0f * origin_scale;
			const auto offset_y = 0.0f * origin_scale;
			const auto margin = (isConnected ? 2.0f : 2.0f) * origin_scale;
			const auto rounding = 0.1f * origin_scale;
			const auto tip_round = 0.7f;

			const float canvasX = drawRect.GetTL().x + margin + offset_x;
			const float canvasY = drawRect.GetTL().y + margin + offset_y;
			const float canvasW = canvasX + drawRect.GetWidth() - margin * 2.0f;
			const float canvasH = canvasY + drawRect.GetWidth() - margin * 2.0f;

			const auto canvas = ImRect(canvasX, canvasY, canvasW, canvasH);

			const auto left = canvas.GetTL().x + canvas.GetWidth() * 0.5f * 0.3f;
			const auto right = canvas.GetTL().x + canvas.GetWidth() - canvas.GetWidth() * 0.5f * 0.3f;
			const auto top = canvas.GetTL().y + canvas.GetHeight() * 0.5f * 0.2f;
			const auto bottom = canvas.GetTL().y + canvas.GetHeight() - canvas.GetHeight() * 0.5f * 0.2f;
			const auto center_y = (top + bottom) * 0.5f;

			const auto tip_top = ImVec2(canvas.GetTL().x + canvas.GetWidth() * 0.5f, top);
			const auto tip_right = ImVec2(right, center_y);
			const auto tip_bottom = ImVec2(canvas.GetTL().x + canvas.GetWidth() * 0.5f, bottom);

			drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
			drawList->PathBezierCurveTo(
				ImVec2(left, top),
				ImVec2(left, top),
				ImVec2(left, top) + ImVec2(rounding, 0));
			drawList->PathLineTo(tip_top);
			drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
			drawList->PathBezierCurveTo(
				tip_right,
				tip_right,
				tip_bottom + (tip_right - tip_bottom) * tip_round);
			drawList->PathLineTo(tip_bottom);
			drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
			drawList->PathBezierCurveTo(
				ImVec2(left, bottom),
				ImVec2(left, bottom),
				ImVec2(left, bottom) - ImVec2(0, rounding));

			if (!isConnected)
			{
				if (innerColor & 0xFF000000)
					drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

				drawList->PathStroke(color, true, 2.0f * outlineScale);
			}
			else
				drawList->PathFillConvex(color);
		}
		break;
		case PinIcon::Circle:
		{
			const float triangleStart = (drawRect.GetTL().x + iconSize / 2) * 0.32f * drawRect.GetWidth();
			drawRect.Min.x -= static_cast<int>(round(drawRect.GetWidth() * 0.25f * 0.25f));
			const ImVec2 center = { drawRect.GetTL().x + sizeRect.x / 2, drawRect.GetTL().y + sizeRect.y / 2 };

			if (!isConnected)
			{
				const auto r = 0.5f * drawRect.GetWidth() / 2.0f - 0.5f;

				if (innerColor & 0xFF000000)
					drawList->AddCircleFilled(center, r, innerColor, 12 + circleExtraSegments);
				drawList->AddCircle(center, r, color, 12 + circleExtraSegments, 2.0f * outlineScale);
			}
			else
				drawList->AddCircleFilled(center, 0.5f * drawRect.GetWidth() / 2.0f, color, 12 + circleExtraSegments);
		}
		break;
		}
	}

	ImGui::Dummy(sizeRect);
}


void BlueprintEditor::Render()
{

	ImNodeEd::SetCurrentEditor(nodeEditorContext);
	// BUG: Breaks if the window is too small, like with brand new window is not present in imgui.ini!
	// Fixed this myself by editing ImNodeEd::Begin() to return false if canvas clipping fails.
	// Have posted on his GitHub about that @ https://github.com/thedmd/imgui-node-editor/issues/191
	ImGui::SetNextWindowSize({ 1280, 720 }, ImGuiCond_FirstUseEver);
	if (myGraph)
	{
		ImGui::SetNextItemWidth(50);
		if (ImGui::Button(">"))
		{
			ImGui::OpenPopup("TriggerEntryPoint");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Open the Trigger Entry Point dialog.");
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Should Tick", &myState.isTicking))
		{
			myGraph->SetTicking(myState.isTicking);
		}

		ImGui::SameLine();
		if (ImGui::Button("Variables"))
		{
			ImGui::OpenPopup("Edit Variables");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Open the Edit Variables dialog.");
		}

		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			SaveGraph(myGraph, myCurrentGraphPath);
			ImNodeEd::SetCurrentEditor(nodeEditorContext);
		}

		if (ImNodeEd::Begin("A Node Editor"))
		{
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

			for (const auto& [edgeId, edge] : myGraph->GetEdges())
			{
				const ImVec4 normalizedColor = ImVec4(edge.Color.x / 255, edge.Color.y / 255, edge.Color.z / 255, edge.Color.w / 255);
				ImNodeEd::Link(edge.EdgeId, edge.FromUID, edge.ToUID, normalizedColor, edge.Thickness);
			}

			if (myState.flowShowFlow || !myGraph->GetLastExecutedPath().empty())
			{
				for (const auto& edgeId : myGraph->GetLastExecutedPath())
				{
					ImNodeEd::Flow(edgeId, ax::NodeEditor::FlowDirection::Forward);
				}
				myState.flowShowFlow = false;
				//myState.lastFlowCount = myGraph->GetLastExecutedPath().size();
				myGraph->ResetLastExecutedPath();
			}

			HandleEditorCreate();
			HandleEditorDelete();

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

			BackgroundContextMenu();
			//NodeContextMenu();

			ImNodeEd::Resume();

			if (myState.initNavToContent)
			{
				ImNodeEd::NavigateToContent();
				myState.initNavToContent = false;
			}

			ImNodeEd::End();
		}

		TriggerEntryPoint();
		EditVariables();
	}

	//ImGui::End();
	ImNodeEd::SetCurrentEditor(nullptr);
}


// Copied from MuninUtils.
// Measures differences in two strings.
template<typename T>
std::enable_if_t<std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, typename T::size_type>
LevenshteinDistance(const T& aString, const T& aSearchString)
{
	if (aString.size() > aSearchString.size())
	{
		return LevenshteinDistance(aSearchString, aString);
	}

	using TSize = typename T::size_type;
	const TSize minSize = aString.size();
	const TSize maxSize = aSearchString.size();
	std::vector<TSize> levenDistance(minSize + 1);

	for (TSize s = 0; s <= minSize; ++s)
	{
		levenDistance[s] = s;
	}

	for (TSize s = 1; s <= maxSize; ++s)
	{
		TSize lastDiag = levenDistance[0], lastDiagMem;
		++levenDistance[0];

		for (TSize t = 1; t <= minSize; ++t)
		{
			lastDiagMem = levenDistance[t];
			if (aString[t - 1] == aSearchString[s - 1])
			{
				levenDistance[t] = lastDiag;
			}
			else
			{
				levenDistance[t] = std::min(std::min(levenDistance[t - 1], levenDistance[t]), lastDiag) + 1;
			}
			lastDiag = lastDiagMem;
		}
	}

	return levenDistance[minSize];
}



void BlueprintEditor::UpdateVariableContextMenu()
{
	myState.bgCtxtVariablesCategory.Items.clear();
	myState.bgCtxtVariablesCategory.Title = "Variables";
	const ScriptGraphNodeType& GetNodeType = ScriptGraphSchema::GetNodeTypeByClass<SGNode_GetVariable>();
	const ScriptGraphNodeType& SetNodeType = ScriptGraphSchema::GetNodeTypeByClass<SGNode_SetVariable>();
	const auto getNodeUUID = AsUUIDAwareSharedPtr(GetNodeType.DefaultObject);
	const auto setNodeUUID = AsUUIDAwareSharedPtr(SetNodeType.DefaultObject);
	for (const auto& [varId, var] : mySchema->GetVariables())
	{

		myState.bgCtxtVariablesCategory.Items.push_back({ "Get " + var->Name, getNodeUUID->GetTypeName(), varId });
		myState.bgCtxtVariablesCategory.Items.push_back({ "Set " + var->Name, setNodeUUID->GetTypeName(), varId });
	}
}

void BlueprintEditor::BackgroundContextMenu()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("BackgroundContextMenu"))
	{
		//ImGui::PushItemWidth(100.0f);
		myState.bgCtxtSearchFieldChanged = ImGui::InputText("##nodeSearch", &myState.bgCtxtSearchField);
		if (myState.bgCtxtSearchFocus)
		{
			ImGui::SetKeyboardFocusHere(0);
			myState.bgCtxtSearchFocus = false;
		}
		//ImGui::PopItemWidth();

		if (myState.bgCtxtSearchField.empty())
		{
			for (const std::string& catName : myBgContextCategoryNamesList)
			{
				const auto catIt = myBgContextCategories.find(catName);
				if (ImGui::TreeNodeEx(catName.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					for (const auto& item : catIt->second.Items)
					{
						bool shouldShow = false;
						for (int i = 0; i < myComponents.size(); i++)
						{
							if (myComponents[i] == item.Acessibility)
							{
								shouldShow = true;
								i = myComponents.size();
							}
						}
						if (item.Acessibility == "Public" || shouldShow)
						{
							if (ImGui::TreeNodeEx(item.Title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
							{
								if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
								{
									const ScriptGraphNodeType& type = ScriptGraphSchema::GetSupportedNodeTypes().find(item.Value)->second;
									std::shared_ptr<ScriptGraphNode> newNode = mySchema->AddNode(type.New());
									if (newNode)
									{
										const auto uuidAwareNewNode = AsUUIDAwareSharedPtr(newNode);
										const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());
										ImNodeEd::SetNodePosition(uuidAwareNewNode->GetUID(), mousePos);
									}
									ImGui::CloseCurrentPopup();
								}
								ImGui::TreePop();
							}
						}
					}
					ImGui::TreePop();
				}
			}

			if (ImGui::TreeNodeEx(myState.bgCtxtVariablesCategory.Title.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
			{
				for (const auto& item : myState.bgCtxtVariablesCategory.Items)
				{
					if (ImGui::TreeNodeEx(item.Title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
					{
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						{
							const ScriptGraphNodeType& type = ScriptGraphSchema::GetSupportedNodeTypes().find(item.Value)->second;

							std::shared_ptr<ScriptGraphNode> newNode;
							if (type.Type == typeid(SGNode_GetVariable))
								newNode = mySchema->AddGetVariableNode(item.Tag);
							else
								newNode = mySchema->AddSetVariableNode(item.Tag);

							const auto uuidAwareNewNode = AsUUIDAwareSharedPtr(newNode);
							const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());
							ImNodeEd::SetNodePosition(uuidAwareNewNode->GetUID(), mousePos);
							ImGui::CloseCurrentPopup();
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
		else
		{
			if (myState.bgCtxtSearchFieldChanged)
			{
				myState.bgCtxtSearchFieldResults.clear();
				const auto supportedNodeTypes = ScriptGraphSchema::GetSupportedNodeTypes();
				for (const auto& [nodeName, nodeType] : supportedNodeTypes)
				{
					if (nodeType.DefaultObject->IsInternalOnly())
						continue;

					myState.bgCtxtSearchFieldResults.push_back({
						nodeType.DefaultObject->GetNodeTitle(),
						nodeType.TypeName,
						0,
						""
						});

					myState.bgCtxtSearchFieldResults.back().Rank = LevenshteinDistance(myState.bgCtxtSearchFieldResults.back().Title, myState.bgCtxtSearchField);
				}

				for (const auto& varItem : myState.bgCtxtVariablesCategory.Items)
				{
					myState.bgCtxtSearchFieldResults.push_back({
						varItem.Title,
						varItem.Value,
						0,
						varItem.Tag
						});

					myState.bgCtxtSearchFieldResults.back().Rank = LevenshteinDistance(myState.bgCtxtSearchFieldResults.back().Title, myState.bgCtxtSearchField);
				}

				std::sort(myState.bgCtxtSearchFieldResults.begin(), myState.bgCtxtSearchFieldResults.end(),
					[](const SearchMenuItem& A, const SearchMenuItem& B)
					{
						return A.Rank < B.Rank;
					});
			}

			for (auto& item : myState.bgCtxtSearchFieldResults)
			{
				if (ImGui::TreeNodeEx(item.Title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						const ScriptGraphNodeType& type = ScriptGraphSchema::GetSupportedNodeTypes().find(item.Value)->second;
						std::shared_ptr<ScriptGraphNode> newNode = nullptr;

						if (type.Type == typeid(SGNode_GetVariable))
						{
							newNode = mySchema->AddGetVariableNode(item.Tag);
						}
						else if (type.Type == typeid(SGNode_SetVariable))
						{
							newNode = mySchema->AddSetVariableNode(item.Tag);
						}
						else
						{
							newNode = mySchema->AddNode(type.New());
						}

						const auto uuidAwareNewNode = AsUUIDAwareSharedPtr(newNode);
						const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());
						ImNodeEd::SetNodePosition(uuidAwareNewNode->GetUID(), mousePos);
						myState.bgCtxtSearchFieldChanged = false;
						myState.bgCtxtSearchField.clear();
						ImGui::CloseCurrentPopup();
					}
					ImGui::TreePop();
				}
			}
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void BlueprintEditor::NodeContextMenu(size_t aNodeUID)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("NodeContextMenu"))
	{
		if (ImGui::BeginMenu("Node Context Menu"))
		{
			if (ImGui::MenuItem("Herp")) {}
			if (ImGui::MenuItem("Derp")) {}
			if (ImGui::MenuItem("Slerp")) {}

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void BlueprintEditor::LinkContextMenu(size_t aLinkUID)
{

}

void BlueprintEditor::TriggerEntryPoint()
{
	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	ImGui::SetNextWindowSize({ 512, 256 });
	if (ImGui::BeginPopupModal("TriggerEntryPoint"))
	{
		const std::vector<std::string>& graphEntryPoints = mySchema->GetEntryPoints();
		ImGui::BeginTable("body", 2, ImGuiTableFlags_SizingStretchProp);
		for (const auto& entry : graphEntryPoints)
		{
			// Don't trigger the Tick node.
			if (entry != "Tick")
			{
				ImGui::TableNextColumn();
				if (ImGui::Button(entry.c_str()))
				{
					ImGui::CloseCurrentPopup();
					myState.flowShowFlow = true;
					ClearErrorState();
					myGraph->Run(entry);
				}
			}
		}
		ImGui::EndTable();

		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void BlueprintEditor::EditVariables()
{
	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.15f, 0.15f, 0.15f, 1));
	ImGui::SetNextWindowSize({ 1024, 512 });
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

		const float x = ImGui::GetCursorPosX();
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
void BlueprintEditor::LoadGraph(std::shared_ptr<ScriptGraph>& aGraph, const std::string& aPath)
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);

	std::ifstream file(aPath);

	const std::string inGraph = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();

	ScriptGraphSchema::DeserializeScriptGraph(aGraph, inGraph, true);

	ImNodeEd::SetCurrentEditor(nullptr);
}

void BlueprintEditor::SetGraph(std::shared_ptr<ScriptGraph>& aGraph, const std::string& aPath)
{
	if (aGraph == myGraph) return;
	if (myGraph)
	{
		SaveGraph(myGraph, myCurrentGraphPath);
	}

	ImNodeEd::SetCurrentEditor(nodeEditorContext);
	myCurrentGraphPath = aPath;
	myGraph = aGraph;
	mySchema = aGraph->GetGraphSchema();
	UpdateVariableContextMenu();
	ImNodeEd::SetCurrentEditor(nullptr);
}

void BlueprintEditor::SaveGraph(const std::shared_ptr<ScriptGraph> aGraph, const std::string& aPath)
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);
	std::string outGraph;
	ScriptGraphSchema::SerializeScriptGraph(aGraph, outGraph);
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

void BlueprintEditor::SaveCurrent()
{
	if (myGraph && !myCurrentGraphPath.empty())
	{
		SaveGraph(myGraph, myCurrentGraphPath);
	}
}


void BlueprintEditor::HandleEditorCreate()
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


void BlueprintEditor::HandleEditorDelete()
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

		ImNodeEd::NodeId deletedNodeId = 0;
		while (ImNodeEd::QueryDeletedNode(&deletedNodeId))
		{
			if (ImNodeEd::AcceptDeletedItem())
			{
				mySchema->RemoveNode(deletedNodeId.Get());
			}
		}
	}
	ImNodeEd::EndDelete();
}

void BlueprintEditor::Copy()
{
	myCopyVector.clear();
	ImNodeEd::SetCurrentEditor(nodeEditorContext);
	myCopyVector.resize(ImNodeEd::GetNodeCount());
	ImNodeEd::GetSelectedNodes(myCopyVector.data(), myCopyVector.size());
}

void BlueprintEditor::Paste()
{
	ImNodeEd::SetCurrentEditor(nodeEditorContext);
	ImVec2 middleOfGroupPos = { 0,0 };
	int count = 0;
	for (int i = 0; i < myCopyVector.size(); i++)
	{
		if ((size_t)myCopyVector[i] != 0)
		{
			middleOfGroupPos.x += ImNodeEd::GetNodePosition(myCopyVector[i]).x;
			middleOfGroupPos.y += ImNodeEd::GetNodePosition(myCopyVector[i]).y;
			count++;
		}
	}
	middleOfGroupPos.x /= count;
	middleOfGroupPos.y /= count;
	for (int i = 0; i < count; i++)
	{
		std::shared_ptr<ScriptGraphNode> copyNode = myGraph->GetNodes().find((size_t)myCopyVector[i])->second;
		std::shared_ptr<ScriptGraphNode> newNode = nullptr;
		const auto uuidBase = AsUUIDAwareSharedPtr(copyNode);
		std::string aType = uuidBase->GetTypeName();
		const ScriptGraphNodeType& type = ScriptGraphSchema::GetSupportedNodeTypes().find(aType)->second;
		std::string aaTsdsd = type.Type.name();
		if (aType == "SGNode_SetVariable")
		{
			std::string varibleName = "Undefined";
			for (auto& pin : copyNode->GetPins())
			{
				if (pin.second.GetType() == ScriptGraphPinType::Variable)
				{
					if (pin.second.GetLabel() != "Get")
					{
						varibleName = pin.second.GetLabel();
						newNode = mySchema->AddSetVariableNode(varibleName);
						std::string aString = pin.second.GetSimpleDataName();

						if (aString != "nullptr")
						{
							void* aPtr = nullptr;
							size_t aSize = 0;
							if (aString == "std::basic_string")
							{
								aPtr = new std::string();
								aSize = sizeof(std::string);
							}
							else
							{
								aPtr = new void*;
								aSize = pin.second.GetDataSize();
							}
							pin.second.GetRawData(aPtr, aSize);
							newNode->SetRawPinData(pin.second.GetLabel(), aPtr, aSize);
							delete aPtr;
							break;
						}
					}
				}
			}
		}
		else if (aType == "SGNode_GetVariable")
		{
			std::string varibleName = "Undefined";
			for (auto& pin : copyNode->GetPins())
			{
				if (pin.second.GetType() == ScriptGraphPinType::Variable)
				{
					varibleName = pin.second.GetLabel();
					newNode = mySchema->AddGetVariableNode(varibleName);
					break;
				}
			}
		}
		else
		{
			newNode = mySchema->AddNode(type.New());
			for (auto& pin : copyNode->GetPins())
			{
				std::string aString = pin.second.GetSimpleDataName();
				size_t aSize = 0;

				if (aString != "nullptr")
				{
					void* aPtr = nullptr;
					if (aString == "std::basic_string")
					{
						aPtr = new std::string();
						aSize = sizeof(std::string);
					}
					else
					{
						aPtr = new void*;
						aSize = pin.second.GetDataSize();
					}
					pin.second.GetRawData(aPtr, aSize);
					newNode->SetRawPinData(pin.second.GetLabel(), aPtr, aSize);

					delete aPtr;
				}
			}
		}
		if (newNode)
		{
			const auto uuidAwareNewNode = AsUUIDAwareSharedPtr(newNode);
			const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos()) + (ImNodeEd::GetNodePosition(myCopyVector[i]) - middleOfGroupPos);
			ImNodeEd::SetNodePosition(uuidAwareNewNode->GetUID(), mousePos);
		}
	}
}

void BlueprintEditor::Init()
{
	//nodeEditorCfg.SettingsFile = "TestNodeCfg.json";
	nodeEditorContext = ImNodeEd::CreateEditor(&nodeEditorCfg);

	if (myBgContextCategories.empty())
	{
		const auto supportedNodeTypes = ScriptGraphSchema::GetSupportedNodeTypes();
		for (const auto& [nodeName, nodeType] : supportedNodeTypes)
		{
			if (nodeType.DefaultObject->IsInternalOnly())
				continue;

			const std::string nodeCategory = nodeType.DefaultObject->GetCategory();
			const auto uuidBase = AsUUIDAwareSharedPtr(nodeType.DefaultObject);
			if (auto catIt = myBgContextCategories.find(nodeCategory); catIt != myBgContextCategories.end())
			{
				catIt->second.Items.push_back({ nodeType.DefaultObject->GetNodeTitle(), uuidBase->GetTypeName(), "", nodeType.DefaultObject->GetAcessibility() });
			}
			else
			{
				myBgContextCategories.insert({ nodeCategory, {nodeCategory, {{ nodeType.DefaultObject->GetNodeTitle(), uuidBase->GetTypeName() }} } });
				myBgContextCategoryNamesList.push_back(nodeCategory);
			}
		}



		std::sort(myBgContextCategoryNamesList.begin(), myBgContextCategoryNamesList.end());
	}

	myState.bgCtxtSearchFieldResults.reserve(ScriptGraphSchema::GetSupportedNodeTypes().size());
	myState.initNavToContent = true;
}

void BlueprintEditor::Update(float aDeltaTime)
{
	if (myState.isTicking)
	{
		myState.flowShowFlow = true;
		myGraph->Tick(aDeltaTime);
	}
	if (Input::GetKeyHeld(KeyCode::CTRL))
	{
		if (Input::GetKeyPress(KeyCode::C))
		{
			Copy();
		}
		else if (Input::GetKeyPress(KeyCode::V))
		{
			Paste();
		}
	}
}

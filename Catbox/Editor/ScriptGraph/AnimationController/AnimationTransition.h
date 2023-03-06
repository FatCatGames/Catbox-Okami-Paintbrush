#pragma once
#include "MuninGraph.h"
#include "ScriptGraph\ScriptGraphVariable.h"
#include "ScriptGraph\ScriptGraphTypes.h"

enum ComparisonOp
{
	Less,
	Equals,
	More
};


struct TransitionCondition
{
private:
	TransitionCondition() = default;

public:
	ComparisonOp myOp = ComparisonOp::Equals;
	std::shared_ptr<ScriptGraphVariable> myLeftHandValue;
	ScriptGraphVariable myRightHandValue;

	TransitionCondition(std::shared_ptr<ScriptGraphVariable> aValue)
	{
		myLeftHandValue = aValue;
		myRightHandValue = *aValue;
		myRightHandValue.ResetVariable();
	}

	bool ConditionMet()
	{
		const std::string& typeName = myLeftHandValue->Data.TypeData->GetTypeName();
		if (typeName == "int") return Compare<int>();
		if (typeName == "float") return Compare<float>();
		if (typeName == "bool") return Compare<bool>();
		if (typeName == "std::string") return Compare<std::string>();
		return false;
	}

	template<typename T>
	bool Compare()
	{
		T& comp1 = *static_cast<T*>(myLeftHandValue->Data.Ptr);
		T& comp2 = *static_cast<T*>(myRightHandValue.Data.Ptr);
		switch (myOp)
		{
		case ComparisonOp::Less: return comp1 < comp2;
		case ComparisonOp::Equals: return comp1 == comp2;
		case ComparisonOp::More: return comp1 > comp2;
		}
		return false;
	}
};

struct AnimationTransition
{
	bool HasExitTime = false;
	size_t EdgeId;
	AnimationTransition() = default;
	AnimationTransition(size_t anEdgeID, std::shared_ptr<AnimationGraphNode> aToNode)
	{
		EdgeId = anEdgeID;
		myToNode = aToNode;
	}

	std::shared_ptr<AnimationGraphNode> GetToNode() { return myToNode; }
	std::vector<TransitionCondition>& GetConditions() { return myConditions; }

	TransitionCondition& AddCondition(std::shared_ptr<ScriptGraphVariable> aValue)
	{
		myConditions.emplace_back(aValue);
		return myConditions.back();
	}

	void Render(AnimationGraphSchema& aSchema)
	{
		ImGui::Checkbox("Has Exit Time", &HasExitTime);
		ImGui::Separator();
		ImGui::Text("Conditions");
		ImGui::Spacing();
		for (size_t i = 0; i < myConditions.size(); i++)
		{
			if (ImGui::Button(("X##Erase" + std::to_string(i)).c_str()))
			{
				myConditions.erase(myConditions.begin() + i--);
				continue;
			}
			ImGui::SameLine();
			ImGui::Text(myConditions[i].myLeftHandValue->Name.c_str());
			ImGui::SameLine();
			
			const std::string& typeName = myConditions[i].myLeftHandValue->Data.TypeData->GetTypeName();
			void* comparisonData = myConditions[i].myRightHandValue.Data.Ptr;

			if (typeName == "int" || typeName == "float")
			{
				const char* comparisonOps[]
				{
					"Less",
					"Equal",
					"More"
				};

				int opIndex = static_cast<int>(myConditions[i].myOp);
				ImGui::SetNextItemWidth(70);
				if (ImGui::Combo(("##ComparisonOp" + std::to_string(i)).c_str(), &opIndex, comparisonOps, 3))
				{
					myConditions[i].myOp = (ComparisonOp)opIndex;
				}

				ImGui::SameLine();
				ImGui::SetNextItemWidth(70);
				if (typeName == "int")
				{
					int* val = static_cast<int*>(comparisonData);
					ImGui::DragInt(("##ComparisonValue" + std::to_string(i)).c_str(), val);
				}
				else
				{
					float* val = static_cast<float*>(comparisonData);
					ImGui::DragFloat(("##ComparisonValue" + std::to_string(i)).c_str(), val);
				}
			}
			else if (typeName == "bool")
			{
				bool* val = static_cast<bool*>(comparisonData);
				ImGui::Checkbox(("##ComparisonValue" + std::to_string(i)).c_str(), val);
			}
			else if (typeName == "std::string")
			{
				std::string* val = static_cast<std::string*>(comparisonData);
				ImGui::InputText(("##ComparisonValue" + std::to_string(i)).c_str(), val);
			}
		}


		if (ImGui::Button("Add condition"))
		{
			ImGui::OpenPopup("##VariableDropdown");
		}

		if (ImGui::BeginPopup("##VariableDropdown"))
		{	
			for (auto& variable : aSchema.GetVariables())
			{
				if (ImGui::Selectable(variable.first.c_str()))
				{
					myConditions.emplace_back(variable.second);
					myConditions.back().myLeftHandValue = variable.second;
					myConditions.back().myOp = ComparisonOp::Equals;
				}
			}

			ImGui::EndPopup();
		}
	}

private:
	std::vector<TransitionCondition> myConditions;
	std::shared_ptr<AnimationGraphNode> myToNode;
};
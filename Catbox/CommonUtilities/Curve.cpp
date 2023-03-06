#include "stdafx.h"
#include "Curve.h"
#include <cassert>
#include <algorithm>
#include "../CommonUtilities/UtilityFunctions.hpp"
#include <string>

Curve::Curve(const std::vector<Coord>& someCoords)
{
	myCoords = someCoords;
	std::sort(myCoords.begin(), myCoords.end(), [](const Coord& aFirst, const Coord& aSecond)
		{
			return aFirst.x > aSecond.x;
		});
}


float Curve::Evaluate(float aX)
{
	aX = Catbox::Clamp(aX, 0.f, 1.f);
	switch (myInterpolationMode)
	{
	case(InterpolationMode::Linear):
	{
		for (size_t i = 0; i < myCoords.size(); i++)
		{
			if (aX < myCoords[i].x)
			{
				float percent = (aX - myCoords[i - 1].x) / (myCoords[i].x - myCoords[i - 1].x);
				return Catbox::Lerp(myCoords[i - 1].y, myCoords[i].y, percent);
			}
		}

		if (aX == 1 && !myCoords.empty()) return myCoords.back().y;
		break;
	}
	case(InterpolationMode::Cubic):
	{
		assert(true && "cubic interpolation not implemented");
		break;
	}
	}
	return 0;
}

void Curve::AddCoord(const Coord& aCoord)
{
	size_t i = 0;
	for (; i < myCoords.size(); i++)
	{
		if (aCoord.x >= myCoords[i].x)
		{
			assert(aCoord.x != myCoords[i].x && "You have 2 points with the same x-coordinate");
			continue;
		}
		myCoords.insert(myCoords.begin() + i, aCoord);
		return;
	}
	myCoords.push_back(aCoord);
}


void Curve::RenderInProperties(const string& aLabel)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImU32 col = ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text));
	float lineThickness = 1.5f;
	const ImVec2 p = ImGui::GetCursorScreenPos();
	float xSizeMul = 214.f;
	float ySizeMul = 19.f / ((myMax - myMin) / 100.f);
	ImVec2 offset = ImVec2(6 + p.x, 19.f / ((myMax - myMin) / 100.f) + p.y);
	float prevX = myCoords[0].x * xSizeMul;
	float prevY = myCoords[0].y * ySizeMul;

	ImGui::Indent(6);
	if (ImGui::Button(("##Curve" + aLabel).c_str(), ImVec2(215, 20))) myEditActive = !myEditActive;
	ImGui::Unindent(6);

	for (size_t i = 1; i < myCoords.size(); i++)
	{
		float currentX = myCoords[i].x * xSizeMul;
		float currentY = myCoords[i].y * ySizeMul;
		draw_list->AddLine(ImVec2(prevX + offset.x, offset.y - prevY), ImVec2(currentX + offset.x, offset.y - currentY), col, lineThickness);
		prevX = currentX;
		prevY = currentY;
	}

	if (myEditActive)
	{
		for (size_t i = 0; i < myCoords.size(); i++)
		{
			ImGui::Text((std::to_string(i) + ":").c_str());
			int x = static_cast<int>(myCoords[i].x * 100);
			int y = static_cast<int>(myCoords[i].y * 100);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50);
			if (i != 0 && i != myCoords.size() - 1)
			{
				if (Catbox::DragInt(("X##" + aLabel + std::to_string(i)).c_str(), &x, 1, 1, 99))
				{
					myCoords[i].x = x / 100.f;
				}

				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					std::sort(myCoords.begin(), myCoords.end(), [](const Coord& aFirst, const Coord& aSecond)
						{
							return aFirst.x < aSecond.x;
						});
				}
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(50);
			if (Catbox::DragInt(("Y##" + aLabel + std::to_string(i)).c_str(), &y, 1, myMin, myMax))
			{
				myCoords[i].y = y / 100.f;
			}

			if (i != 0 && i != myCoords.size() - 1)
			{
				ImGui::SameLine();
				ImGui::SetNextItemWidth(40);
				if (ImGui::Button(("Remove##RemoveCoord" + aLabel + std::to_string(i)).c_str()))
				{
					myCoords.erase(myCoords.begin() + i);
					--i;
				}
			}

			if (i != myCoords.size() - 1)
			{
				ImGui::SameLine();
				if (ImGui::Button(("Insert##InsertCoord" + aLabel + std::to_string(i)).c_str()))
				{
					float xPos = (myCoords[i].x + myCoords[i + 1].x) / 2.f;
					float yPos = (myCoords[i].y + myCoords[i + 1].y) / 2.f;

					AddCoord({ xPos, yPos });
				}
			}
		}
	}

	ImGui::Dummy(ImVec2(0, 5.f));
}

void Curve::LoadFromJson(rapidjson::Value& aJsonObject)
{
	myCoords.clear();
	for (auto& coord : aJsonObject.GetArray())
	{
		Coord newCoord;
		newCoord.x = coord["x"].GetFloat();
		newCoord.y = coord["y"].GetFloat();
		myCoords.push_back(newCoord);
	}
}

void Curve::ParseToJsonObject(rapidjson::Value& aJsonObject, rapidjson::Document& aOutput, const char* aName)
{
	rapidjson::Value coords(rapidjson::kArrayType);
	coords.SetArray();

	rapidjson::GenericStringRef<char> name = rapidjson::GenericStringRef<char>(aName);

	for (auto& coord : myCoords)
	{
		rapidjson::Value coordVal;
		coordVal.SetObject();
		coordVal.AddMember("x", coord.x, aOutput.GetAllocator());
		coordVal.AddMember("y", coord.y, aOutput.GetAllocator());
		coords.PushBack(coordVal, aOutput.GetAllocator());
	}

	aJsonObject.AddMember(name, coords, aOutput.GetAllocator());
}

void Curve::SetMin(int aMin)
{
	myMin = aMin;
}

void Curve::SetMax(int aMax)
{
	myMax = aMax;
}

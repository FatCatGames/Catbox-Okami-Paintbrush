#include "stdafx.h"
#include "ColorGradient.h"
#include "UtilityFunctions.hpp"

static const float GRADIENT_BAR_WIDGET_HEIGHT = 25;
static const float GRADIENT_BAR_EDITOR_HEIGHT = 30;
static const float GRADIENT_MARK_DELETE_DIFFY = 40;


ColorGradient::ColorGradient()
{
	AddColor(Color::White(), 0);
	AddColor(Color::Black(), 1);
}

void ColorGradient::AddColor(const Color& aColor, float aPosition)
{
	for (size_t i = 0; i < myColors.size(); i++)
	{
		if (aPosition <= myColors[i].pos)
		{
			if (aPosition == myColors[i].pos)
			{
				myColors[i].color = aColor;
				return;
			}
			ColorPoint color;
			color.color = aColor;
			color.pos = aPosition;
			myColors.insert(myColors.begin() + i, color);
			return;
		}
	}
	ColorPoint color;
	color.color = aColor;
	color.pos = aPosition;
	myColors.push_back(color);
}

const Color ColorGradient::Evaluate(float aPosition)
{
	for (size_t i = 0; i < myColors.size(); i++)
	{
		if (aPosition < myColors[i].pos)
		{
			if (i == 0) return myColors[i].color;
			float percent = (aPosition - myColors[i - 1].pos) / (myColors[i].pos - myColors[i - 1].pos);
			return Catbox::Lerp(myColors[i - 1].color, myColors[i].color, percent);
		}
	}
	return (aPosition <= myColors[0].pos ? myColors[0].color : myColors.back().color);
}

void ColorGradient::RemoveColor(int anIndex)
{
	myColors.erase(myColors.begin() + anIndex);
}

void ColorGradient::RemoveColor(ColorPoint& aColor)
{
	for (size_t i = 0; i < myColors.size(); i++)
	{
		if (myColors[i].pos == aColor.pos)
		{
			myColors.erase(myColors.begin() + i);
			break;
		}
	}
}

void ColorGradient::Sort()
{
	std::sort(myColors.begin(), myColors.end(), [](const ColorPoint& aFirst, const ColorPoint& aSecond)
		{
			return aFirst.pos < aSecond.pos;
		});
}

void ColorGradient::LoadFromJson(rapidjson::Value& aJsonObject)
{
	myColors.clear();
	for (auto& colorPointObj : aJsonObject.GetArray())
	{
		ColorPoint newColorPoint;
		newColorPoint.color.LoadFromJson(colorPointObj["Color"]);
		newColorPoint.pos = colorPointObj["pos"].GetInt() / 100.f;
		myColors.push_back(newColorPoint);
	}
}

void ColorGradient::ParseToJsonObject(rapidjson::Value& aEmissionData, rapidjson::Document& aOutput, const char* aName)
{
	rapidjson::Value colors(rapidjson::kArrayType);
	colors.SetArray();

	rapidjson::GenericStringRef<char> name = rapidjson::GenericStringRef<char>(aName);

	for (auto& colorPoint : myColors)
	{
		rapidjson::Value colorPointObj;
		colorPointObj.SetObject();
		colorPoint.color.ParseToJsonObject(colorPointObj, aOutput, "Color");
		colorPointObj.AddMember("pos", static_cast<int>(100 * colorPoint.pos), aOutput.GetAllocator());
		colors.PushBack(colorPointObj, aOutput.GetAllocator());
	}

	aEmissionData.AddMember(name, colors, aOutput.GetAllocator());
}

void ColorGradient::DrawGradientBar(ImVec2 const& bar_pos, float maxWidth, float height)
{
	ImVec4 colorA = { 1,1,1,1 };
	ImVec4 colorB = { 1,1,1,1 };
	float prevX = bar_pos.x;
	float barBottom = bar_pos.y + height;
	ColorPoint* prevMark = nullptr;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	draw_list->AddRectFilled(ImVec2(bar_pos.x - 2, bar_pos.y - 2),
		ImVec2(bar_pos.x + maxWidth + 2, barBottom + 2),
		IM_COL32(100, 100, 100, 255));

	if (myColors.empty())
	{
		draw_list->AddRectFilled(ImVec2(bar_pos.x, bar_pos.y),
			ImVec2(bar_pos.x + maxWidth, barBottom),
			IM_COL32(255, 255, 255, 255));

	}

	ImU32 colorAU32 = 0;
	ImU32 colorBU32 = 0;

	for(auto& point : myColors)
	{
		ColorPoint* mark = &point;

		float from = prevX;
		float to = prevX = bar_pos.x + mark->pos * maxWidth;

		if (prevMark == nullptr)
		{
			colorA.x = mark->color.r;
			colorA.y = mark->color.g;
			colorA.z = mark->color.b;
		}
		else
		{
			colorA.x = prevMark->color.r;
			colorA.y = prevMark->color.g;
			colorA.z = prevMark->color.b;
		}

		colorB.x = mark->color.r;
		colorB.y = mark->color.g;
		colorB.z = mark->color.b;

		colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
		colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);

		if (mark->pos > 0.0)
		{

			draw_list->AddRectFilledMultiColor(ImVec2(from, bar_pos.y),
				ImVec2(to, barBottom),
				colorAU32, colorBU32, colorBU32, colorAU32);
		}

		prevMark = mark;
	}

	if (prevMark && prevMark->pos < 1.0)
	{

		draw_list->AddRectFilledMultiColor(ImVec2(prevX, bar_pos.y),
			ImVec2(bar_pos.x + maxWidth, barBottom),
			colorBU32, colorBU32, colorBU32, colorBU32);
	}

	ImGui::SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 10.0f));
}

void ColorGradient::DrawGradientMarks(ColorPoint*& draggingMark, ColorPoint*& selectedMark, struct ImVec2 const& bar_pos, float maxWidth, float height)
{
	ImVec4 colorA = { 1,1,1,1 };
	ImVec4 colorB = { 1,1,1,1 };
	float barBottom = bar_pos.y + height;
	ColorPoint* prevMark = nullptr;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImU32 colorAU32 = 0;
	ImU32 colorBU32 = 0;

	for (auto& point : myColors)
	{
		ColorPoint* mark = &point;


		float to = bar_pos.x + mark->pos * maxWidth;

		if (prevMark == nullptr)
		{
			colorA.x = mark->color.r;
			colorA.y = mark->color.g;
			colorA.z = mark->color.b;
		}
		else
		{
			colorA.x = prevMark->color.r;
			colorA.y = prevMark->color.g;
			colorA.z = prevMark->color.b;
		}

		colorB.x = mark->color.r;
		colorB.y = mark->color.g;
		colorB.z = mark->color.b;

		colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
		colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);

		draw_list->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 6)),
			ImVec2(to - 6, barBottom),
			ImVec2(to + 6, barBottom), IM_COL32(100, 100, 100, 255));

		draw_list->AddRectFilled(ImVec2(to - 6, barBottom),
			ImVec2(to + 6, bar_pos.y + (height + 12)),
			IM_COL32(100, 100, 100, 255), 1.0f, 1.0f);

		draw_list->AddRectFilled(ImVec2(to - 5, bar_pos.y + (height + 1)),
			ImVec2(to + 5, bar_pos.y + (height + 11)),
			IM_COL32(0, 0, 0, 255), 1.0f, 1.0f);

		if (selectedMark == mark)
		{
			draw_list->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 3)),
				ImVec2(to - 4, barBottom + 1),
				ImVec2(to + 4, barBottom + 1), IM_COL32(0, 255, 0, 255));

			draw_list->AddRect(ImVec2(to - 5, bar_pos.y + (height + 1)),
				ImVec2(to + 5, bar_pos.y + (height + 11)),
				IM_COL32(0, 255, 0, 255), 1.0f, 1.0f);
		}

		draw_list->AddRectFilledMultiColor(ImVec2(to - 3, bar_pos.y + (height + 3)),
			ImVec2(to + 3, bar_pos.y + (height + 9)),
			colorBU32, colorBU32, colorBU32, colorBU32);

		ImGui::SetCursorScreenPos(ImVec2(to - 6, barBottom));
		ImGui::InvisibleButton("mark", ImVec2(12, 12));

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(0))
			{
				selectedMark = mark;
				Editor::GetInstance()->SetCanDeleteObjects(selectedMark == nullptr);
				draggingMark = mark;
			}
		}


		prevMark = mark;
	}

	ImGui::SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 20.0f));
}

bool ColorGradient::GradientButton()
{
	ImVec2 widget_pos = ImGui::GetCursorScreenPos();

	//float maxWidth = Catbox::Max(250.0f, ImGui::GetContentRegionAvail().x - 100.0f);
	float maxWidth = 250.f;
	bool clicked = ImGui::InvisibleButton("gradient_bar", ImVec2(maxWidth, GRADIENT_BAR_WIDGET_HEIGHT));

	DrawGradientBar(widget_pos, maxWidth, GRADIENT_BAR_WIDGET_HEIGHT);

	return clicked;
}

bool ColorGradient::GradientEditor(ColorPoint*& draggingMark, ColorPoint*& selectedMark)
{
	bool modified = false;

	ImVec2 bar_pos = ImGui::GetCursorScreenPos();
	bar_pos.x += 10;
	float maxWidth = ImGui::GetContentRegionAvail().x - 20;
	float barBottom = bar_pos.y + GRADIENT_BAR_EDITOR_HEIGHT;

	ImGui::InvisibleButton("gradient_editor_bar", ImVec2(maxWidth, GRADIENT_BAR_EDITOR_HEIGHT));

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
	{
		float pos = (ImGui::GetIO().MousePos.x - bar_pos.x) / maxWidth;
		AddColor(Evaluate(pos), pos);
	}

	DrawGradientBar(bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);
	DrawGradientMarks(draggingMark, selectedMark, bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);

	if (!ImGui::IsMouseDown(0) && draggingMark)
	{
		draggingMark = nullptr;
	}

	if (ImGui::IsMouseDragging(0) && draggingMark)
	{
		float increment = ImGui::GetIO().MouseDelta.x / maxWidth;
		bool insideZone = (ImGui::GetIO().MousePos.x > bar_pos.x) &&
			(ImGui::GetIO().MousePos.x < bar_pos.x + maxWidth);

		if (increment != 0.0f && insideZone)
		{
			draggingMark->pos += increment;
			draggingMark->pos = Catbox::Clamp(draggingMark->pos * 1.f, 0.0f, 1.0f);
			Sort();
			modified = true;
		}

		float diffY = ImGui::GetIO().MousePos.y - barBottom;

		if (diffY >= GRADIENT_MARK_DELETE_DIFFY)
		{
			RemoveColor(*draggingMark);
			draggingMark = nullptr;
			selectedMark = nullptr;
			modified = true;
		}
	}

	if (selectedMark)
	{
		float color[4] = { selectedMark->color.r, selectedMark->color.g, selectedMark->color.b, selectedMark->color.a };
		ImGui::SetNextItemWidth(250);
		bool colorModified = ImGui::ColorPicker4("Color", color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_AlphaBar);

		if (selectedMark && colorModified)
		{
			modified = true;
			selectedMark->color.r = color[0];
			selectedMark->color.g = color[1];
			selectedMark->color.b = color[2];
			selectedMark->color.a = color[3];
			Sort();
		}
	}

	return modified;
}

#include "stdafx.h"
#include "ImguiWrapper.h"
#include "Editor/Commands/EditorCommandHandler.h"
#include "Editor/Commands/ImGuiValueCommand.h"
#include "Editor/Commands/Vector3Command.h"
#include "Editor/Commands/Vector4Command.h"
#include "imgui.h"
#include "imgui_internal.h"

float startFloatValue = 0;
int startIntValue = 0;
Vector3f startVectorfValue;
Vector4f startVector4fValue;

bool Catbox::InputFloat(const char* label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
{
	float copy = *v;
	if (ImGui::InputFloat(label, v, step, step_fast, format, flags))
	{
		EditorCommandHandler::AddCommand(std::make_shared<ImGuiValueCommand<float>>(v, *v - copy));
		return true;
	}

	return false;
}

bool Catbox::DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	bool edit = ImGui::DragFloat(label, v, v_speed, v_min, v_max, format, flags);

	if (ImGui::IsItemActivated())
	{
		startFloatValue = *v;
	}

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		EditorCommandHandler::AddCommand(std::make_shared<ImGuiValueCommand<float>>(v, *v - startFloatValue));
	}

	return edit;
}

bool Catbox::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	float copy = *v;
	bool edit = ImGui::SliderFloat(label, &copy, v_min, v_max, format, flags);
	if (edit)
	{
		if (ImGui::IsItemActivated())
		{
			startFloatValue = *v;
		}
		*v = copy;
	}

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		EditorCommandHandler::AddCommand(std::make_shared<ImGuiValueCommand<float>>(v, *v - startFloatValue));
	}

	return edit;
}

bool Catbox::DragFloat3(const char* label, Vector3f* vec, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	float v[3];
	v[0] = vec->x;
	v[1] = vec->y;
	v[2] = vec->z;

	bool edit =  ImGui::DragFloat3(label, v, v_speed, v_min, v_max, format, flags);

	if (ImGui::IsItemActivated())
	{
		startVectorfValue = *vec;
	}

	if(edit) *vec = Vector3f(v[0], v[1], v[2]);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		EditorCommandHandler::AddCommand(std::make_shared<Vector3Command<float>>(vec, *vec - startVectorfValue));
	}

	return edit;
}

bool Catbox::DragFloat4(const char* label, Vector4f* vec, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	float v[4];
	v[0] = vec->x;
	v[1] = vec->y;
	v[2] = vec->z;
	v[3] = vec->w;

	bool edit = ImGui::DragFloat4(label, v, v_speed, v_min, v_max, format, flags);

	if (ImGui::IsItemActivated())
	{
		startVector4fValue = *vec;
	}

	if (edit) *vec = Vector4f(v[0], v[1], v[2], v[3]);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		EditorCommandHandler::AddCommand(std::make_shared<Vector4Command<float>>(vec, *vec - startVector4fValue));
	}

	return edit;
}

bool Catbox::InputInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
	int copy = *v;
	if (ImGui::InputInt(label, v, step, step_fast, flags))
	{
		EditorCommandHandler::AddCommand(std::make_shared<ImGuiValueCommand<int>>(v, *v - copy));
		return true;
	}

	return false;
}

bool Catbox::DragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	bool edit = ImGui::DragInt(label, v, v_speed, v_min, v_max, format, flags);

	if (ImGui::IsItemActivated())
	{
		startIntValue = *v;
	}

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		EditorCommandHandler::AddCommand(std::make_shared<ImGuiValueCommand<int>>(v, *v - startIntValue));
	}

	return edit;
}

bool Catbox::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	int copy = *v;
	bool edit = ImGui::SliderInt(label, &copy, v_min, v_max, format, flags);
	if (edit)
	{
		if (ImGui::IsItemActivated())
		{
			startIntValue = *v;
		}
		*v = copy;
	}

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		EditorCommandHandler::AddCommand(std::make_shared<ImGuiValueCommand<int>>(v, *v - startIntValue));
	}

	return edit;
}

bool Catbox::Checkbox(const char* label, bool* v)
{
	int copy = *v;
	if (ImGui::Checkbox(label, v))
	{
		EditorCommandHandler::AddCommand(std::make_shared<ImGuiValueCommand<int>>(reinterpret_cast<int*>(v), *v - copy));
		return true;
	}
	return false;
}

using namespace ImGui;
void ImGui::CenteredText(const char* label, const ImVec2& size_arg)
{
	ImGuiWindow* window = GetCurrentWindow();

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImVec2 pos2 = ImVec2((pos.x + size.x), (pos.y + size.y));
	const ImRect bb(pos, pos2);

	ItemSize(size, style.FramePadding.y);

	const ImVec2 pos_min = ImVec2((bb.Min.x + style.FramePadding.x), (bb.Min.y + style.FramePadding.y));
	const ImVec2 pos_max = ImVec2((bb.Max.x - style.FramePadding.x), (bb.Max.y - style.FramePadding.y));

	RenderTextClipped(pos_min, pos_max, label, NULL, &label_size, style.ButtonTextAlign, &bb);
}

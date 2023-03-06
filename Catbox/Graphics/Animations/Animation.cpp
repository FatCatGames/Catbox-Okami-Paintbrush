#include "stdafx.h"
#include "Animation.h"

void Animation::RenderInProperties()
{
	ImGui::Text("Animation:");
	std::string tempStr = "Frame Count : ";
	tempStr.append(std::to_string(frameCount));
	ImGui::Text(tempStr.c_str());
	tempStr = "Duration (Seconds) : ";
	tempStr.append(std::to_string(duration));
	ImGui::Text(tempStr.c_str());

	tempStr = "Frame Per Second : ";
	tempStr.append(std::to_string(std::round(framesPerSecond)));
	ImGui::Text(tempStr.c_str());
}

void Animation::SaveAsset(const char* /*aPath*/)
{
	print("Animation saving not implemented.");
}

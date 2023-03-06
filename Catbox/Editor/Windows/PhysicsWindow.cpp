#include "stdafx.h"
#include "PhysicsWindow.h"
#include "Physics/Collisions/CollisionManager.h"
#include "ProjectSettings.h"

PhysicsWindow::PhysicsWindow()
{
	myCollisionManager = Engine::GetInstance()->GetCollisionManager();
}

void PhysicsWindow::Render()
{
	if (!BeginWindow("Physics", true)) return;

	ImGui::Text("Layers");
	auto& layers = myCollisionManager->myCollisionLayerNames;
	std::vector<std::vector<bool>>& matrix = myCollisionManager->myCollisionMatrix;
	std::vector<std::vector<bool>>& physicsLayers = myCollisionManager->myPhysicsCollisionLayers;

	ImGui::SetWindowFontScale(0.95f);
	int maxCharCount = 8;
	std::string tempName = "";
	tempName.resize(maxCharCount, ' ');
	ImGui::Text(tempName.c_str());
	ImGui::SameLine();

	for (int i = static_cast<int>(layers.size()) -1; i >= 0; i--)
	{
		tempName = layers[i];
		tempName.resize(maxCharCount, ' ');
		ImGui::Text(tempName.c_str());
		ImGui::SameLine();
	}
	ImGui::NewLine();

	for (int lay1 = 0; lay1 < matrix.size(); lay1++)
	{
		ImGui::NewLine();
		tempName = layers[lay1];
		tempName.resize(maxCharCount, ' ');
		ImGui::Text(tempName.c_str());

		for (int lay2 = 0; lay2 < matrix[lay1].size(); lay2++)
		{
			ImGui::SameLine();
			if (lay2 != 0) 
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() +35);
			}
			else 
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
			}
			bool b = matrix[lay1][matrix[lay1].size() - 1 - lay2];
			if (ImGui::Checkbox(("##" + std::to_string(lay1) + std::to_string(lay2)).c_str(), &b))
			{
				int layer2Index = static_cast<int>(matrix[lay1].size()) - 1 - lay2;
				matrix[lay1][layer2Index] = b;
				print(layers[lay1] + "," + layers[matrix[lay1].size() - 1 - lay2]);

				physicsLayers[lay1][physicsLayers[lay1].size() - 1 - lay2] = b;
				physicsLayers[physicsLayers[lay1].size() - 1 - lay2][lay1] = b;

				ProjectSettings::SaveProjectSettings();
			}
		}
	}

	ImGui::NewLine();
	ImGui::TextWrapped("Visual Representation of the Physics Layer Vector, if it doesn't match up with the Matrix Above, try to fix it or contact SP!");
	
	maxCharCount = 8;
	tempName = "";
	tempName.resize(maxCharCount, ' ');
	ImGui::Text(tempName.c_str());
	ImGui::SameLine();
	
	for (int i = 0 ; i < layers.size(); i++)
	{
		tempName = layers[i];
		tempName.resize(maxCharCount, ' ');
		ImGui::Text(tempName.c_str());
		ImGui::SameLine();
	}
	ImGui::NewLine();

	for (size_t i = 0; i < physicsLayers.size(); i++)
	{
		ImGui::NewLine();
		tempName = layers[i];
		tempName.resize(maxCharCount, ' ');
		ImGui::Text(tempName.c_str());

		for (size_t j = 0; j < physicsLayers[i].size(); j++)
		{
			ImGui::SameLine();
			if (j != 0)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 35);
			}
			else
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
			}
			bool b = physicsLayers[i][j];
			ImGui::Checkbox(("##" + std::to_string(i) + std::to_string(j)).c_str(), &b);
		}
	}

	if (ImGui::Button("Reload Layer Values"))
	{
		ProjectSettings::LoadProjectSettings();
	}

	ImGui::SetWindowFontScale(1);
	
	EndWindow();
}
#include "stdafx.h"
#include "Shader.h"


void VertexShader::RenderInProperties()
{
	char nameInput[32] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	std::string emptyText = "##VertexShaderNameInput";
	if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	{
		myName = nameInput;
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}
	
}
void PixelShader::RenderInProperties()
{

	char nameInput[32] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	std::string emptyText = "##PixelShaderNameInput";
	if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	{
		myName = nameInput;
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}

}
void PixelShader::LoadDefaultValues()
{
	printmsg("2323");
}
PixelShader::PixelShader()
{
	//ofstream stream(myPath);
}
void GeometryShader::RenderInProperties()
{

	char nameInput[32] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	std::string emptyText = "##GeometryShaderNameInput";
	if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	{
		myName = nameInput;
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}

}
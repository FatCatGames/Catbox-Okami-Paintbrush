#include "stdafx.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Graphics\Rendering\Buffers\CBuffer.hpp"
#include "Physics/PhysicsEngine.h"
#include <d3dcompiler.h>

std::shared_ptr<Texture> Material::ourDefaultCTex = nullptr;
std::shared_ptr<Texture> Material::ourDefaultMTex = nullptr;
std::shared_ptr<Texture> Material::ourDefaultNTex = nullptr;

Material::Material()
{
	myVertexShader = AssetRegistry::GetInstance()->GetAsset<VertexShader>("DefaultVS");
	myPixelShader = AssetRegistry::GetInstance()->GetAsset<PixelShader>("DefaultPS");
	if (!ourDefaultCTex)
	{
		ourDefaultCTex = AssetRegistry::GetInstance()->GetAsset<Texture>("T_default_C");
		ourDefaultMTex = AssetRegistry::GetInstance()->GetAsset<Texture>("T_default_M");
		ourDefaultNTex = AssetRegistry::GetInstance()->GetAsset<Texture>("T_default_N");
	}
	myTextures[TextureChannel::Albedo] = ourDefaultCTex;
}

Material::~Material()
{
	myPixelShader = nullptr;
	myVertexShader = nullptr;
	myGeometryShader = nullptr;	

}

void Material::SetAsResource(Catbox::CBuffer<MaterialBufferData>& aMaterialBuffer)
{
	aMaterialBuffer.SetData(&myData);

	if (myTextures[Material::TextureChannel::Albedo])
	{
		myTextures[Material::TextureChannel::Albedo]->SetAsResource(0);
	}
	else
	{
		ourDefaultCTex->SetAsResource(0);
	}
	if (myTextures[Material::TextureChannel::Normal])
	{
		myTextures[Material::TextureChannel::Normal]->SetAsResource(1);
	}
	else
	{
		ourDefaultNTex->SetAsResource(1);
	}
	if (myTextures[Material::TextureChannel::PbrTex])
	{
		myTextures[Material::TextureChannel::PbrTex]->SetAsResource(2);
	}
	else
	{
		ourDefaultMTex->SetAsResource(2);
	}

	if (myPixelShader)
	{
		myPixelShader->SetData();
		myPixelShader->SetResource();
	}
	if (myVertexShader)
	{
		myVertexShader->SetData();
		myVertexShader->SetResource();
	}
	if (myGeometryShader)
	{
		myGeometryShader->SetData();
		myGeometryShader->SetResource();
	}
}

std::shared_ptr<Material> Material::MakeInstanceOfMaterial()
{
	std::shared_ptr<Material> returnMaterial = std::make_shared<Material>();
	for (int i = 0; i < 3; i++)
	{
		returnMaterial->myTextures[i] = myTextures[i];
	}
	//returnMaterial->myPixelShader = myPixelShader->MakeInstance();
	returnMaterial->myVertexShader = myVertexShader;
	returnMaterial->myData = myData;
	returnMaterial->myName = myName;
	returnMaterial->myIsInstance = true;
	return returnMaterial;
}

inline void Material::SetPixelShader(std::shared_ptr<PixelShader> aShader)
{
	if (aShader)
	{
		myPixelShader = aShader;
		myPixelShader->pixelShader = aShader->pixelShader;
		myPixelShader->SetPath(aShader->GetPathStr());
	}
	else
	{
		printerror("Tried to set a pixel shader on " + myName + " that did not exist!");
	}
}

void Material::SaveAsset(const char* aPath)
{
	int version = 6;

	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& document = wrapper->CreateDocument();
	
	if (myHasPhysxMaterial) 
	{
		if (!Engine::GetInstance()->GetPhysicsEngine()->GetMaterial(myName)) 
		{
			Engine::GetInstance()->GetPhysicsEngine()->CreateMaterial(myName, myPhysxMaterial);
		}
		else 
		{
			Engine::GetInstance()->GetPhysicsEngine()->EditMaterial(myName, myPhysxMaterial);
		}
	}

	wrapper->SaveValue<DataType::Int>("Version", version);
	wrapper->SaveValue<DataType::Float>("Emissive", myData.EmissiveStr);

	myData.color.ParseToJsonObject(document, document, "Color");

	std::string albedoName = myTextures[Albedo] ? myTextures[Albedo]->GetName() : "";
	wrapper->SaveString("Albedo", albedoName.c_str());

	std::string pbrName = myTextures[PbrTex] ? myTextures[PbrTex]->GetName() : "";
	wrapper->SaveString("PBR", pbrName.c_str());

	std::string normalName = myTextures[Normal] ? myTextures[Normal]->GetName() : "";
	wrapper->SaveString("Normal", normalName.c_str());

	auto pixelShader = wrapper->CreateObject();
	wrapper->SetTarget(pixelShader);
	std::string psName = myPixelShader ? myPixelShader->GetName() : "";
	wrapper->SaveString("Name", psName.c_str());
	if (myPixelShader)
	{
		myPixelShader->SaveData();
	}
	wrapper->SetTarget(document);
	wrapper->SaveObject("PixelShader", pixelShader);

	auto vertexShader = wrapper->CreateObject();
	wrapper->SetTarget(vertexShader);

	std::string vsName = myVertexShader ? myVertexShader->GetName() : "";
	wrapper->SaveString("Name", vsName.c_str());
	if (myVertexShader)
	{
		myVertexShader->SaveData();
	}
	wrapper->SetTarget(document);
	wrapper->SaveObject("VertexShader", vertexShader);


	auto geometryShader = wrapper->CreateObject();
	wrapper->SetTarget(geometryShader);
	std::string gsName = myGeometryShader ? myGeometryShader->GetName() : "";
	wrapper->SaveString("Name", gsName.c_str());
	if (myGeometryShader)
	{
		myGeometryShader->SaveData();
	}
	wrapper->SetTarget(document);
	wrapper->SaveObject("GeometryShader", geometryShader);


	wrapper->SaveValue<DataType::Int, int>("TilingMultiplier", myTilingMuliplier);
	wrapper->SaveValue<DataType::Bool, bool>("IsTiling", myIsTiling);
	int aNumber = static_cast<int>(myBlendState);
	wrapper->SaveValue<DataType::Float, float>("c1", myData.CustomValues3.x);
	wrapper->SaveValue<DataType::Float, float>("c2", myData.CustomValues3.y);
	wrapper->SaveValue<DataType::Float, float>("c3", myData.CustomValues3.z);
	wrapper->SaveValue<DataType::Float, float>("c4", myData.CustomValues4.x);
	wrapper->SaveValue<DataType::Float, float>("c5", myData.CustomValues4.y);
	wrapper->SaveValue<DataType::Float, float>("c6", myData.CustomValues4.z);
	wrapper->SaveValue<DataType::Float, float>("c7", myData.CustomValues4.w);

	int blendStateValue = static_cast<int>(myBlendState);
	wrapper->SaveValue<DataType::Int, int>("BlendState", blendStateValue);

	int rasterizerState = static_cast<int>(myRastertizerState);
	wrapper->SaveValue<DataType::Int, int>("RasterizerState", rasterizerState);

	if (myHasPhysxMaterial) 
	{
		wrapper->SaveValue<DataType::Float, float>("Dynamic Friction", myPhysxMaterial.x);
		wrapper->SaveValue<DataType::Float, float>("Static Friction", myPhysxMaterial.y);
		wrapper->SaveValue<DataType::Float, float>("Restitution", myPhysxMaterial.z);
	}
	wrapper->SaveDocument(aPath);
}

void Material::ReloadShaders()
{

	std::filesystem::path defaultPathReg = "..\\Catbox\\Shaders";
	std::filesystem::path defaultPathProj = "..\\Game\\Shaders";

	ComPtr<ID3DBlob> shaderBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	std::string pixelName;
	if (myPixelShader != nullptr)
	{
		pixelName = myPixelShader->GetName();
	}
	std::string vertexName;
	if (myVertexShader != nullptr)
	{
		vertexName = myVertexShader->GetName();
	}
	std::string geometryName;
	if (myGeometryShader != nullptr)
	{
		geometryName = myGeometryShader->GetName();
	}

	HRESULT hr;

	if (!pixelName.empty())
	{
		std::filesystem::path pixelShaderPath;
		pixelShaderPath = LoadFolder(defaultPathReg, pixelName);
	

		if (pixelShaderPath.empty())
		{
			pixelShaderPath = LoadFolder(defaultPathProj, pixelName);
		}
		if (!pixelShaderPath.empty())
		{
			pixelShaderPath += "\\";
			pixelShaderPath += pixelName;
			pixelShaderPath += ".hlsl";

			hr = D3DCompileFromFile(pixelShaderPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0",
				NULL, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());

			if (hr != E_FAIL)
			{
				DX11::Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &myPixelShader->pixelShader);
			}
			else
			{
				printerror("Pixel shader scuffed on: " + myName);
			}
		}
	}


	if (!vertexName.empty())
	{
		std::filesystem::path vertexShaderPath;
		vertexShaderPath = LoadFolder(defaultPathReg, vertexName);
	

		if (vertexShaderPath.empty())
		{
			vertexShaderPath = LoadFolder(defaultPathProj, vertexName);
		}

		if (!vertexShaderPath.empty())
		{
			vertexShaderPath += "/";
			vertexShaderPath += vertexName;
			vertexShaderPath += ".hlsl";

			hr = D3DCompileFromFile(vertexShaderPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
				NULL, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());

			if (hr != E_FAIL)
			{
				DX11::Device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &myVertexShader->vertexShader);
			}
			else
			{
				printerror("Vertex shader scuffed on: " + myName);
			}
		}
	}

	

	if (!geometryName.empty())
	{
		std::filesystem::path geometryShaderPath;
		geometryShaderPath = LoadFolder(defaultPathReg, geometryName);
	

		if (geometryShaderPath.empty())
		{
			geometryShaderPath = LoadFolder(defaultPathProj, geometryName);
		}

		if (!geometryShaderPath.empty())
		{
			geometryShaderPath += "\\";
			geometryShaderPath += geometryName;
			geometryShaderPath += ".hlsl";

			hr = D3DCompileFromFile(geometryShaderPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0",
				NULL, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());

			if (hr != E_FAIL)
			{
				DX11::Device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &myGeometryShader->geometryShader);
			}
			else
			{
				printerror("Geometry shader scuffed on: " + myName);
			}
		}
	}

	if (shaderBlob != nullptr)
	{
		shaderBlob->Release();
	}
	if (errorBlob != nullptr)
	{
		errorBlob->Release();
	}
}

std::filesystem::path Material::LoadFolder(const std::filesystem::path& aDirectory, const std::filesystem::path& aName)
{
	
	for (auto& directoryEntry : std::filesystem::directory_iterator(aDirectory))
	{
		if (directoryEntry.is_directory())
		{
			std::filesystem::path foundPath = LoadFolder(directoryEntry.path(), aName);
			if (!foundPath.empty())
			{
				return foundPath;
			}
		}
		else
		{

			if (directoryEntry.path().string().find(aName.string()) != std::string::npos)
			{
				return aDirectory;
			}
		}
	}
	return "";
}

void Material::LoadFromPath(const char* aPath)
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& document = wrapper->LoadDocument(aPath);

	int version = 0;
	if (document.HasMember("Version"))
	{
		version = document["Version"].GetInt();
	}

	myTextures[Albedo] = AssetRegistry::GetInstance()->GetAsset<Texture>(wrapper->LoadValue("Albedo").GetString());
	myTextures[PbrTex] = AssetRegistry::GetInstance()->GetAsset<Texture>(wrapper->LoadValue("PBR").GetString());
	myTextures[Normal] = AssetRegistry::GetInstance()->GetAsset<Texture>(wrapper->LoadValue("Normal").GetString());

	if (document.HasMember("PixelShader"))
	{
		if (version == 0)
		{
			myPixelShader = AssetRegistry::GetInstance()->GetAsset<PixelShader>(wrapper->LoadValue("PixelShader").GetString());
			myVertexShader = AssetRegistry::GetInstance()->GetAsset<VertexShader>(wrapper->LoadValue("VertexShader").GetString());
		}

		if (version >= 1)
		{
			wrapper->SetTarget(document["PixelShader"].GetObj());
			auto ps = AssetRegistry::GetInstance()->GetAsset<PixelShader>(wrapper->LoadValue("Name").GetString());
			SetPixelShader(ps);
			if (myPixelShader)
			{
				myPixelShader->LoadData();
			}

			wrapper->SetTarget(document["VertexShader"].GetObj());
			myVertexShader = AssetRegistry::GetInstance()->GetAsset<VertexShader>(wrapper->LoadValue("Name").GetString());
			if (myVertexShader)
			{
				myVertexShader->LoadData();
			}

			if (version >= 2)
			{
				myData.EmissiveStr = document["Emissive"].GetFloat();

				if (version >= 3)
				{
					myData.CustomValues3.x = document["c1"].GetFloat();
					myData.CustomValues3.y = document["c2"].GetFloat();
					myData.CustomValues3.z = document["c3"].GetFloat();
					myData.CustomValues4.x = document["c4"].GetFloat();
					myData.CustomValues4.y = document["c5"].GetFloat();
					myData.CustomValues4.z = document["c6"].GetFloat();
					myData.CustomValues4.w = document["c7"].GetFloat();
				}
				if (version >= 4) 
				{
					myBlendState = (BlendState)document["BlendState"].GetInt();
					myRastertizerState = (RastertizerState)document["RasterizerState"].GetInt();
				}
				if (version >= 5) 
				{
					wrapper->SetTarget(document["GeometryShader"].GetObj());
					auto gs = AssetRegistry::GetInstance()->GetAsset<GeometryShader>(wrapper->LoadValue("Name").GetString());
					SetGeometryShader(gs);
					if (myGeometryShader)
					{
						myGeometryShader->LoadData();
					}
				}
				if(version >= 6) 
				{
					if (document.HasMember("Dynamic Friction")) 
					{
						myHasPhysxMaterial = true;
						myPhysxMaterial.x = document["Dynamic Friction"].GetFloat();
						myPhysxMaterial.y = document["Static Friction"].GetFloat();
						myPhysxMaterial.z = document["Restitution"].GetFloat();
						Engine::GetInstance()->GetPhysicsEngine()->CreateMaterial(myName, myPhysxMaterial);
					}
				}
			}
		}
	}

	if (document.HasMember("Color"))
	{
		myData.color.LoadFromJson(document["Color"]);
	}
	if (document.HasMember("TilingMultiplier"))
	{
		myTilingMuliplier = document["TilingMultiplier"].GetInt();
	}
	if (document.HasMember("IsTiling"))
	{
		myIsTiling = document["IsTiling"].GetBool();
	}
}


void Material::RenderTextureDropdown(std::vector<Material*>& aMaterialList, Material::TextureChannel aChannel, std::string aChannelName)
{
	bool edit;

	Texture* tex = myTextures[aChannel].get();
	for (auto& mat : aMaterialList)
	{
		if (mat->myTextures[aChannel].get() != tex)
		{
			tex = nullptr;
			break;
		}
	}

	if (tex)
	{
		ImGui::Image(static_cast<ImTextureID>(tex->GetShaderResourceView().Get()), { 45, 45 });
	}
	else
	{
		ImGui::Image(nullptr, { 45, 45 });
	}

	ImGui::SameLine();
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 26);
	ImGui::SetNextItemWidth(100);
	auto newTex = AssetRegistry::GetInstance()->AssetDropdownMenu<Texture>(tex, aChannelName, edit);
	if (edit)
	{
		for (auto& mat : aMaterialList)
		{
			mat->myTextures[aChannel] = newTex;
			mat->SaveAsset(mat->myPath.string().c_str());
		}
	}
}

void Material::RenderInProperties(std::vector<Asset*>& anAssetList)
{
	if (anAssetList.size() == 1)
	{
		char nameInput[32] = "";
		for (size_t i = 0; i < myName.size(); i++)
		{
			nameInput[i] = myName[i];
		}

		std::string emptyText = "##MaterialNameInput";
		if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
		{
			myName = nameInput;
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			AssetRegistry::GetInstance()->RenameAsset(this, myName);
		}
	}
	else ImGui::Text("Multiple materials selected");

	ImGui::Separator();

	auto materialList = AssetVectorCast<Material>(anAssetList);

	int blendState = (int)myBlendState;
	const char* states[] = { "None", "Alpha blend", "Additive", "Foliage" };
	if (ImGui::Combo("Theme", &blendState, states, IM_ARRAYSIZE(states)))
	{
		for (auto& mat : materialList)
		{
			mat->myBlendState = (BlendState)blendState;
		}
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& mat : materialList)
		{
			mat->SaveAsset(mat->myPath.string().c_str());
		}
	}
	int rasterizerState = (int)myRastertizerState;
	const char* statesName[] = { "Default", "Cull None", "Wire Frame"};
	if (ImGui::Combo("Rasterizer State", &rasterizerState, statesName, IM_ARRAYSIZE(statesName)))
	{
		for (auto& mat : materialList)
		{
			mat->myRastertizerState = (RastertizerState)rasterizerState;
		}
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& mat : materialList)
		{
			mat->SaveAsset(mat->myPath.string().c_str());
		}
	}


	float col[4] = { myData.color.r, myData.color.g, myData.color.b, myData.color.a };
	if (ImGui::ColorEdit4("Color", col))
	{
		for (auto& mat : materialList)
		{
			mat->myData.color = { col[0], col[1], col[2], col[3] };
		}
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& mat : materialList)
		{
			mat->SaveAsset(mat->myPath.string().c_str());
		}
	}

	Catbox::DragFloat("Emissive strength", &myData.EmissiveStr, 0.01f, 0, 1);



	bool edit = false;
	if (ImGui::TreeNodeEx(("Custom values##" + myRuntimeId).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		edit |= Catbox::DragFloat("1", &myData.CustomValues3.x, 0.01f);
		edit |= Catbox::DragFloat("2", &myData.CustomValues3.y, 0.01f);
		edit |= Catbox::DragFloat("3", &myData.CustomValues3.z, 0.01f);
		edit |= Catbox::DragFloat4("Float4", &myData.CustomValues4, 0.01f);
		if (edit)
		{
			for (auto& mat : materialList)
			{
				if (!mat) continue;
				mat->SaveAsset(mat->myPath.string().c_str());
			}
			edit = false;
		}

		ImGui::TreePop();
	}

	RenderTextureDropdown(materialList, TextureChannel::Albedo, "Albedo");
	ImGui::NewLine();
	RenderTextureDropdown(materialList, TextureChannel::PbrTex, "PBR");
	ImGui::NewLine();
	RenderTextureDropdown(materialList, TextureChannel::Normal, "Normal");

	for (auto& mat : materialList)
	{
		if (!mat) continue;
		std::string idString = "##" + mat->GetName();
		std::string tempString = "Is Tiling";
		tempString.append(idString);
		ImGui::Checkbox(tempString.c_str(), &mat->myIsTiling);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	std::shared_ptr<PixelShader> ps = myPixelShader;


	for (auto& mat : materialList)
	{
		if (!mat || !mat->myPixelShader || mat->myPixelShader->GetName() != ps->GetName())
		{
			ps = nullptr;
			break;
		}
	}

	if (myPixelShader)
	{
		ImGui::Text(myPixelShader->GetName().c_str());
	}
	auto newPS = AssetRegistry::GetInstance()->AssetDropdownMenu<PixelShader>(ps.get(), "Pixel shader", edit);
	if (edit)
	{
		for (auto& mat : materialList)
		{
			if (!mat) continue;
			mat->myPixelShader = newPS;
			mat->SaveAsset(mat->myPath.string().c_str());
		}
	}
	if (myPixelShader)
	{
		myPixelShader->RenderInProperties();
		if (myPixelShader->WasEditedThisFrame())
		{
			for (auto& mat : materialList)
			{
				mat->SaveAsset(mat->myPath.string().c_str());
			}
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	std::shared_ptr<VertexShader> vs = myVertexShader;
	for (auto& mat : materialList)
	{
		if (!mat || mat->myVertexShader != vs)
		{
			vs = nullptr;
			break;
		}
	}

	if (myVertexShader)
	{
		ImGui::Text(myVertexShader->GetName().c_str());
	}
	auto newVS = AssetRegistry::GetInstance()->AssetDropdownMenu<VertexShader>(vs.get(), "Vertex shader", edit);
	if (edit)
	{
		for (auto& mat : materialList)
		{
			if (!mat) continue;
			mat->myVertexShader = newVS;
			mat->SaveAsset(mat->myPath.string().c_str());
		}
	}
	if (myVertexShader)
	{
		myVertexShader->RenderInProperties();
		if (myVertexShader->WasEditedThisFrame())
		{
			for (auto& mat : materialList)
			{
				mat->SaveAsset(mat->myPath.string().c_str());
			}
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	std::shared_ptr<GeometryShader> gs = myGeometryShader;
	for (auto& mat : materialList)
	{
		if (!mat || mat->myGeometryShader != gs)
		{
			gs = nullptr;
			break;
		}
	}
	if (myGeometryShader)
	{
		ImGui::Text(myGeometryShader->GetName().c_str());
	}
	auto newGS = AssetRegistry::GetInstance()->AssetDropdownMenu<GeometryShader>(gs.get(), "Geometry Shader", edit);
	if (edit)
	{
		for (auto& mat : materialList)
		{
			if (!mat) continue;
			mat->myGeometryShader = newGS;
			mat->SaveAsset(mat->myPath.string().c_str());
		}
	}
	if (myGeometryShader)
	{
		myGeometryShader->RenderInProperties();
		if (myGeometryShader->WasEditedThisFrame())
		{
			for (auto& mat : materialList)
			{
				mat->SaveAsset(mat->myPath.string().c_str());
			}
		}
	}
	ImGui::Checkbox("Physx Material", &myHasPhysxMaterial);
	if (myHasPhysxMaterial) 
	{
		ImGui::InputFloat("Dynamic Friction: ", &myPhysxMaterial.x, 0.1f, 0.2f, "%.2f");
		if (ImGui::IsItemEdited())
		{
			edit = true;
		}

		if (myPhysxMaterial.x > 1.0f) 
		{
			myPhysxMaterial.x = 1.0f;
		}
		else if (myPhysxMaterial.x < 0.0f)
		{
			myPhysxMaterial.x = 0.0f;
		}

		ImGui::InputFloat("Static Friction: ", &myPhysxMaterial.y, 0.1f, 0.2f, "%.2f");
		if (ImGui::IsItemEdited())
		{
			edit = true;
		}
		if (myPhysxMaterial.y > 1.0f)
		{
			myPhysxMaterial.y = 1.0f;
		}
		else if (myPhysxMaterial.x < 0.0f)
		{
			myPhysxMaterial.y = 0.0f;
		}

		ImGui::InputFloat("Restitution: ", &myPhysxMaterial.z, 0.1f, 0.2f, "%.2f");
		if (ImGui::IsItemEdited())
		{
			edit = true;
		}
		if (myPhysxMaterial.z > 1.0f)
		{
			myPhysxMaterial.z = 1.0f;
		}
		else if (myPhysxMaterial.z < 0.0f)
		{
			myPhysxMaterial.z = 0.0f;
		}
	}
	if (ImGui::Button("Reload Shaders"))
	{
		ReloadShaders();
	}
	if (edit) 
	{
		SaveAsset(myPath.string().c_str());
	}
}

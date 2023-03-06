#include "stdafx.h"
#include "UISprite.h"
#include <d3dcompiler.h>

UISprite::UISprite()
{
	myTexture = AssetRegistry::GetInstance()->GetAsset<Texture>("Square-Default");
	myPixelShader = AssetRegistry::GetInstance()->GetAsset<PixelShader>("UISpritePS");
	myVertexShader = AssetRegistry::GetInstance()->GetAsset<VertexShader>("UISpriteVS");
}

UISprite::~UISprite()
{
	Engine::GetInstance()->GetGraphicsEngine()->RemoveFromRenderQueue(this);
}

void UISprite::Render()
{
	Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(this);
}

void UISprite::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto sprites = ComponentVectorCast<UISprite>(aComponentList);

	if (Catbox::InputInt("Layer", &myLayer)) 
	{
		for (auto& sprite : sprites)
		{
			sprite->myLayer = myLayer;
		}
	}

	bool sharedTex = true;
	auto& tex = myTexture;
	for (auto& sprite : sprites)
	{
		if (sprite->myTexture != tex)
		{
			sharedTex = false;
			break;
		}
	}

	bool edit;
	auto chosenTex = AssetRegistry::GetInstance()->AssetDropdownMenu<Texture>(sharedTex ? myTexture.get() : nullptr, "Texture", edit);
	if (edit) 
	{
		for (auto& sprite : sprites)
		{
			sprite->myTexture = chosenTex;
		}
	}

	auto myColor = myUIRenderData.color;
	float col[4] = { myColor.r, myColor.g, myColor.b, myColor.a };
	if (ImGui::ColorEdit4("Color", col))
	{
		for (auto& sprite : sprites)
		{
			sprite->myUIRenderData.color = { col[0], col[1], col[2], col[3] };
		}
	}

	bool sharedPS = true;
	auto& ps = myPixelShader;
	for (auto& sprite : sprites)
	{
		if (sprite->myPixelShader != ps)
		{
			sharedPS = false;
			break;
		}
	}

	auto chosenPS = AssetRegistry::GetInstance()->AssetDropdownMenu<PixelShader>(sharedPS ? myPixelShader.get() : nullptr, "Pixel Shader", edit);
	if (edit && chosenPS)
	{
		for (auto& sprite : sprites)
		{
			sprite->myPixelShader = chosenPS;
		}
	}

	bool sharedVS = true;
	auto& vs = myVertexShader;
	for (auto& sprite : sprites)
	{
		if (sprite->myVertexShader != vs)
		{
			sharedVS = false;
			break;
		}
	}

	auto chosenVS = AssetRegistry::GetInstance()->AssetDropdownMenu<VertexShader>(sharedVS ? myVertexShader.get() : nullptr, "Vertex Shader", edit);
	if (edit && chosenVS)
	{
		for (auto& sprite : sprites)
		{
			sprite->myVertexShader = chosenVS;
		}
	}
	if (ImGui::Checkbox("Instance Shader", &myInstanceShader)) 
	{
		for (auto& sprite : sprites)
		{
			sprite->myInstanceShader = myInstanceShader;
		}
	}
	if (ImGui::Button("Reload shaders"))
	{
		ReloadShaders();
	}
}


void UISprite::Save(rapidjson::Value& /*aComponentData*/)
{
	int version = 3;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveString("Texture", myTexture->GetName().c_str());
	wrapper.SaveString("VertexShader", myVertexShader->GetName().c_str());
	wrapper.SaveString("PixelShader", myPixelShader->GetName().c_str());
	wrapper.SaveValue<DataType::Int>("Layer", myLayer);
	wrapper.SaveValue<DataType::Bool>("Instance Shader", myInstanceShader);

	myUIRenderData.color.ParseToJsonObject(*wrapper.GetTarget(), wrapper.GetDocument(), "Color");
}

void UISprite::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Version"))
	{
		int version = aComponentData["Version"].GetInt();
		auto tex = AssetRegistry::GetInstance()->GetAsset<Texture>(aComponentData["Texture"].GetString());
		if (tex)
		{
			myTexture = tex;
		}

		if (version >= 2) 
		{
			myVertexShader = AssetRegistry::GetInstance()->GetAsset<VertexShader>(aComponentData["VertexShader"].GetString());
			myPixelShader = AssetRegistry::GetInstance()->GetAsset<PixelShader>(aComponentData["PixelShader"].GetString());
		
			if (version >= 3)
			{
				myLayer = aComponentData["Layer"].GetInt();
			}
		}
		if (aComponentData.HasMember("Instance Shader"))
		{
			myInstanceShader = aComponentData["Instance Shader"].GetBool();
		}
	}

	if (aComponentData.HasMember("Color"))
	{
		myUIRenderData.color.LoadFromJson(aComponentData["Color"]);
	}
}
void UISprite::Awake() 
{
	if (myInstanceShader) 
	{
		if (myPixelShader != nullptr) 
		{
			myPixelShader = myPixelShader->MakeInstance();
		}
	}
}
void UISprite::ReloadShaders()
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
				printerror("Pixel shader scuffed on: " + pixelName);
			}
		}
		else
		{
			printerror("Can't find: " + pixelName);
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
				printerror("Vertex shader scuffed on: " + vertexName);
			}
		}
		else
		{
			printerror("Can't find: " + vertexName);
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

std::filesystem::path UISprite::LoadFolder(const std::filesystem::path& aDirectory, const std::filesystem::path& aName)
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
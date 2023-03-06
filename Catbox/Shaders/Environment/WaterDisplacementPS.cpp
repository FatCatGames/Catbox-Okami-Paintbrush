#include "stdafx.h"
#include "WaterDisplacementPS.h"
#include "Graphics/Rendering/GraphicsEngine.h"
#include "Graphics/Rendering/Buffers/Buffers.h"

WaterDisplacementPS::WaterDisplacementPS()
{
	ifstream PSFile;
	PSFile.open("Resources/BuiltIn/Shaders/WaterDisplacementPS.cso", ios::binary);
	std::string PSData = { std::istreambuf_iterator<char>(PSFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(PSData.data(), PSData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading WaterDisplacementPS Pixel shader failed!");
	PSFile.close();
}

void WaterDisplacementPS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("WaveHeight", &myWaterDisplacementData.WaveHeight);
	myWasEdited |= Catbox::DragFloat("WaveSpeed", &myWaterDisplacementData.WaveSpeed);
	myWasEdited |= Catbox::DragFloat("WaveScale", &myWaterDisplacementData.WaveScale);
	myWasEdited |= Catbox::DragFloat("WaveAngle", &myWaterDisplacementData.WaveAngle);
	//myWasEdited |= Catbox::InputInt("Columns", &myUVPannerData.columns);
	//myWasEdited |= Catbox::InputInt("Framerate", &myWaterDisplacementData.framerate);
	//Catbox::InputFloat("Start delay", &myWaterDisplacementData.startDelay);
	//Catbox::InputFloat("Stop delay", &myWaterDisplacementData.stopDelay);
}

void WaterDisplacementPS::SetData()
{
	if (myWaterDisplacementDataBuffer.myBuffer != nullptr)
	{
		myWaterDisplacementDataBuffer.SetData(&myWaterDisplacementData);
	}
}

void WaterDisplacementPS::SaveData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Float>("WaveHeight", myWaterDisplacementData.WaveHeight);
	wrapper->SaveValue<DataType::Float>("WaveSpeed", myWaterDisplacementData.WaveSpeed);
	wrapper->SaveValue<DataType::Float>("WaveScale", myWaterDisplacementData.WaveScale);
	wrapper->SaveValue<DataType::Float>("WaveAngle", myWaterDisplacementData.WaveAngle);
	//wrapper->SaveValue<DataType::Int>("Framerate", myWaterDisplacementData.framerate);
	//wrapper->SaveValue<DataType::Bool>("Loop", myWaterDisplacementData.loop);
}

void WaterDisplacementPS::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	if (graphics.HasMember("Columns"))
	{
		myWaterDisplacementData.WaveHeight = graphics["WaveHeight"].GetFloat();
		myWaterDisplacementData.WaveSpeed = graphics["WaveSpeed"].GetFloat();
		myWaterDisplacementData.WaveScale = graphics["WaveScale"].GetFloat();
		myWaterDisplacementData.WaveAngle = graphics["WaveAngle"].GetFloat();
		//myWaterDisplacementData.framerate = graphics["Framerate"].GetInt();
		//myWaterDisplacementData.loop = graphics["Loop"].GetBool();
	}
}

void WaterDisplacementPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myWaterDisplacementDataBuffer.GetAddress());
	ID3D11ShaderResourceView* srv = { nullptr };
	
	Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetDepthStencil();
	//auto meme = Engine::GetInstance()->GetGraphicsEngine()->GetRenderTargetView().GetAddressOf()

	//	DX11::Context->OMSetRenderTargets(1, , nullptr);
}

std::shared_ptr<PixelShader> WaterDisplacementPS::MakeInstance()
{
	std::shared_ptr<WaterDisplacementPS> returnShader = std::make_shared<WaterDisplacementPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myWaterDisplacementData = myWaterDisplacementData;
	return returnShader;
}

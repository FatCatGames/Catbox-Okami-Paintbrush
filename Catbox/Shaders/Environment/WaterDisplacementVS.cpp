#include "stdafx.h"
#include "WaterDisplacementVS.h"

WaterDisplacementVS::WaterDisplacementVS()
{
	ifstream vsFile;
	vsFile.open("Resources/BuiltIn/Shaders/WaterDisplacementVS.cso", ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	assert(!FAILED(result) && "Loading WaterDisplacementVS vertex shader failed!");
	vsFile.close();
}

void WaterDisplacementVS::RenderInProperties()
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

void WaterDisplacementVS::SetData()
{
	if (myWaterDisplacementDataBuffer.myBuffer != nullptr)
	{
		myWaterDisplacementDataBuffer.SetData(&myWaterDisplacementData);
	}
}

void WaterDisplacementVS::SaveData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Float>("WaveHeight", myWaterDisplacementData.WaveHeight);
	wrapper->SaveValue<DataType::Float>("WaveSpeed", myWaterDisplacementData.WaveSpeed);
	wrapper->SaveValue<DataType::Float>("WaveScale", myWaterDisplacementData.WaveScale);
	wrapper->SaveValue<DataType::Float>("WaveAngle", myWaterDisplacementData.WaveAngle);
	//wrapper->SaveValue<DataType::Int>("Framerate", myWaterDisplacementData.framerate);
	//wrapper->SaveValue<DataType::Bool>("Loop", myWaterDisplacementData.loop);
}

void WaterDisplacementVS::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	if (graphics.HasMember("Columns"))
	{
		myWaterDisplacementData.WaveHeight =  graphics["WaveHeight"].GetFloat();
		myWaterDisplacementData.WaveSpeed  =  graphics["WaveSpeed"].GetFloat();
		myWaterDisplacementData.WaveScale  =  graphics["WaveScale"].GetFloat();
		myWaterDisplacementData.WaveAngle  =  graphics["WaveAngle"].GetFloat();
		//myWaterDisplacementData.framerate = graphics["Framerate"].GetInt();
		//myWaterDisplacementData.loop = graphics["Loop"].GetBool();
	}
}

void WaterDisplacementVS::SetResource()
{
	DX11::Context->VSSetConstantBuffers(10, 1, myWaterDisplacementDataBuffer.GetAddress());
}

std::shared_ptr<VertexShader> WaterDisplacementVS::MakeInstance()
{
	std::shared_ptr<WaterDisplacementVS> returnShader = std::make_shared<WaterDisplacementVS>();
	returnShader->vertexShader = this->vertexShader;
	returnShader->myWaterDisplacementData = myWaterDisplacementData;
	return returnShader;
}

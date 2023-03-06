#include "stdafx.h"
#include "FlipbookShader.h"

FlipbookShader::FlipbookShader()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/FlipbookPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading flipbook pixel shader failed!");
	psFile.close();
}

void FlipbookShader::RenderInProperties()
{
	myWasEdited = false;
	myWasEdited |= Catbox::Checkbox("Looping", &myFlipbookData.loop);
	myWasEdited |= Catbox::InputInt("Rows", &myFlipbookData.rows);
	myWasEdited |= Catbox::InputInt("Columns", &myFlipbookData.columns);
	myWasEdited |= Catbox::InputInt("Framerate", &myFlipbookData.framerate);
	//Catbox::InputFloat("Start delay", &myFlipbookData.startDelay);
	//Catbox::InputFloat("Stop delay", &myFlipbookData.stopDelay);
}

void FlipbookShader::SetData()
{
	if (myFlipbookDataBuffer.myBuffer != nullptr)
	{
		myFlipbookDataBuffer.SetData(&myFlipbookData);
	}
}

void FlipbookShader::SaveData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Int>("Columns", myFlipbookData.columns);
	wrapper->SaveValue<DataType::Int>("Rows", myFlipbookData.rows);
	wrapper->SaveValue<DataType::Int>("Framerate", myFlipbookData.framerate);
	wrapper->SaveValue<DataType::Bool>("Loop", myFlipbookData.loop);
}

void FlipbookShader::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	if (graphics.HasMember("Columns")) 
	{
		myFlipbookData.columns = graphics["Columns"].GetInt();
		myFlipbookData.rows = graphics["Rows"].GetInt();
		myFlipbookData.framerate = graphics["Framerate"].GetInt();
		myFlipbookData.loop = graphics["Loop"].GetBool();
	}
}

void FlipbookShader::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myFlipbookDataBuffer.GetAddress());
}

std::shared_ptr<PixelShader> FlipbookShader::MakeInstance()
{
	std::shared_ptr<FlipbookShader> returnShader = std::make_shared<FlipbookShader>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myFlipbookData = myFlipbookData;
	return returnShader;
}

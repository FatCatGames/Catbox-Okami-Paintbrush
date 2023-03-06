#include "stdafx.h"
#include "UVPannerShader.h"

UVPannerShader::UVPannerShader()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/UVPannerPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading UVPanner pixel shader failed!");
	psFile.close();
}

void UVPannerShader::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("X-Speed", &myUVPannerData.xSpeed);
	myWasEdited |= Catbox::DragFloat("Y-Speed", &myUVPannerData.ySpeed);
	myWasEdited |= Catbox::Checkbox("Frac Time", &myUVPannerData.isFrac);
	myWasEdited |= Catbox::Checkbox("Accelerate", &myUVPannerData.isAccelerate);
	//myWasEdited |= Catbox::InputInt("Columns", &myUVPannerData.columns);
	//myWasEdited |= Catbox::InputInt("Framerate", &myUVPannerData.framerate);
	//Catbox::InputFloat("Start delay", &myUVPannerData.startDelay);
	//Catbox::InputFloat("Stop delay", &myUVPannerData.stopDelay);
}

void UVPannerShader::SetData()
{
	if (myUVPannerDataBuffer.myBuffer != nullptr)
	{
		myUVPannerDataBuffer.SetData(&myUVPannerData);
	}
}

void UVPannerShader::SaveData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Float>("X-Speed", myUVPannerData.xSpeed);
	wrapper->SaveValue<DataType::Float>("Y-Speed", myUVPannerData.ySpeed);
	wrapper->SaveValue<DataType::Bool>("Frac Time", myUVPannerData.isFrac);
	wrapper->SaveValue<DataType::Bool>("Accelerate", myUVPannerData.isAccelerate);
	//wrapper->SaveValue<DataType::Int>("Framerate", myUVPannerData.framerate);
	//wrapper->SaveValue<DataType::Bool>("Loop", myUVPannerData.loop);
}

void UVPannerShader::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	if (graphics.HasMember("Columns"))
	{
		myUVPannerData.xSpeed = graphics["X-Speed"].GetFloat();
		myUVPannerData.ySpeed = graphics["Y-Speed"].GetFloat();
		//myUVPannerData.framerate = graphics["Framerate"].GetInt();
		//myUVPannerData.loop = graphics["Loop"].GetBool();
	}
}

void UVPannerShader::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myUVPannerDataBuffer.GetAddress());
}

std::shared_ptr<PixelShader> UVPannerShader::MakeInstance()
{
	std::shared_ptr<UVPannerShader> returnShader = std::make_shared<UVPannerShader>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myUVPannerData = myUVPannerData;
	return returnShader;
}

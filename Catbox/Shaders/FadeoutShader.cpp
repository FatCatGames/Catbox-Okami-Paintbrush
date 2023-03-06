#include "stdafx.h"
#include "FadeoutShader.h"

FadeoutShader::FadeoutShader()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/FadeoutPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading Fadeout pixel shader failed!");
	psFile.close();
}

void FadeoutShader::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("LifeTime", &myFadeoutData.lifeTime);
	//myWasEdited |= Catbox::DragFloat("Y-Speed", &myFadeoutData.ySpeed);
	//myWasEdited |= Catbox::Checkbox("Frac Time", &myFadeoutData.isFrac);
	//myWasEdited |= Catbox::Checkbox("Accelerate", &myFadeoutData.isAccelerate);
	//myWasEdited |= Catbox::InputInt("Columns", &myFadeoutData.columns);
	//myWasEdited |= Catbox::InputInt("Framerate", &myFadeoutData.framerate);
	//Catbox::InputFloat("Start delay", &myFadeoutData.startDelay);
	//Catbox::InputFloat("Stop delay", &myFadeoutData.stopDelay);
}

void FadeoutShader::SetData()
{
	if (myFadeoutDataBuffer.myBuffer != nullptr)
	{
		myFadeoutDataBuffer.SetData(&myFadeoutData);
	}
}

void FadeoutShader::SaveData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Float>("LifeTime", myFadeoutData.lifeTime);
	//wrapper->SaveValue<DataType::Float>("Y-Speed", myFadeoutData.ySpeed);
	//wrapper->SaveValue<DataType::Bool>("Frac Time", myFadeoutData.isFrac);
	//wrapper->SaveValue<DataType::Bool>("Accelerate", myFadeoutData.isAccelerate);
	//wrapper->SaveValue<DataType::Int>("Framerate", myFadeoutData.framerate);
	//wrapper->SaveValue<DataType::Bool>("Loop", myFadeoutData.loop);
}

void FadeoutShader::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	if (graphics.HasMember("Columns"))
	{
		myFadeoutData.lifeTime = graphics["LifeTime"].GetFloat();
		//myFadeoutData.ySpeed = graphics["Y-Speed"].GetFloat();
		//myFadeoutData.framerate = graphics["Framerate"].GetInt();
		//myFadeoutData.loop = graphics["Loop"].GetBool();
	}
}

void FadeoutShader::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myFadeoutDataBuffer.GetAddress());
}

std::shared_ptr<PixelShader> FadeoutShader::MakeInstance()
{
	std::shared_ptr<FadeoutShader> returnShader = std::make_shared<FadeoutShader>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myFadeoutData = myFadeoutData;
	return returnShader;
}

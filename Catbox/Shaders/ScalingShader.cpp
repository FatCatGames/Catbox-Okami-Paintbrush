#include "stdafx.h"
#include "ScalingShader.h"

ScalingShader::ScalingShader()
{
	ifstream vsFile;
	vsFile.open("Resources/BuiltIn/Shaders/ScalingVS.cso", ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	assert(!FAILED(result) && "Loading Scaling vertex shader failed!");
	vsFile.close();
}

void ScalingShader::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("X-Scale",        &myScalingData.xScale          );
	myWasEdited |= Catbox::DragFloat("Y-Scale",        &myScalingData.yScale          );
	myWasEdited |= Catbox::DragFloat("Z-Scale",        &myScalingData.zScale          );
	myWasEdited |= Catbox::Checkbox("SmoothStop",      &myScalingData.isSmoothStop    );
	myWasEdited |= Catbox::Checkbox("SmoothStart",     &myScalingData.isSmoothStart   );
	myWasEdited |= Catbox::Checkbox("BellCurve",       &myScalingData.isBellCurve     );
	//myWasEdited |= Catbox::InputInt("Columns", &myScalingData.columns);
	//myWasEdited |= Catbox::InputInt("Framerate", &myScalingData.framerate);
	//Catbox::InputFloat("Start delay", &myScalingData.startDelay);
	//Catbox::InputFloat("Stop delay", &myScalingData.stopDelay);
}

void ScalingShader::SetData()
{
	if (myScalingBufferData.myBuffer != nullptr)
	{
		myScalingBufferData.SetData(&myScalingData);
	}
}

void ScalingShader::SaveData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Float>("X-Scale",   myScalingData.xScale          );
	wrapper->SaveValue<DataType::Float>("Y-Scale",   myScalingData.yScale          );
	wrapper->SaveValue<DataType::Float>("Z-Scale",   myScalingData.zScale          );
	wrapper->SaveValue<DataType::Bool>("SmoothStop", myScalingData.isSmoothStop);
	wrapper->SaveValue<DataType::Bool>("SmoothStart", myScalingData.isSmoothStart);
	wrapper->SaveValue<DataType::Bool>("BellCurve", myScalingData.isBellCurve);
}

void ScalingShader::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	if (graphics.HasMember("Columns"))
	{
		myScalingData.xScale = graphics["X-Scale"].GetFloat();
		myScalingData.yScale = graphics["Y-Scale"].GetFloat();
		myScalingData.zScale = graphics["Z-Scale"].GetFloat();
		myScalingData.isSmoothStop = graphics["SmoothStop"].GetBool();
		myScalingData.isSmoothStart = graphics["SmoothStart"].GetBool();
		myScalingData.isBellCurve = graphics["BellCurve"].GetBool();
		//myScalingData.xSpeed = graphics["X-Speed"].GetFloat();
		//myScalingData.ySpeed = graphics["Y-Speed"].GetFloat();
		//myScalingData.framerate = graphics["Framerate"].GetInt();
		//myScalingData.loop = graphics["Loop"].GetBool();
	}
}

void ScalingShader::SetResource()
{
	DX11::Context->VSSetConstantBuffers(10, 1, myScalingBufferData.GetAddress());
}

std::shared_ptr<VertexShader> ScalingShader::MakeInstance()
{
	std::shared_ptr<ScalingShader> returnShader = std::make_shared<ScalingShader>();
	returnShader->vertexShader = this->vertexShader;
	returnShader->myScalingData = myScalingData;
	return returnShader;
}

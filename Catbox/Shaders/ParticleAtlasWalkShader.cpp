#include "stdafx.h"
#include "ParticleAtlasWalkShader.h"

ParticleAtlasWalkShader::ParticleAtlasWalkShader()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/ParticleAtlasWalkPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading ParticleAtlasWalk pixel shader failed!");
	psFile.close();
}

void ParticleAtlasWalkShader::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;

	myWasEdited |= Catbox::InputInt("Rows", &myParticleAtlasWalkData.Rows);
	myWasEdited |= Catbox::InputInt("Columns", &myParticleAtlasWalkData.Columns);
	myWasEdited |= Catbox::InputInt("Start sprite", &myParticleAtlasWalkData.Sprite);
	myWasEdited |= Catbox::InputInt("End sprite", &myParticleAtlasWalkData.NoSprites);
	//Catbox::InputFloat("Start delay", &myParticleAtlasWalkData.startDelay);
	//Catbox::InputFloat("Stop delay", &myParticleAtlasWalkData.stopDelay);
}

void ParticleAtlasWalkShader::SetData()
{
	if (myParticleAtlasWalkDataBuffer.myBuffer != nullptr)
	{
		myParticleAtlasWalkDataBuffer.SetData(&myParticleAtlasWalkData);
	}
}

void ParticleAtlasWalkShader::SaveData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Int>("Columns", myParticleAtlasWalkData.Columns);
	wrapper->SaveValue<DataType::Int>("Rows", myParticleAtlasWalkData.Rows);
	wrapper->SaveValue<DataType::Int>("Start sprite", myParticleAtlasWalkData.Sprite);
	wrapper->SaveValue<DataType::Int>("End sprite", myParticleAtlasWalkData.NoSprites);
}

void ParticleAtlasWalkShader::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	if (graphics.HasMember("Columns"))
	{
		myParticleAtlasWalkData.Columns = graphics["Columns"].GetInt();
		myParticleAtlasWalkData.Rows = graphics["Rows"].GetInt();
		myParticleAtlasWalkData.Sprite = graphics["Start sprite"].GetInt();
		myParticleAtlasWalkData.NoSprites = graphics["End sprite"].GetInt();
	}
}

void ParticleAtlasWalkShader::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myParticleAtlasWalkDataBuffer.GetAddress());
}

std::shared_ptr<PixelShader> ParticleAtlasWalkShader::MakeInstance()
{
	std::shared_ptr<ParticleAtlasWalkShader> returnShader = std::make_shared<ParticleAtlasWalkShader>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myParticleAtlasWalkData = myParticleAtlasWalkData;
	return returnShader;
}

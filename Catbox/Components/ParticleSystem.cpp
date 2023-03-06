#include "stdafx.h"
#include "ParticleSystem.h"
#include "UtilityFunctions.hpp"


void ParticleSystem::Update()
{
	for (auto& emitter : myEmitters)
	{
		emitter.Update();
	}

	Render();
}


void ParticleSystem::RunInEditor()
{
	//if (myGameObject->IsSelected(0))
	{
		for (auto& emitter : myEmitters)
		{
			emitter.Update();
		}
	}

	Render();
}

void ParticleSystem::Render()
{
	Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(this);
}

std::vector<ParticleEmitter>& ParticleSystem::GetEmitters()
{
	return myEmitters;
}

void ParticleSystem::AddEmitter(std::shared_ptr<ParticleEmitterSettings> aSettings)
{
	if (!aSettings) 
	{
		printerror("Tried to add emitter that does not exist!");
		return;
	}
	myEmitters.push_back(ParticleEmitter(myTransform));
	myEmitters.back().SetSharedData(aSettings, true);
}

void ParticleSystem::Save(rapidjson::Value& aComponentData)
{
	auto parser = RapidJsonWrapper::GetInstance();
	auto& alloc = parser->GetDocument().GetAllocator();

	auto emittersArray = parser->CreateArray();
	emittersArray.SetArray();
	for (size_t i = 0; i < myEmitters.size(); i++)
	{
		auto ref = rapidjson::GenericStringRef(rapidjson::StringRef(myEmitters[i].GetSharedData()->GetName().c_str()));
		emittersArray.PushBack(ref, alloc);
	}

	aComponentData.AddMember("Emitters", emittersArray, alloc);
}

void ParticleSystem::Load(rapidjson::Value& aComponentData)
{
	if (!aComponentData.HasMember("Emitters")) return;
	auto emitters = aComponentData["Emitters"].GetArray();

	for (auto& emitterPath : emitters)
	{
		AddEmitter(AssetRegistry::GetInstance()->GetAsset<ParticleEmitterSettings>(emitterPath.GetString()));
	}
}

void ParticleSystem::OnObjectFinishedLoading()
{
	for (auto& emitter : myEmitters)
	{
		emitter.Init();
	}
}

void ParticleSystem::Play()
{
	Replay();
}

void ParticleSystem::Replay()
{
	for (auto& emitter : myEmitters)
	{
		emitter.Replay();
	}
}

void ParticleSystem::RenderInProperties(std::vector<Component*>& aComponentList)
{
	for (size_t i = 0; i < myEmitters.size(); i++)
	{
		if (i != 0) ImGui::Separator();
		ImGui::Unindent(20);
		ImGui::SetNextItemWidth(200);
		if (ImGui::CollapsingHeader((myEmitters[i].GetSharedData()->GetName() + "##" + std::to_string(i)).c_str()))
		{
			ImGui::Indent(20);
			myEmitters[i].RenderInProperties();

			if (ImGui::Button("Remove", { 70, 30 }))
			{
				myEmitters.erase(myEmitters.begin() + i);
				--i;
				continue;
			}
		}
		else ImGui::Indent(20);
		ImGui::Spacing();
	}

	if (ImGui::BeginPopupContextItem("Add emitter"))
	{
		if (ImGui::Selectable("Create New"))
		{
			std::string filePath = Catbox::SaveFile("Particle Emitter (*.pe)\0*.pe\0");
			if (!filePath.empty())
			{
				auto sharedData = CreateAsset<ParticleEmitterSettings>(filePath);
				myEmitters.push_back(ParticleEmitter(myTransform));
				myEmitters.back().SetSharedData(sharedData);
			}
		}
		if (ImGui::Selectable("Load Existing"))
		{
			std::string filePath = Catbox::OpenFile("Particle Emitter (*.pe)\0*.pe\0");
			if (!filePath.empty())
			{
				string emitterName = Catbox::GetNameFromPath(filePath);
				AddEmitter(AssetRegistry::GetInstance()->GetAsset<ParticleEmitterSettings>(emitterName));
			}
		}
		ImGui::EndPopup();
	}

	ImGui::Spacing();
	if (ImGui::Button("Add Emitter"))
	{
		ImGui::OpenPopup("Add emitter");
	}
}
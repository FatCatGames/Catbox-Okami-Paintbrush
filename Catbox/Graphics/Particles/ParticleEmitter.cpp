#include "stdafx.h"
#include "ParticleEmitter.h"
#include <UtilityFunctions.hpp>
#include <cassert>
#include "Graphics/Rendering/DX11/DX11.h"
#include "Assets/Texture.h"
#include "Assets/Shader.h"
#include "Assets/Material.h"
#include "Shaders\FlipbookShader.h"

void ParticleEmitter::RenderInProperties()
{
	ImGui::Spacing();
	bool edit;
	auto newPS = AssetRegistry::GetInstance()->AssetDropdownMenu<ParticleEmitterSettings>(mySharedData.get(), "Settings", edit);
	if (edit)
	{
		SetSharedData(newPS);
	}
	ImGui::Spacing();

	ImGui::Text(("Active particles:" + std::to_string(myParticlesToRender)).c_str());
	mySharedData->RenderInProperties();

	if (mySharedData->myNeedsRestart)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 255));
		ImGui::Text("Needs restart!");
		ImGui::PopStyleColor();
	}


	if (ImGui::Button("Replay", { 80, 30 }))
	{
		if (mySharedData->myNeedsRestart)
		{
			mySharedData->myNeedsRestart = false;
			UpdateMaxParticles();
			Replay();
		}
		else Replay();
	}

}

void ParticleEmitter::SpawnParticles()
{
	float percent = 1.f - (myInstanceData.timePassed / mySharedData->myDuration);
	myInstanceData.timeUntilEmission = mySharedData->myTimeBetweenEmissions.GetValue(percent);
	int particleCount = mySharedData->myParticlesPerEmission.GetValue(percent);

	if (particleCount > 0)
	{
		std::vector<ParticleData*> newParticles(particleCount);

		for (int i = 0; i < particleCount; i++)
		{
			ResetParticle(myParticles[myParticleIndex]);
			newParticles[i] = &myParticles[myParticleIndex];
			if (++myParticleIndex >= myParticles.size()) myParticleIndex = 0;
		}

		CreateInitialShape(newParticles, mySharedData->myShapeData);
	}
}

void ParticleEmitter::Update()
{
	if (!mySharedData) return;
	myParticlesToRender = 0;
	myInstanceData.timeUntilEmission -= deltaTime;
	myInstanceData.timePassed += deltaTime;

	//Updating particles
	Vector4f offset = { 0,0,0,0 };
	if (!mySharedData->myUseWorldSpace)
	{
		Vector3f worldPos = myParent->worldPos();
		offset = { worldPos.x, worldPos.y, worldPos.z, 0 };
	}

	for (size_t i = 0; i < myParticles.size(); i++)
	{
		if (myParticles[i].remainingLifetime <= 0) continue;
		ParticleData& p = myParticles[i];
		if (p.spawnTimer > 0)
		{
			p.spawnTimer -= deltaTime;
			continue;
		}
		p.remainingLifetime -= deltaTime;
		myParticleBufferData[myParticlesToRender].lifetime = p.startLifetime - p.remainingLifetime;

		const float percent = 1 - (p.remainingLifetime / p.startLifetime);

		if (mySharedData->mySpeedOverLifetimeEnabled)
		{
			p.currentSpeed = p.startSpeed * mySharedData->mySpeedOverLifetime.Evaluate(percent);
		}

		if (mySharedData->myUseGravity)
		{
			if (p.velocity.y > mySharedData->myGravityModifier * -9.81f)
			{
				p.velocity.y -= mySharedData->myGravityModifier * 9.81f * deltaTime;
			}
		}

		Vector4f dir = p.velocity;
		if (mySharedData->myVelocityOverLifetimeEnabled)
		{
			dir.x += mySharedData->myVelocityOverLifetimeX.Evaluate(percent) * mySharedData->myVelocityOverTimeInfluence;
			dir.y += mySharedData->myVelocityOverLifetimeY.Evaluate(percent) * mySharedData->myVelocityOverTimeInfluence;
			dir.z += mySharedData->myVelocityOverLifetimeZ.Evaluate(percent) * mySharedData->myVelocityOverTimeInfluence;
		}
		p.position += dir * p.currentSpeed * deltaTime;

		if (mySharedData->myRotationOverLifetimeEnabled)
		{
			p.currentRotation.z = p.startRotation.z + mySharedData->myRotationOverLifetime.Evaluate(percent) * 360;
		}

		if (mySharedData->mySizeOverLifetimeEnabled)
		{
			p.currentSize = p.startSize * mySharedData->mySizeOverLifetime.Evaluate(percent);
		}


		if (mySharedData->myColorOverLifetimeEnabled)
		{
			myParticleBufferData[myParticlesToRender].color = mySharedData->myColorOverLifetime.Evaluate(percent);
		}
		else
		{
			Color col = mySharedData->myStartColor;
			myParticleBufferData[myParticlesToRender].color = { col.r, col.g, col.b, col.a };
		}

		myParticleBufferData[myParticlesToRender].position = p.position + offset;

		myParticleBufferData[myParticlesToRender].scale = Vector3f::one() * p.currentSize * 0.1f;
		myParticleBufferData[myParticlesToRender].index = p.index;
		myParticleBufferData[myParticlesToRender].rotation = Catbox::Deg2Rad(p.currentRotation.z);
		myParticlesToRender++;
	}


	//Spawning new particles

	if (myIsPaused) return;
	myDelayTimer -= deltaTime;
	if (myDelayTimer > 0) return;
	if (myInstanceData.timeUntilEmission <= 0)
	{
		SpawnParticles();
	}
	if (myInstanceData.timePassed >= mySharedData->myDuration)
	{
		if (!mySharedData->myIsLooping)
		{
			myIsPaused = true;
		}
	}
}

void ParticleEmitter::CreateInitialShape(std::vector<ParticleEmitter::ParticleData*>& someParticles, ParticleEmitterSettings::ShapeData& aShapeData)
{
	Vector4f spawnPos = { aShapeData.offset.x, aShapeData.offset.y, aShapeData.offset.z, 1};
	if (mySharedData->myUseWorldSpace)
	{
		Vector3f parentPos = myParent->worldPos();
		spawnPos += { parentPos.x, parentPos.y, parentPos.z, 0 };
	}

	for (int i = 0; i < someParticles.size(); i++)
	{
		someParticles[i]->position = spawnPos;

		if (aShapeData.shape == ParticleEmitterSettings::EmissionShape::Cone)
		{
			someParticles[i]->velocity = { Catbox::GetRandom(-0.5f, 0.5f), Catbox::GetRandom(1.f, 1.5f), Catbox::GetRandom(-0.5f, 0.5f), 0 };
		}
		else if (aShapeData.shape == ParticleEmitterSettings::EmissionShape::Sphere)
		{
			someParticles[i]->velocity = { Catbox::GetRandom(-1.f, 1.f), Catbox::GetRandom(-1.f, 1.f), Catbox::GetRandom(-1.f, 1.f), 0 };
		}
		else if (aShapeData.shape == ParticleEmitterSettings::EmissionShape::Edge)
		{
			someParticles[i]->velocity = { 0.f, 1.f, 0.f, 0 };
			//someParticles[i]->position += { i / static_cast<float>(someParticles.size()), 0, 0, 1 };
		}
	}
}

void ParticleEmitter::ResetParticle(ParticleData& aParticle)
{
	float percent = 1.f - (myInstanceData.timePassed / mySharedData->myDuration);
	aParticle.velocity = { 0,0,0,0 };
	aParticle.currentRotation.z = aParticle.startRotation.z = mySharedData->myRotation.GetValue(percent);
	aParticle.spawnTimer = mySharedData->mySpawnDelay.GetValue(percent);
	aParticle.startColor = mySharedData->myStartColor;

	aParticle.currentSize = aParticle.startSize = mySharedData->mySize.GetValue(percent);
	if (mySharedData->mySizeOverLifetimeEnabled)
	{
		aParticle.currentSize *= mySharedData->mySizeOverLifetime.Evaluate(0);
	}

	aParticle.currentSpeed = aParticle.startSpeed = mySharedData->mySpeed.GetValue(percent);
	if (mySharedData->mySpeedOverLifetimeEnabled)
	{
		aParticle.currentSpeed *= mySharedData->mySpeedOverLifetime.Evaluate(0);
	}

	aParticle.endSpeed = mySharedData->myEndSpeed;
	aParticle.startLifetime = mySharedData->myLifetime.GetValue(percent);
	aParticle.remainingLifetime = aParticle.startLifetime;
}

void ParticleEmitter::UpdateParticleSystem()
{
	for (auto& p : myParticles)
	{
		ResetParticle(p);
		p.remainingLifetime = 0;
	}
}

void ParticleEmitter::Replay()
{
	if (mySharedData->myNeedsRestart)
	{
		mySharedData->myNeedsRestart = false;
	}

	myInstanceData.timePassed = 0;
	myInstanceData.timeUntilEmission = 0;
	myDelayTimer = mySharedData->myStartDelay;
	UpdateParticleSystem();
	myParticleIndex = 0;
	myParticlesToRender = 0;
	myIsPaused = false;
}

void ParticleEmitter::SetAsResource(Catbox::CBuffer<Material::MaterialBufferData>& aMaterialBuffer)
{
	if (myParticleBufferData.size() == 0) return;

	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE bufferData;
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = DX11::Context->Map(mySharedData->myVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	memcpy_s(bufferData.pData, sizeof(ParticleBufferData) * myParticleBufferData.size(), &myParticleBufferData[0], sizeof(ParticleBufferData) * myParticleBufferData.size());

	DX11::Context->Unmap(mySharedData->myVertexBuffer.Get(), 0);

	DX11::Context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(mySharedData->myPrimitiveTopology));
	DX11::Context->IASetInputLayout(mySharedData->myVertexShader->inputLayout.Get());
	DX11::Context->IASetVertexBuffers(0, 1, mySharedData->myVertexBuffer.GetAddressOf(), &mySharedData->myStride, &mySharedData->myDXOffset);



	if (mySharedData->myType == ParticleEmitterSettings::RenderType::Material)
	{
		mySharedData->myMaterial->SetAsResource(aMaterialBuffer);
	}
	else 
	{
		DX11::Context->VSSetShader(mySharedData->myVertexShader->vertexShader, nullptr, 0);
		DX11::Context->GSSetShader(mySharedData->myGeometryShader->geometryShader, nullptr, 0);
		if (mySharedData->myType == ParticleEmitterSettings::RenderType::Default)
		{
			mySharedData->myDefaultMaterial->SetAsResource(aMaterialBuffer);
			DX11::Context->PSSetShader(mySharedData->myMaterial->GetPixelShader()->pixelShader, nullptr, 0);
		}
		else if (mySharedData->myType == ParticleEmitterSettings::RenderType::Flipbook)
		{
			DX11::Context->PSSetShader(mySharedData->myFlipbookShader->pixelShader, nullptr, 0);
			mySharedData->myFlipbookShader->SetData();
			mySharedData->myFlipbookShader->SetResource();
		}
		else if (mySharedData->myType == ParticleEmitterSettings::RenderType::AtlasWalk)
		{
			DX11::Context->PSSetShader(mySharedData->myAtlasWalkShader->pixelShader, nullptr, 0);
			mySharedData->myAtlasWalkShader->SetData();
			mySharedData->myAtlasWalkShader->SetResource();
		}


		if (mySharedData->myTexture)
		{
			mySharedData->myTexture->SetAsResource(0);
		}
	}
}

void ParticleEmitter::Render()
{
	if (myParticleBufferData.empty()) return;
	DX11::Context->Draw(myParticlesToRender, 0);
}


void ParticleEmitter::UpdateMaxParticles()
{
	size_t prev = myParticles.size();
	myParticles.resize(mySharedData->myMaxParticles);
	myParticleBufferData.resize(mySharedData->myMaxParticles);

	for (size_t i = prev; i < mySharedData->myMaxParticles; i++)
	{
		myParticles[i].remainingLifetime = 0;
		myParticles[i].index = static_cast<int>(i);
	}

	mySharedData->mySubresourceData.pSysMem = &myParticles[0];
	HRESULT result = DX11::Device->CreateBuffer(&mySharedData->myVertexBufferDesc, &mySharedData->mySubresourceData, mySharedData->myVertexBuffer.GetAddressOf());
}


void ParticleEmitter::SetSharedData(std::shared_ptr<ParticleEmitterSettings> someSharedData, bool reloadParticles)
{
	mySharedData = someSharedData;
	myDelayTimer = mySharedData->myStartDelay;
	if (reloadParticles) UpdateMaxParticles();
}

void ParticleEmitter::Init()
{
	UpdateMaxParticles();
}


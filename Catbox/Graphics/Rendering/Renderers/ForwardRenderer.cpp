#include "stdafx.h"
#include "../GraphicsEngine.pch.h"
#include "ForwardRenderer.h"
#include "../DX11/DX11.h"
#include "Components/3D/Camera.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Material.h"
#include "Assets/MeshData.h"
#include "Assets/Model.h"
#include "Components/ParticleSystem.h"
#include "Assets/Scene.h"
#include "../Buffers/Buffers.h"
#include "../DXDeviceStates.h"

bool ForwardRenderer::Initialize(std::shared_ptr<Buffers> aBuffers)
{
	myBuffers = aBuffers;
	return true;
}

void ForwardRenderer::SetDebugRenderMode(int aRenderMode)
{
	myBuffers->myFrameBufferData.DebugMode = aRenderMode;
}


void ForwardRenderer::Render(const std::vector<ModelInstance*>& aModelList, const std::vector<ParticleSystem*>& aParticleSystemList, ComPtr<ID3D11RenderTargetView> aTarget)
{
	myDrawCalls = 0;
	DX11::Context->VSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
	DX11::Context->PSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
	DX11::Context->GSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());

	ID3D11ShaderResourceView* srv = { nullptr };

	DX11::Context->PSSetShaderResources(GBuffer::GBufferTexture::Position, 1, &srv);

	ID3D11RenderTargetView* buffers[4];
	buffers[0] = aTarget.Get();
	buffers[1] = DX11::SelectionBuffer.Get();
	buffers[2] = myBuffers->myGBuffer.GetTex(GBuffer::GBufferTexture::Position).GetRenderTargetView().Get();
	buffers[3] = GraphicsEngine::GetInstance()->GetLightMap()->GetRenderTargetView().Get();

	auto dsv = Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetDepthStencil().GetDepthStencilView().Get();
	DX11::Context->OMSetRenderTargets(4, buffers, dsv);

	std::vector<ModelInstance*> orderedList = OrderBackToFront(aModelList);

	RenderModels(orderedList);
	RenderParticles(aParticleSystemList);

	srv = myBuffers->myGBuffer.GetTex(GBuffer::GBufferTexture::Position).GetShaderResourceView().Get();
	buffers[0] = nullptr;
	buffers[1] = nullptr;
	buffers[2] = nullptr;
	buffers[3] = nullptr;
	DX11::Context->OMSetRenderTargets(4, buffers, nullptr);
	DX11::Context->PSSetShaderResources(GBuffer::GBufferTexture::Position, 1, &srv);
}

void ForwardRenderer::SetBlendState(const BlendState& aBlendState)
{
	DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(aBlendState)].Get(), nullptr, 0xffffffff);
}
void ForwardRenderer::SetRastertizerState(const RastertizerState& aState)
{
	DX11::Context->RSSetState( RastertizerStates::GetRastertizerStates()[static_cast<int>(aState)].Get());
}

void ForwardRenderer::SetDepthStencilState(const DepthStencilState& aDepthStencilState)
{
	DX11::Context->OMSetDepthStencilState(DepthStencilStates::GetDepthStencilStates()[static_cast<int>(aDepthStencilState)].Get(), 0xffffffff);
}

std::vector<ModelInstance*> ForwardRenderer::OrderBackToFront(std::vector<ModelInstance*> aModelList)
{
	if (Engine::GetInstance()->GetMainCamera() != nullptr)
	{
		std::vector<ModelInstance*> returnVector = aModelList;
		Vector3f aCameraPos = Engine::GetInstance()->GetMainCamera()->GetTransform()->worldPos();
		std::vector<float> distances(aModelList.size());
		for (int i = 0; i < aModelList.size(); i++)
		{
			for (int j = 0; j < aModelList.size() - i - 1; j++)
			{
				if (returnVector[j]->GetTransform()->GetCenter().Distance(aCameraPos) < returnVector[j + 1]->GetTransform()->GetCenter().Distance(aCameraPos))
				{
					auto temp = returnVector[j];
					returnVector[j] = returnVector[j + 1];
					returnVector[j + 1] = temp;
					float distance = returnVector[j]->GetTransform()->worldPos().Distance(aCameraPos);
					distances[j] = returnVector[j + 1]->GetTransform()->worldPos().Distance(aCameraPos);
					distances[j + 1] = distance;
				}
			}
		}
		return returnVector;
	}
	return aModelList;
}
int ForwardRenderer::GetDebugRenderMode() 
{
	return myBuffers->myFrameBufferData.DebugMode;
}

void ForwardRenderer::RenderModels(const std::vector<ModelInstance*>& aModelList)
{
	myDrawCalls = 0;
	DX11::Context->PSSetConstantBuffers(4, 1, myBuffers->mySceneLightsBuffer.GetAddress());
	DX11::Context->PSSetShaderResources(9, 1, myBuffers->myLightBuffer.GetSRV().GetAddressOf());

	ID3D11ShaderResourceView* clearSrvs[1] = { nullptr };
	if (myBuffers->myEnvironmentLight)
	{
		DX11::Context->PSSetShaderResources(10, 1, myBuffers->myEnvironmentLight->GetTexture()->GetShaderResourceView().GetAddressOf());
	}
	else
	{
		DX11::Context->PSSetShaderResources(10, 1, &clearSrvs[0]);
	}

	DX11::Context->PSSetShaderResources(11, 1, GraphicsEngine::GetInstance()->GetPreviousDepth()->GetShaderResourceView().GetAddressOf());

	for (auto& modelInstance : aModelList)
	{
		auto model = modelInstance->GetModel();
		if (!model) continue;
		std::vector<std::shared_ptr<MeshData>> modelParts = modelInstance->GetModel()->GetModelParts();
		int index = -1;
		const std::vector<Model::InstanceData>& renderedInstances = model->GetRenderedInstances();

		for (auto& meshData : modelParts)
		{
			++index;
			if (meshData == nullptr) continue;
			std::shared_ptr<Material> material = modelInstance->GetMaterial(index);
			if (!material) continue;
			if (!material->GetVertexShader()) continue;

			myBuffers->myObjectBufferData.isInstanced = false;
			myBuffers->myObjectBufferData.world = modelInstance->GetTransform()->GetWorldTransformMatrix();
			myBuffers->myObjectBufferData.scale = modelInstance->GetTransform()->worldScale();
			myBuffers->myObjectBufferData.id = modelInstance->GetGameObject().GetObjectInstanceID();
			myBuffers->myObjectBufferData.aliveTime = modelInstance->GetGameObject().GetAliveTime();
			if (modelInstance->GetModel()->IsUnitsConverted())
			{
				for (int row = 1; row < 4; row++)
				{
					for (int col = 1; col < 4; col++)
					{
						myBuffers->myObjectBufferData.world(row, col) *= 0.01f;
					}
				}
			}

#pragma region Skeleton
			std::shared_ptr<SkeletonInstance> skeleton = modelInstance->GetSkeleton();
			if (skeleton)
			{
				myBuffers->myObjectBufferData.isStatic = skeleton->sharedData == nullptr;
			}
			else myBuffers->myObjectBufferData.isStatic = true;


			if (skeleton && skeleton->sharedData)
			{
				memcpy_s(myBuffers->mySkeletonBufferData.BoneData, sizeof(Matrix4x4<float>) * 128, skeleton->boneMatrices.data(), sizeof(Matrix4x4<float>) * 128);
				myBuffers->mySkeletonBuffer.SetData(&myBuffers->mySkeletonBufferData);
			}
#pragma endregion
			myBuffers->myObjectBuffer.SetData(&myBuffers->myObjectBufferData);
			SetBlendState(material->GetBlendState());
			if (myBuffers->myFrameBufferData.DebugMode == 10)
			{
				SetRastertizerState(RastertizerState::RS_WireFrame);
			}
			else
			{
				SetRastertizerState(material->GetRastertizerState());
			}

			material->SetAsResource(myBuffers->myMaterialBuffer);

			DX11::Context->IASetVertexBuffers(0, 1, meshData->myVertexBuffer.GetAddressOf(), &meshData->myStride, &meshData->myOffset);
			DX11::Context->IASetIndexBuffer(meshData->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			DX11::Context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(meshData->myPrimitiveTopology));
			DX11::Context->PSSetShader(material->GetPixelShader()->pixelShader, nullptr, 0);
			DX11::Context->PSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());
			DX11::Context->PSSetConstantBuffers(2, 1, myBuffers->myMaterialBuffer.GetAddress());


			DX11::Context->IASetInputLayout(material->GetVertexShader()->inputLayout.Get());
			DX11::Context->VSSetShader(material->GetVertexShader()->vertexShader, nullptr, 0);
			DX11::Context->VSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());
			DX11::Context->VSSetConstantBuffers(3, 1, myBuffers->mySkeletonBuffer.GetAddress());

			if (material->GetGeometryShader())
			{
				DX11::Context->GSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
				DX11::Context->GSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());
				DX11::Context->GSSetConstantBuffers(3, 1, myBuffers->myMaterialBuffer.GetAddress());
				DX11::Context->GSSetShader(material->GetGeometryShader()->geometryShader, nullptr, 0);
			}
			else
			{
				DX11::Context->GSSetShader(nullptr, nullptr, 0);
			}

			DX11::Context->DrawIndexed(meshData->myNumberOfIndices, 0, 0);
			myDrawCalls++;

			if (!renderedInstances.empty())
			{
				model->SetMaxInstances(renderedInstances.size());
				myBuffers->myObjectBufferData.isInstanced = true;
				myBuffers->myObjectBuffer.SetData(&myBuffers->myObjectBufferData);
				ID3D11Buffer* buffers[2] = { meshData->myVertexBuffer.Get(), model->GetInstanceBuffer().Get() };
				UINT stride[2] = { meshData->myStride, sizeof(Model::InstanceData) };
				UINT offset[2] = { 0, 0 };
				DX11::Context->IASetVertexBuffers(0, 2, buffers, stride, offset);
				DX11::Context->DrawIndexedInstanced(meshData->myNumberOfIndices, renderedInstances.size(), 0, 0, 0);
			}
		}
		SetRastertizerState(RastertizerState::RS_Default);
	}

	myBuffers->myObjectBufferData.isInstanced = false;
	SetBlendState(BlendState::BS_None);
	SetDepthStencilState(DepthStencilState::DSS_ReadWrite);
}

void ForwardRenderer::RenderParticles(const std::vector<ParticleSystem*>& aParticleSystemList)
{
	SetDepthStencilState(DepthStencilState::DSS_ReadOnly);

	for (auto& ps : aParticleSystemList)
	{
		myBuffers->myObjectBufferData.world = ps->GetTransform()->GetWorldTransformMatrix();
		myBuffers->myObjectBufferData.scale = ps->GetTransform()->worldScale();
		myBuffers->myObjectBufferData.isStatic = true;
		myBuffers->myObjectBufferData.id = ps->GetGameObject().GetObjectInstanceID();

		myBuffers->myObjectBuffer.SetData(&myBuffers->myObjectBufferData);

		const auto& objBufferAddress = myBuffers->myObjectBuffer.GetAddress();
		DX11::Context->VSSetConstantBuffers(1, 1, objBufferAddress);
		DX11::Context->GSSetConstantBuffers(1, 1, objBufferAddress);
		DX11::Context->PSSetConstantBuffers(1, 1, objBufferAddress);

		for (auto& emitter : ps->GetEmitters())
		{
			emitter.SetAsResource(myBuffers->myMaterialBuffer);
			SetBlendState(emitter.GetSharedData()->GetBlendState());
			emitter.Render();
		}
	}
	DX11::Context->GSSetShader(nullptr, nullptr, 0);
	SetBlendState(BlendState::BS_None);
	SetDepthStencilState(DepthStencilState::DSS_ReadWrite);
}
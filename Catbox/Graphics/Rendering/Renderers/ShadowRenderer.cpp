#include "stdafx.h"
#include "../GraphicsEngine.pch.h"
#include "ShadowRenderer.h"
#include "../DX11/DX11.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/MeshData.h"
#include "Assets/Model.h"
#include "Assets/Scene.h"
#include "../Buffers/Buffers.h"
#include "../DXDeviceStates.h"
#include "../DepthStencil.h"
#include "Components/3D/Light.h"


bool ShadowRenderer::Initialize(std::shared_ptr<Buffers> aBuffers)
{
	myBuffers = aBuffers;
	return true;
}

void ShadowRenderer::Render(const std::vector<ModelInstance*>& aModelList, std::vector<Light*>& aLightList)
{
	for (auto& light : aLightList)
	{
		auto& data = light->GetLightData();
		if (!light->GetShadowMap()) continue;
		//DX11::Context->PSSetShaderResources(7, 1, light->GetShadowMap()->GetShaderResourceView().GetAddressOf());
		DX11::Context->RSSetViewports(1, &light->GetShadowMap()->GetViewport());

		myBuffers->myFrameBufferData.View = data.view;
		myBuffers->myFrameBufferData.CamPosition = data.pos;
		myBuffers->myFrameBufferData.Projection = data.projection;
		myBuffers->myFrameBufferData.NearPlane = data.nearPlane;
		myBuffers->myFrameBufferData.FarPlane = data.farPlane;
		myBuffers->myFrameBuffer.SetData(&myBuffers->myFrameBufferData);

		DX11::Context->VSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
		DX11::Context->PSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
		DX11::Context->PSSetShader(nullptr, nullptr, 0);
		DX11::Context->GSSetShader(nullptr, nullptr, 0);


		DX11::Context->OMSetRenderTargets(0, nullptr, light->GetShadowMap()->GetDepthStencilView().Get());


		for (auto& model : aModelList)
		{
			if (!model->ShouldCastShadows() || !model->GetModel()) continue;
			std::vector<std::shared_ptr<MeshData>> modelParts = model->GetModel()->GetModelParts();
			int index = -1;
			const std::vector<Model::InstanceData>& renderedInstances = model->GetModel()->GetRenderedInstances();

			for (auto& meshData : modelParts)
			{
				++index;
				if (meshData == nullptr) continue;
				std::shared_ptr<Material> material = model->GetMaterial(index);
				if (!material) continue;
				if (!material->GetVertexShader()) continue;

				myBuffers->myObjectBufferData.world = model->GetTransform()->GetWorldTransformMatrix();
				myBuffers->myObjectBufferData.scale = model->GetTransform()->worldScale();
				myBuffers->myObjectBufferData.isInstanced = false;


				for (int row = 1; row < 4; row++)
				{
					for (int col = 1; col < 4; col++)
					{
						myBuffers->myObjectBufferData.world(row, col) *= 0.01f;
					}
				}

				if (!model->GetModel()->IsUnitsConverted())
				{
					for (int col = 1; col < 4; col++)
					{
						myBuffers->myObjectBufferData.world(4, col) *= 100;
					}
				}


#pragma region Skeleton
				std::shared_ptr<SkeletonInstance> skeleton = model->GetSkeleton();
				if (skeleton)
				{
					myBuffers->myObjectBufferData.isStatic = skeleton->sharedData == nullptr;
				}
				else myBuffers->myObjectBufferData.isStatic = true;

				myBuffers->myObjectBuffer.SetData(&myBuffers->myObjectBufferData);

				if (skeleton && skeleton->sharedData)
				{
					memcpy_s(myBuffers->mySkeletonBufferData.BoneData, sizeof(Matrix4x4<float>) * 128, skeleton->boneMatrices.data(), sizeof(Matrix4x4<float>) * 128);
					myBuffers->mySkeletonBuffer.SetData(&myBuffers->mySkeletonBufferData);
				}
#pragma endregion
				DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(BlendState::BS_AlphaBlend)].Get(), nullptr, 0xffffffff);

				DX11::Context->IASetVertexBuffers(0, 1, meshData->myVertexBuffer.GetAddressOf(), &meshData->myStride, &meshData->myOffset);
				DX11::Context->IASetIndexBuffer(meshData->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

				DX11::Context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(meshData->myPrimitiveTopology));
				DX11::Context->IASetInputLayout(material->GetVertexShader()->inputLayout.Get());

				DX11::Context->VSSetShader(material->GetVertexShader()->vertexShader, nullptr, 0);
				DX11::Context->VSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());
				DX11::Context->VSSetConstantBuffers(3, 1, myBuffers->mySkeletonBuffer.GetAddress());
				DX11::Context->PSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());

				DX11::Context->DrawIndexed(meshData->myNumberOfIndices, 0, 0);

				if (!renderedInstances.empty())
				{
					model->GetModel()->SetMaxInstances(renderedInstances.size());
					myBuffers->myObjectBufferData.isInstanced = true;
					myBuffers->myObjectBuffer.SetData(&myBuffers->myObjectBufferData);
					ID3D11Buffer* buffers[2] = { meshData->myVertexBuffer.Get(), model->GetModel()->GetInstanceBuffer().Get() };
					UINT stride[2] = { meshData->myStride, sizeof(Model::InstanceData) };
					UINT offset[2] = { 0, 0 };
					DX11::Context->IASetVertexBuffers(0, 2, buffers, stride, offset);
					DX11::Context->DrawIndexedInstanced(meshData->myNumberOfIndices, renderedInstances.size(), 0, 0, 0);
				}
			}
		}
		DX11::Context->OMSetRenderTargets(0, nullptr, nullptr);
		DX11::Context->PSSetShaderResources(7, 1, light->GetShadowMap()->GetShaderResourceView().GetAddressOf());
	}

	DX11::Context->RSSetViewports(1, &DX11::viewPort);
}
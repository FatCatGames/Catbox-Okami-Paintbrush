#include "stdafx.h"
#include "../GraphicsEngine.pch.h"
#include "DeferredRenderer.h"
#include "../DX11/DX11.h"
#include "Components/3D/Camera.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Material.h"
#include "Assets/MeshData.h"
#include "Assets/Model.h"
#include "Components/ParticleSystem.h"
#include "Assets/Scene.h"
#include "../DXDeviceStates.h"
#include "Editor\Windows\SceneWindow.h"

void DeferredRenderer::SetBlendState(const BlendState& aBlendState) const
{
	DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(aBlendState)].Get(), nullptr, 0xffffffff);
}
void DeferredRenderer::SetRastertizerState(const RastertizerState& aState) const
{
	DX11::Context->RSSetState(RastertizerStates::GetRastertizerStates()[static_cast<int>(aState)].Get());
}

void DeferredRenderer::SetDepthStencilState(const DepthStencilState& aDepthStencilState) const
{
	DX11::Context->OMSetDepthStencilState(DepthStencilStates::GetDepthStencilStates()[static_cast<int>(aDepthStencilState)].Get(), 0xffffffff);
}

void DeferredRenderer::OrderFrontToBack(std::vector<ModelInstance*>& aModelList)
{

	if (Engine::GetInstance()->GetMainCamera() != nullptr)
	{
		Vector3f aCameraPos = Engine::GetInstance()->GetMainCamera()->GetTransform()->worldPos();
		std::vector<float> distances(aModelList.size());
		for (int i = 0; i < aModelList.size(); i++)
		{
			for (int j = 0; j < aModelList.size() - i - 1; j++)
			{
				if (aModelList[j]->GetTransform()->GetCenter().Distance(aCameraPos) > aModelList[j + 1]->GetTransform()->GetCenter().Distance(aCameraPos))
				{
					auto& temp = aModelList[j];
					aModelList[j] = aModelList[j + 1];
					aModelList[j + 1] = temp;
					float distance = aModelList[j]->GetTransform()->worldPos().Distance(aCameraPos);
					distances[j] = aModelList[j + 1]->GetTransform()->worldPos().Distance(aCameraPos);
					distances[j + 1] = distance;
				}
			}
		}
	}
	else if (Editor::GetInstance()->GetEditorCamera().GetCamera() != nullptr) 
	{
		Vector3f aCameraPos = Editor::GetInstance()->GetEditorCamera().GetCamera()->GetTransform()->worldPos();
		std::vector<float> distances(aModelList.size());
		for (int i = 0; i < aModelList.size(); i++)
		{
			for (int j = 0; j < aModelList.size() - i - 1; j++)
			{
				if (aModelList[j]->GetTransform()->GetCenter().Distance(aCameraPos) > aModelList[j + 1]->GetTransform()->GetCenter().Distance(aCameraPos))
				{
					auto& temp = aModelList[j];
					aModelList[j] = aModelList[j + 1];
					aModelList[j + 1] = temp;
					float distance = aModelList[j]->GetTransform()->worldPos().Distance(aCameraPos);
					distances[j] = aModelList[j + 1]->GetTransform()->worldPos().Distance(aCameraPos);
					distances[j + 1] = distance;
				}
			}
		}
	}
}


bool DeferredRenderer::Initialize(std::shared_ptr<Buffers> aBuffers)
{
	myDeferredPS = CreateAsset<PixelShader>("Assets/Shaders/DeferredPS.sh");
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/DeferredPS.cso", ios::binary);
	string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myDeferredPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading gbuffer pixel shader failed!");

	myGBufferPS = CreateAsset<PixelShader>("Assets/Shaders/GBufferPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/GBufferPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myGBufferPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading deferred pixel shader failed!");

	myFullscreenVS = AssetRegistry::GetInstance()->GetAsset<VertexShader>("FullscreenVS");

	myBuffers = aBuffers;

	if (!myBuffers->myGBuffer.CreateTextures()) return false;

	int width = DX11::GetResolution().x;
	int height = DX11::GetResolution().y;


	return true;
}

void DeferredRenderer::GenerateGBuffer(std::vector<ModelInstance*>& aModelList)
{
	//Catbox::QuickSort<ModelInstance*>(aModelList, 0, aModelList.size());
	myBuffers->myGBuffer.ClearResource(0);
	myBuffers->myGBuffer.Clear();
	myBuffers->myGBuffer.SetAsTarget();
	SetDepthStencilState(DepthStencilState::DSS_ReadWrite);
	SetBlendState(BlendState::BS_None);
	//ID3D11Query* aQuery;
	//CD3D11_QUERY_DESC aDesc;
	//aDesc.Query = D3D11_QUERY_OCCLUSION;
	//auto Queery = DX11::Device->CreateQuery(&aDesc, &aQuery);
	//if (aQuery) 
	//{
	//	DX11::Context->Begin(aQuery);
	//	
	//	//DX11::Context->DrawIndexedInstanced(6, 1, 0, 0, 0);

	//	DX11::Context->End(aQuery);
	//}
	myDrawCalls = 0;
	for (auto& modelInstance : aModelList)
	{
		auto model = modelInstance->GetModel();
		if (!model) continue;
		std::vector<std::shared_ptr<MeshData>>& modelParts = model->GetModelParts();
		int index = -1;
		const std::vector<Model::InstanceData>& renderedInstances = model->GetRenderedInstances();

		for (auto& meshData : modelParts)
		{
			auto& object = modelInstance->GetGameObject();
			auto transform = object.GetTransform();

			myBuffers->myObjectBufferData.id = object.GetObjectInstanceID();
			myBuffers->myObjectBufferData.world = transform->GetWorldTransformMatrix();
			myBuffers->myObjectBufferData.scale = transform->worldScale();
			myBuffers->myObjectBufferData.aliveTime = object.GetAliveTime();
			myBuffers->myObjectBufferData.isInstanced = false;
			++index;
			if (meshData == nullptr) continue;
			std::shared_ptr<Material> material = modelInstance->GetMaterial(index);
			if (!material) continue;
			if (!material->GetVertexShader()) continue;
			myBuffers->myObjectBufferData.isTiling = material->IsTiling();

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
			if (!skeleton)
			{
				myBuffers->myObjectBufferData.isStatic = true;
			}
			else
			{
				if (skeleton->sharedData)
				{
					memcpy_s(myBuffers->mySkeletonBufferData.BoneData, sizeof(Matrix4x4<float>) * BONE_LIMIT, skeleton->boneMatrices.data(), sizeof(Matrix4x4<float>) * BONE_LIMIT);
					myBuffers->mySkeletonBuffer.SetData(&myBuffers->mySkeletonBufferData);
					myBuffers->myObjectBufferData.isStatic = false;
				}
				else myBuffers->myObjectBufferData.isStatic = true;
			}

			myBuffers->myObjectBuffer.SetData(&myBuffers->myObjectBufferData);

#pragma endregion

			material->SetAsResource(myBuffers->myMaterialBuffer);
			
			SetBlendState(material->GetBlendState());
			if(myBuffers->myFrameBufferData.DebugMode == 10) 
			{
				SetRastertizerState(RastertizerState::RS_WireFrame);
			}
			else 
			{
				SetRastertizerState(material->GetRastertizerState());
			}
			

			DX11::Context->IASetVertexBuffers(0, 1, meshData->myVertexBuffer.GetAddressOf(), &meshData->myStride, &meshData->myOffset);
			DX11::Context->IASetIndexBuffer(meshData->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			DX11::Context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(meshData->myPrimitiveTopology));
			DX11::Context->IASetInputLayout(material->GetVertexShader()->inputLayout.Get());

			DX11::Context->PSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
			DX11::Context->PSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());
			DX11::Context->PSSetConstantBuffers(2, 1, myBuffers->myMaterialBuffer.GetAddress());
			DX11::Context->PSSetShader(myGBufferPS->pixelShader, nullptr, 0);

			if (Engine::GetInstance()->GetActiveCamera()) 
			{
				DX11::Context->PSSetShaderResources(11, 1, Engine::GetInstance()->GetActiveCamera()->GetDepthStencil().GetShaderResourceView().GetAddressOf());
			}


			DX11::Context->VSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
			DX11::Context->VSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());
			DX11::Context->VSSetConstantBuffers(3, 1, myBuffers->mySkeletonBuffer.GetAddress());
			DX11::Context->VSSetShader(material->GetVertexShader()->vertexShader, nullptr, 0);

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
			++myDrawCalls;

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

			/*SetDepthStencilState(DepthStencilState::DSS_ReadOnly);
			DX11::Context->OMSetRenderTargets(1, DX11::SelectionBuffer.GetAddressOf(), nullptr);
			DX11::Context->PSSetShader(mySelectionPS->pixelShader, nullptr, 0);
			DX11::Context->DrawIndexed(meshData->myNumberOfIndices, 0, 0);
			myGBuffer.SetAsTarget();*/
		}
		SetRastertizerState(RastertizerState::RS_Default);
	}

	myBuffers->myGBuffer.ClearTarget();
	myBuffers->myGBuffer.SetAsResource(0);
}

void DeferredRenderer::Render(ComPtr<ID3D11RenderTargetView> aTarget)
{
	//myBuffers->myGBuffer.SetAsResource(0);
	SetDepthStencilState(DepthStencilState::DSS_Off);
	SetBlendState(BlendState::BS_None);
	//if we are using post processing we want to write the result to a separate texture before writing to viewport
	ID3D11RenderTargetView* rtvs[2];
	rtvs[0] = aTarget.Get();
	rtvs[1] = GraphicsEngine::GetInstance()->GetLightMap()->GetRenderTargetView().Get();

	DX11::Context->OMSetRenderTargets(2, &rtvs[0], nullptr);

	DX11::Context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	DX11::Context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11::Context->IASetInputLayout(nullptr);

	DX11::Context->GSSetShader(nullptr, nullptr, 0);
	DX11::Context->VSSetShader(myFullscreenVS->vertexShader, nullptr, 0);
	DX11::Context->PSSetShader(myDeferredPS->pixelShader, nullptr, 0);
	DX11::Context->PSSetConstantBuffers(4, 1, myBuffers->mySceneLightsBuffer.GetAddress());
	DX11::Context->PSSetShaderResources(0, 1, myBuffers->myGBuffer.GetTex(GBuffer::Albedo).GetShaderResourceView().GetAddressOf());
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
	DX11::Context->Draw(3, 0);

	ID3D11RenderTargetView* clearRtvs[2] = { nullptr, nullptr };
	DX11::Context->PSSetShaderResources(9, 1, &clearSrvs[0]);
	DX11::Context->OMSetRenderTargets(2, clearRtvs, nullptr);
}
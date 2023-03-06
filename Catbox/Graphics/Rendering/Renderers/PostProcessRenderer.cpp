#include "stdafx.h"
#include "Graphics/Rendering/GraphicsEngine.pch.h"
#include "PostProcessRenderer.h"
#include "../DX11/DX11.h"
#include "../DXDeviceStates.h"
#include "Assets/Shader.h"
#include "Components\3D\Camera.h"
#include "Components/3D/ModelInstance.h"
#include "Assets\Model.h"
#include "UserPreferences.h"


void PostProcessRenderer::SetBlendState(const BlendState& aBlendState) const
{
	DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(aBlendState)].Get(), nullptr, 0xffffffff);
}

void PostProcessRenderer::SetDepthStencilState(const DepthStencilState& aDepthStencilState) const
{
	DX11::Context->OMSetDepthStencilState(DepthStencilStates::GetDepthStencilStates()[static_cast<int>(aDepthStencilState)].Get(), 0xffffffff);
}

void PostProcessRenderer::Clear()
{
	ID3D11ShaderResourceView* clearSrvs[1] = { nullptr };
	ID3D11RenderTargetView* clearRtvs[1] = { nullptr };
	DX11::Context->PSSetShaderResources(0, 1, clearSrvs);
	DX11::Context->OMSetRenderTargets(1, clearRtvs, nullptr);
}


void PostProcessRenderer::RenderColorAdjustments(std::shared_ptr<PostProcessingVolume> aVolume, Texture* lastUsedTarget, Texture* targetToUse)
{
	DX11::Context->PSSetShaderResources(0, 1, lastUsedTarget->GetShaderResourceView().GetAddressOf());
	DX11::Context->OMSetRenderTargets(1, targetToUse->GetRenderTargetView().GetAddressOf(), nullptr);
	myColorAdjustmentBuffer.SetData(aVolume->GetPostProcess());
	DX11::Context->PSSetShader(myPostProcessingPS->pixelShader, nullptr, 0);
	DX11::Context->PSSetConstantBuffers(10, 1, myColorAdjustmentBuffer.GetAddress());
	DX11::Context->Draw(3, 0);
	Clear();
	std::swap(myResourceToUse, myTargetToUse);
}

void PostProcessRenderer::RenderVignette(std::shared_ptr<PostProcessingVolume> aVolume, Texture* lastUsedTarget, Texture* targetToUse)
{
	auto profile = aVolume->GetProfile();
	if (!profile) return;

	myVignetteBuffer.SetData(aVolume->GetVignetteSettings());
	DX11::Context->PSSetConstantBuffers(10, 1, myVignetteBuffer.GetAddress());

	DX11::Context->VSSetShader(myFullscreenVS->vertexShader, nullptr, 0);
	DX11::Context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	DX11::Context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11::Context->IASetInputLayout(nullptr);

	DX11::Context->PSSetShader(myVignettePS->pixelShader, nullptr, 0);
	DX11::Context->PSSetShaderResources(0, 1, lastUsedTarget->GetShaderResourceView().GetAddressOf());
	DX11::Context->OMSetRenderTargets(1, targetToUse->GetRenderTargetView().GetAddressOf(), nullptr);
	DX11::Context->Draw(3, 0);
	Clear();
	std::swap(myResourceToUse, myTargetToUse);
}

void PostProcessRenderer::RenderAmbientOcclusion(Camera& aCamera, ID3D11ShaderResourceView* const* aSource)
{
	float red[4] = { 1, 0, 0, 1 };
	DX11::Context->ClearRenderTargetView(mySSAOTarget.GetRenderTargetView().Get(), red);
	mySSAOTarget.SetAsResource(8);

	auto volume = aCamera.GetPostProcessingVolume();
	if (!volume || !volume->IsEnabled()) return;
	auto profile = volume->GetProfile();
	if (!profile || !profile->IsEffectEnabled(PostProcessEffect::AmbientOcclusion)) return;

	myNoiseTexture->SetAsResource(8);
	myBuffers->myGBuffer.GetTex(GBuffer::Normal).SetAsResource(1);
	myBuffers->myGBuffer.GetTex(GBuffer::Position).SetAsResource(4);
	mySSAOBuffer.SetData(&profile->GetSSAOSettings());
	DX11::Context->PSSetShader(myPostProcessingPS->pixelShader, nullptr, 0);
	DX11::Context->PSSetConstantBuffers(10, 1, mySSAOBuffer.GetAddress());
	GraphicsEngine::RunFullScreenShader(aSource, mySSAOTarget.GetRenderTargetView().GetAddressOf(), mySSAOPS);
	mySSAOTarget.SetAsResource(8);
	Clear();
}

void PostProcessRenderer::RenderBloom(Camera& aCamera, ID3D11ShaderResourceView* const* aSource, ID3D11RenderTargetView* const* aTarget)
{
	auto volume = aCamera.GetPostProcessingVolume();
	if (!volume) return;
	auto profile = volume->GetProfile();
	if (!profile) return;

	float clearColor[4] = { 0, 0, 0, 0 };
	DX11::Context->ClearRenderTargetView(myIntermediateTargetA.GetRenderTargetView().Get(), clearColor);
	DX11::Context->ClearRenderTargetView(myIntermediateTargetB.GetRenderTargetView().Get(), clearColor);

	DX11::Context->VSSetShader(myFullscreenVS->vertexShader, nullptr, 0);
	DX11::Context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	DX11::Context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11::Context->IASetInputLayout(nullptr);

	DX11::Context->PSSetShader(myCopyPS->pixelShader, nullptr, 0);
	DX11::Context->PSSetShaderResources(0, 1, aSource);
	DX11::Context->OMSetRenderTargets(1, myIntermediateTargetA.GetRenderTargetView().GetAddressOf(), nullptr);
	DX11::Context->Draw(3, 0);
	Clear();

	Texture* lastUsedTarget = &myIntermediateTargetA;
	Texture* targetToUse = &myIntermediateTargetB;


	if (profile->IsEffectEnabled(PostProcessEffect::Bloom))
	{
		myBloomBuffer.SetData(volume->GetBloomSettings());
		DX11::Context->PSSetConstantBuffers(10, 1, myBloomBuffer.GetAddress());

		DX11::Context->PSSetShader(myLuminancePS->pixelShader, nullptr, 0);
		DX11::Context->PSSetShaderResources(0, 1, lastUsedTarget->GetShaderResourceView().GetAddressOf());
		DX11::Context->OMSetRenderTargets(1, targetToUse->GetRenderTargetView().GetAddressOf(), nullptr);
		DX11::Context->Draw(3, 0);
		Clear();

		std::swap(targetToUse, lastUsedTarget);

		DX11::Context->RSSetViewports(1, &DX11::viewPortHalfSize);
		DX11::Context->PSSetShader(myCopyPS->pixelShader, nullptr, 0);
		DX11::Context->PSSetShaderResources(0, 1, lastUsedTarget->GetShaderResourceView().GetAddressOf());
		DX11::Context->OMSetRenderTargets(1, myHalfSizeTarget.GetRenderTargetView().GetAddressOf(), nullptr);
		DX11::Context->Draw(3, 0);
		Clear();

		DX11::Context->RSSetViewports(1, &DX11::viewPortQuarterSize);
		myHalfSizeTarget.SetAsResource(0);
		DX11::Context->OMSetRenderTargets(1, myQuarterSizeTarget.GetRenderTargetView().GetAddressOf(), nullptr);
		DX11::Context->Draw(3, 0);
		Clear();

		DX11::Context->PSSetShader(myGaussianBlurPS->pixelShader, nullptr, 0);
		myQuarterSizeTarget.SetAsResource(0);
		DX11::Context->OMSetRenderTargets(1, myBlurTargetA.GetRenderTargetView().GetAddressOf(), nullptr);
		DX11::Context->Draw(3, 0);
		Clear();

		myBlurTargetA.SetAsResource(0);
		DX11::Context->OMSetRenderTargets(1, myBlurTargetB.GetRenderTargetView().GetAddressOf(), nullptr);
		DX11::Context->Draw(3, 0);
		Clear();

		DX11::Context->PSSetShader(myCopyPS->pixelShader, nullptr, 0);
		myBlurTargetB.SetAsResource(0);
		DX11::Context->OMSetRenderTargets(1, myQuarterSizeTarget.GetRenderTargetView().GetAddressOf(), nullptr);
		DX11::Context->Draw(3, 0);
		Clear();

		DX11::Context->RSSetViewports(1, &DX11::viewPortHalfSize);
		myQuarterSizeTarget.SetAsResource(0);
		DX11::Context->OMSetRenderTargets(1, myHalfSizeTarget.GetRenderTargetView().GetAddressOf(), nullptr);
		DX11::Context->Draw(3, 0);
		Clear();

		DX11::Context->RSSetViewports(1, &DX11::viewPort);
		DX11::Context->PSSetShader(myBloomPS->pixelShader, nullptr, 0);
		DX11::Context->PSSetShaderResources(0, 1, aSource);
		myHalfSizeTarget.SetAsResource(1);
		DX11::Context->OMSetRenderTargets(1, targetToUse->GetRenderTargetView().GetAddressOf(), nullptr);
		DX11::Context->Draw(3, 0);
		Clear();
		std::swap(targetToUse, lastUsedTarget);

		DX11::Context->ClearRenderTargetView(myBlurTargetA.GetRenderTargetView().Get(), clearColor);
		DX11::Context->ClearRenderTargetView(myBlurTargetB.GetRenderTargetView().Get(), clearColor);
		DX11::Context->ClearRenderTargetView(myHalfSizeTarget.GetRenderTargetView().Get(), clearColor);
		DX11::Context->ClearRenderTargetView(myQuarterSizeTarget.GetRenderTargetView().Get(), clearColor);
	}

	DX11::Context->PSSetShader(myCopyPS->pixelShader, nullptr, 0);
	DX11::Context->PSSetShaderResources(0, 1, lastUsedTarget->GetShaderResourceView().GetAddressOf());
	DX11::Context->OMSetRenderTargets(1, aTarget, nullptr);
	DX11::Context->Draw(3, 0);
	Clear();
}


void PostProcessRenderer::RenderFog(Camera& aCamera, Texture* aSource, Texture* aTarget)
{
	auto volume = aCamera.GetPostProcessingVolume();
	if (!volume) return;
	auto profile = volume->GetProfile();
	if (!profile) return;

	myFogBuffer.SetData(&profile->GetFogSettings());
	DX11::Context->PSSetConstantBuffers(10, 1, myFogBuffer.GetAddress());


	DX11::Context->VSSetShader(myFullscreenVS->vertexShader, nullptr, 0);
	DX11::Context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	DX11::Context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11::Context->IASetInputLayout(nullptr);

	DX11::Context->PSSetShader(myFogPS->pixelShader, nullptr, 0);
	ID3D11ShaderResourceView* srvs[3] = { 
		aSource->GetShaderResourceView().Get(), 
		aCamera.GetDepthStencil().GetShaderResourceView().Get(), 
		GraphicsEngine::GetInstance()->GetLightMap()->GetShaderResourceView().Get()
	};

	DX11::Context->PSSetShaderResources(0, 3, &srvs[0]);
	DX11::Context->OMSetRenderTargets(1, aTarget->GetRenderTargetView().GetAddressOf(), nullptr);
	DX11::Context->Draw(3, 0);
	Clear();
	std::swap(myResourceToUse, myTargetToUse);
}


void PostProcessRenderer::RenderPostProcessing(Camera& aCamera, Texture* aSource, Texture* aTarget)
{
	auto volume = aCamera.GetPostProcessingVolume();
	if (!volume) return;
	auto profile = volume->GetProfile();
	if (!profile) return;

	myResourceToUse = aSource;
	myTargetToUse = aTarget;
	if (profile->IsEffectEnabled(PostProcessEffect::Fog)) RenderFog(aCamera, myResourceToUse, myTargetToUse);
	if (profile->IsEffectEnabled(PostProcessEffect::ColorAdjustment)) RenderColorAdjustments(volume, myResourceToUse, myTargetToUse);
	if (profile->IsEffectEnabled(PostProcessEffect::Vignette)) RenderVignette(volume, myResourceToUse, myTargetToUse);


	DX11::Context->PSSetShader(myCopyPS->pixelShader, nullptr, 0);
	DX11::Context->PSSetShaderResources(0, 1, myResourceToUse->GetShaderResourceView().GetAddressOf());
	DX11::Context->OMSetRenderTargets(1, aTarget->GetRenderTargetView().GetAddressOf(), nullptr);
	DX11::Context->Draw(3, 0);
	Clear();

	if (profile.get()->GetShaderReload())
	{
		ReloadPostProcessShaders();
		profile.get()->SetShaderReload(false);
	}
}

Texture* PostProcessRenderer::CreateMask(const std::vector<ModelInstance*>& aModelList)
{
	std::shared_ptr<Material> material = AssetRegistry::GetInstance()->GetAsset<Material>("DefaultMat");

	float clearColor[4] = { 0, 0, 0, 0 };
	DX11::Context->ClearRenderTargetView(myIntermediateTargetA.GetRenderTargetView().Get(), clearColor);

	for (auto& model : aModelList)
	{
		if (!model->GetModel()) continue;

		std::vector<std::shared_ptr<MeshData>> modelParts = model->GetModel()->GetModelParts();
		int index = -1;
		for (auto& meshData : modelParts)
		{
			++index;
			if (meshData == nullptr) continue;
			material = model->GetMaterial(index);
			if (!material) continue;
			if (!material->GetVertexShader()) continue;

			myBuffers->myObjectBufferData.world = model->GetTransform()->GetWorldTransformMatrix();
			myBuffers->myObjectBufferData.scale = model->GetTransform()->worldScale();
			myBuffers->myObjectBufferData.id = model->GetGameObject().GetObjectInstanceID();
			if (model->GetModel()->IsUnitsConverted())
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

			auto dsv = Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetDepthStencil().GetDepthStencilView().Get();
			DX11::Context->OMSetRenderTargets(1, myIntermediateTargetA.GetRenderTargetView().GetAddressOf(), dsv);

			DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(BlendState::BS_None)].Get(), nullptr, 0xffffffff);

			DX11::Context->IASetVertexBuffers(0, 1, meshData->myVertexBuffer.GetAddressOf(), &meshData->myStride, &meshData->myOffset);
			DX11::Context->IASetIndexBuffer(meshData->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			DX11::Context->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(meshData->myPrimitiveTopology));
			DX11::Context->IASetInputLayout(material->GetVertexShader()->inputLayout.Get());

			DX11::Context->VSSetShader(material->GetVertexShader()->vertexShader, nullptr, 0);
			DX11::Context->VSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());
			DX11::Context->VSSetConstantBuffers(3, 1, myBuffers->mySkeletonBuffer.GetAddress());

			DX11::Context->PSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());
			DX11::Context->PSSetShader(myOutlineMaskPS->pixelShader, nullptr, 0);

			DX11::Context->PSSetConstantBuffers(1, 1, myBuffers->myObjectBuffer.GetAddress());
			DX11::Context->DrawIndexed(meshData->myNumberOfIndices, 0, 0);
		}
	}

	Clear();
	return &myIntermediateTargetA;
}

void PostProcessRenderer::RenderOutlines(ID3D11ShaderResourceView** aMask, ID3D11ShaderResourceView** aScreenTex, ID3D11RenderTargetView** aTarget, bool aDefault)
{
	myEditorOutlineData.color = aDefault ? Editor::GetInstance()->GetUserPrefs().GetOutlineColor() : Color::Red();
	myOutlineBuffer.SetData(&myEditorOutlineData);
	DX11::Context->PSSetConstantBuffers(10, 1, myOutlineBuffer.GetAddress());

	DX11::Context->PSSetShaderResources(1, 1, aMask);
	GraphicsEngine::RunFullScreenShader(aScreenTex, aTarget, myOutlinePS);
}


void PostProcessRenderer::ReloadPostProcessShaders()
{
	std::filesystem::path defaultPathReg = "..\\Catbox\\Shaders\\PostProcess";

	ComPtr<ID3DBlob> shaderBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr;

	for (auto shader : myPostProcessShaders)
	{
		if (shader != nullptr)
		{
			if (!shader->GetName().empty())
			{
				std::filesystem::path pixelShaderPath;
				pixelShaderPath = LoadFolder(defaultPathReg, shader->GetName());

				if (!pixelShaderPath.empty())
				{
					pixelShaderPath += "\\";
					pixelShaderPath += shader->GetName();
					pixelShaderPath += ".hlsl";

					hr = D3DCompileFromFile(pixelShaderPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0",
						NULL, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());

					if (hr == S_OK)
					{
						DX11::Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shader->pixelShader);
					}
					else
					{
						auto error = GetLastError();
						printerror(shader->GetName() + " is scuffed");

					}
				}
			}
		}

	}

	if (shaderBlob != nullptr)
	{
		shaderBlob->Release();
	}
	if (errorBlob != nullptr)
	{
		errorBlob->Release();
	}
	
}

std::filesystem::path PostProcessRenderer::LoadFolder(const std::filesystem::path& aDirectory, const std::filesystem::path& aName)
{

	for (auto& directoryEntry : std::filesystem::directory_iterator(aDirectory))
	{
		if (directoryEntry.is_directory())
		{
			std::filesystem::path foundPath = LoadFolder(directoryEntry.path(), aName);
			if (!foundPath.empty())
			{
				return foundPath;
			}
		}
		else
		{

			if (directoryEntry.path().string().find(aName.string()) != std::string::npos)
			{
				return aDirectory;
			}
		}
	}
	return "";
}

bool PostProcessRenderer::Initialize(std::shared_ptr<Buffers> aBuffers)
{
	myPostProcessingPS = CreateAsset<PixelShader>("Assets/Shaders/PostProcessPS.sh");
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/PostProcessPS.cso", ios::binary);
	string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPostProcessingPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading post processing pixel shader failed!");

	myLuminancePS = CreateAsset<PixelShader>("Assets/Shaders/LuminancePS.sh");
	psFile.open("Resources/BuiltIn/Shaders/LuminancePS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myLuminancePS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading luminance shader failed!");

	myPostProcessShaders.push_back(myLuminancePS);

	myGaussianBlurPS = CreateAsset<PixelShader>("Assets/Shaders/GaussianBlurPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/GaussianBlurPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myGaussianBlurPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading gaussian blur shader failed!");

	myPostProcessShaders.push_back(myGaussianBlurPS);

	myBloomPS = CreateAsset<PixelShader>("Assets/Shaders/BloomPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/BloomPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myBloomPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading bloom shader failed!");

	myPostProcessShaders.push_back(myBloomPS);

	myVignettePS = CreateAsset<PixelShader>("Assets/Shaders/VignettePS.sh");
	psFile.open("Resources/BuiltIn/Shaders/VignettePS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myVignettePS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading vignette shader failed!");

	myPostProcessShaders.push_back(myVignettePS);

	mySSAOPS = CreateAsset<PixelShader>("Assets/Shaders/SSAOPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/SSAOPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &mySSAOPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading SSAO shader failed!");

	myPostProcessShaders.push_back(mySSAOPS);
	 
	myFogPS = CreateAsset<PixelShader>("Assets/Shaders/FogPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/FogPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myFogPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading fog shader failed!");

	myPostProcessShaders.push_back(myFogPS);

	myFullscreenVS = AssetRegistry::GetInstance()->GetAsset<VertexShader>("FullscreenVS");
	myNoiseTexture = AssetRegistry::GetInstance()->GetAsset<Texture>("BlueNoise");
	myOutlinePS = AssetRegistry::GetInstance()->GetAsset<PixelShader>("OutlinePS");
	myOutlineMaskPS = AssetRegistry::GetInstance()->GetAsset<PixelShader>("OutlineMaskPS");

	myBuffers = aBuffers;
	if (!myColorAdjustmentBuffer.Initialize()) return false;
	if (!myBloomBuffer.Initialize()) return false;
	if (!myVignetteBuffer.Initialize()) return false;
	if (!myOutlineBuffer.Initialize()) return false;
	if (!mySSAOBuffer.Initialize()) return false;

	myIntermediateTargetA.CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT);
	myIntermediateTargetA.CreateRenderTargetView();

	myIntermediateTargetB.CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT);
	myIntermediateTargetB.CreateRenderTargetView();

	myHalfSizeTarget.CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 2);
	myHalfSizeTarget.CreateRenderTargetView();

	myQuarterSizeTarget.CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 4);
	myQuarterSizeTarget.CreateRenderTargetView();

	myBlurTargetA.CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 4);
	myBlurTargetA.CreateRenderTargetView();

	myBlurTargetB.CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 4);
	myBlurTargetB.CreateRenderTargetView();

	mySSAOTarget.CreateScreenSizeTexture(DXGI_FORMAT_R32_FLOAT);
	mySSAOTarget.CreateRenderTargetView();

	myCopyPS = AssetRegistry::GetInstance()->GetAsset<PixelShader>("CopyTexturePS");


	return true;
}
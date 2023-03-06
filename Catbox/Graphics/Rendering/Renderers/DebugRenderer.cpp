#include "stdafx.h"
#include "../GraphicsEngine.pch.h"
#include "DebugRenderer.h"
#include "../DX11/DX11.h"
#include "../Buffers/Buffers.h"
#include "Assets\Shader.h"
#include "Navigation\NavMesh.h"
#include "Assets\Material.h"


bool DebugRenderer::Initialize(std::shared_ptr<Buffers> aBuffers)
{
	myBuffers = aBuffers;

	auto vertices = std::vector<Vertex>();
	for (float x = -1; x < 2; x += 2)
	{
		for (float y = -1; y < 2; y += 2)
		{
			for (float z = -1; z < 2; z += 2)
			{
				Vertex v;
				v.position = Vector4f(x / 2.f, y / 2.f, z / 2.f, 1);
				vertices.push_back(v);
			}
		}
	}
	myCubeMesh.SetVertices(vertices);

	std::vector<unsigned int> mdlIndices =
	{
		2,0,1,
		1,3,2,

		5,1,0,
		0,4,5,

		6,4,0,
		0,2,6,

		6,2,3,
		3,7,6,

		7,5,4,
		4,6,7,

		3,1,5,
		5,7,3
	};
	myCubeMesh.SetIndices(mdlIndices);

	vertices.resize(2);
	mdlIndices = { 0, 1 };
	myLineMesh.SetVertices(vertices);
	myLineMesh.SetIndices(mdlIndices);

	myPixelShader = CreateAsset<PixelShader>("Assets/Shaders/DebugPS.sh");
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/DebugPS.cso", ios::binary);
	string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading debug pixel shader failed!");

	myVertexShader = CreateAsset<VertexShader>("Assets/Shaders/DebugVS.sh");
	ifstream vsFile;
	vsFile.open("Resources/BuiltIn/Shaders/DebugVS.cso", ios::binary);
	string vsData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader->vertexShader);
	assert(!FAILED(result) && "Loading debug vertex shader failed!");
	vsFile.close();
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//Instance data
		{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	result = DX11::Device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), myVertexShader->inputLayout.GetAddressOf());
	assert(!FAILED(result) && "Creating debug vertex shader input layout failed!");

	myDebugMat = AssetRegistry::GetInstance()->GetAsset<Material>("DefaultMat")->MakeInstanceOfMaterial();
	return true;
}


void DebugRenderer::SetBlendState(const BlendState& aBlendState)
{
	DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(aBlendState)].Get(), nullptr, 0xffffffff);
}

void DebugRenderer::SetDepthStencilState(const DepthStencilState& aDepthStencilState)
{
	DX11::Context->OMSetDepthStencilState(DepthStencilStates::GetDepthStencilStates()[static_cast<int>(aDepthStencilState)].Get(), 0xffffffff);
}


void DebugRenderer::Render(const std::vector<DebugLine>& aLineList, const std::vector<DebugCube>& aBoxList, ComPtr<ID3D11RenderTargetView> aTarget)
{
	if (aLineList.empty() && aBoxList.empty()) return;

	DX11::Context->VSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
	DX11::Context->PSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
	DX11::Context->PSSetConstantBuffers(2, 1, myBuffers->myMaterialBuffer.GetAddress());

	auto dsv = Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetDepthStencil().GetDepthStencilView().Get();
	DX11::Context->OMSetRenderTargets(1, aTarget.GetAddressOf(), dsv);
	
	DX11::Context->IASetInputLayout(myVertexShader->inputLayout.Get());
	DX11::Context->VSSetShader(myVertexShader->vertexShader, nullptr, 0);
	DX11::Context->PSSetShader(myPixelShader->pixelShader, nullptr, 0);
	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	Matrix4x4<float> identity;
	myBuffers->myObjectBufferData.world = identity;
	myBuffers->myObjectBuffer.SetData(&myBuffers->myObjectBufferData);

	UINT stride[2] = { myLineMesh.myStride, sizeof(Model::InstanceData) };
	UINT offset[2] = { 0, 0 };


	for (auto& line : aLineList)
	{
		myLineMesh.myVertices[0].position = Vector4f(line.from.x, line.from.y, line.from.z, 1);
		myLineMesh.myVertices[1].position = Vector4f(line.to.x, line.to.y, line.to.z, 1);
		myLineMesh.SetVertices(myLineMesh.myVertices);

		myDebugMat->SetColor(line.color);
		myDebugMat->SetAsResource(myBuffers->myMaterialBuffer);

		DX11::Context->IASetVertexBuffers(0, 1, myLineMesh.myVertexBuffer.GetAddressOf(), &myLineMesh.myStride, &myLineMesh.myOffset);
		DX11::Context->IASetIndexBuffer(myLineMesh.myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11::Context->DrawIndexed(myLineMesh.myNumberOfIndices, 0, 0);
	}

	//for (size_t i = 0; i < aLineList.size(); i++)
	//{
	//	auto& line = aLineList[i];
	//	myLineMesh.myVertices[0].position = Vector4f(line.from.x, line.from.y, line.from.z, 1);
	//	myLineMesh.myVertices[1].position = Vector4f(line.to.x, line.to.y, line.to.z, 1);
	//	myLineMesh.SetVertices(myLineMesh.myVertices);

	//	myDebugMat->SetColor(line.color);
	//	myDebugMat->SetAsResource(myBuffers->myMaterialBuffer);

	//	ID3D11Buffer* buffers[2] = { myLineMesh.myVertexBuffer.Get(), myLineModel.GetInstanceBuffer().Get()};
	//	DX11::Context->IASetVertexBuffers(0, 2, buffers, stride, offset);


	//	//DX11::Context->IASetIndexBuffer(myLine.myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//	DX11::Context->DrawIndexedInstanced(myLineMesh.myNumberOfIndices, aLineList.size(), 0, 0, 0);
	//}

	for (auto& box : aBoxList)
	{
		myBuffers->myObjectBufferData.world = box.transform;
		myBuffers->myObjectBuffer.SetData(&myBuffers->myObjectBufferData);

		myDebugMat->SetColor(box.color);
		myDebugMat->SetAsResource(myBuffers->myMaterialBuffer);

		DX11::Context->IASetVertexBuffers(0, 1, myCubeMesh.myVertexBuffer.GetAddressOf(), &myCubeMesh.myStride, &myCubeMesh.myOffset);
		DX11::Context->IASetIndexBuffer(myCubeMesh.myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11::Context->DrawIndexed(myCubeMesh.myNumberOfIndices, 0, 0);
	}
}

void DebugRenderer::Render(const std::vector<NavMesh*>& aNavMeshList, ComPtr<ID3D11RenderTargetView> aTarget)
{
	if (aNavMeshList.empty()) return;
	DX11::Context->VSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
	DX11::Context->PSSetConstantBuffers(0, 1, myBuffers->myFrameBuffer.GetAddress());
	DX11::Context->PSSetConstantBuffers(2, 1, myBuffers->myMaterialBuffer.GetAddress());

	auto dsv = Engine::GetInstance()->GetGraphicsEngine()->GetMainCamera()->GetDepthStencil().GetDepthStencilView().Get();
	DX11::Context->OMSetRenderTargets(1, aTarget.GetAddressOf(), dsv);

	DX11::Context->IASetInputLayout(myVertexShader->inputLayout.Get());
	DX11::Context->VSSetShader(myVertexShader->vertexShader, nullptr, 0);
	DX11::Context->PSSetShader(myPixelShader->pixelShader, nullptr, 0);
	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Matrix4x4<float> identity;
	myBuffers->myObjectBufferData.world = identity;
	myBuffers->myObjectBuffer.SetData(&myBuffers->myObjectBufferData);
	DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(BlendState::BS_AlphaBlend)].Get(), nullptr, 0xffffffff);

	for (auto& navMesh : aNavMeshList)
	{
		auto data = navMesh->GetData();
		if (!data) continue;
		auto mesh = data->meshData;
		if (!mesh) continue;
		for (int i = 0; i < mesh->myIndices.size() - 3; i += 3)
		{
			DebugDrawer::DrawLine(mesh->myVertices[mesh->myIndices[i]].position.GetVector3(), mesh->myVertices[mesh->myIndices[i + 1]].position.GetVector3());
			DebugDrawer::DrawLine(mesh->myVertices[mesh->myIndices[i + 1]].position.GetVector3(), mesh->myVertices[mesh->myIndices[i + 2]].position.GetVector3());
			DebugDrawer::DrawLine(mesh->myVertices[mesh->myIndices[i + 2]].position.GetVector3(), mesh->myVertices[mesh->myIndices[i]].position.GetVector3());
		}

		myDebugMat->SetColor({ 0.2f, 0.5f, 1.f, 0.4f });
		myDebugMat->SetAsResource(myBuffers->myMaterialBuffer);
		DX11::Context->IASetVertexBuffers(0, 1, mesh->myVertexBuffer.GetAddressOf(), &mesh->myStride, &mesh->myOffset);
		DX11::Context->IASetIndexBuffer(mesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11::Context->DrawIndexed(mesh->myNumberOfIndices, 0, 0);
	}
}
#include "stdafx.h"
#include "Model.h"


Model::Model(std::vector<std::shared_ptr<MeshData>> aMeshDataList)
{
	myModelParts = aMeshDataList;
}

Model::Model(std::shared_ptr<MeshData> aMeshdata)
{
	myModelParts.clear();
	myModelParts.push_back(aMeshdata);
}

const float Model::GetBoundsRadius()
{
	return myBoundsRadius;
}

std::shared_ptr<MeshData> Model::GetModelPart(int anIndex)
{
	return myModelParts[anIndex];
}

std::vector<std::shared_ptr<MeshData>>& Model::GetModelParts()
{
	return myModelParts;
}

void Model::SaveAsset(const char* /*aPath*/)
{
	print("Model saving not implemented.");
}
void Model::CreateBoundingUnits()
{
	float front = -FLT_MAX;
	float back = FLT_MAX;
	float top = -FLT_MAX;
	float bottom = FLT_MAX;
	float right = -FLT_MAX;
	float left = FLT_MAX;

	for (int i = 0; i < myModelParts.size(); i++)
	{
		if (myModelParts[i]->myIsAnimated)
		{
			for (int y = 0; y < myModelParts[i]->myAnimatedVertices.size(); y++)
			{
				Vector4f tempPos = myModelParts[i]->myAnimatedVertices[y].position;
				if (front < tempPos.z)
				{
					front = tempPos.z;
				}
				if (back > tempPos.z)
				{
					back = tempPos.z;
				}
				if (top < tempPos.y)
				{
					top = tempPos.y;
				}
				if (bottom > tempPos.y)
				{
					bottom = tempPos.y;
				}
				if (right < tempPos.x)
				{
					right = tempPos.x;
				}
				if (left > tempPos.x)
				{
					left = tempPos.x;
				}
			}
		}
		else
		{
			for (int y = 0; y < myModelParts[i]->myVertices.size(); y++)
			{
				Vector4f tempPos = myModelParts[i]->myVertices[y].position;
				if (front < tempPos.z)
				{
					front = tempPos.z;
				}
				if (back > tempPos.z)
				{
					back = tempPos.z;
				}
				if (top < tempPos.y)
				{
					top = tempPos.y;
				}
				if (bottom > tempPos.y)
				{
					bottom = tempPos.y;
				}
				if (right < tempPos.x)
				{
					right = tempPos.x;
				}
				if (left > tempPos.x)
				{
					left = tempPos.x;
				}
			}
		}
	}
	myBoundingUnits[0] = front / 100.0f;
	myBoundingUnits[1] = back / 100.0f;
	myBoundingUnits[2] = top / 100.0f;
	myBoundingUnits[3] = bottom / 100.0f;
	myBoundingUnits[4] = right / 100.0f;
	myBoundingUnits[5] = left / 100.0f;
	myBoundsRadius = std::max<float>(front / 100.0f - (-back / 100.0f), std::max<float>(top / 100.0f - (-bottom / 100.0f), (right / 100.0f - (-left / 100.0f)))) * 0.5f;

	myPivot.x = myBoundingUnits[4] + myBoundingUnits[5];
	myPivot.y = myBoundingUnits[2] + myBoundingUnits[3];
	myPivot.z = myBoundingUnits[0] + myBoundingUnits[1];

	myCenter = Vector3f(myBoundingUnits[4] - (-myBoundingUnits[5]), myBoundingUnits[2] - (-myBoundingUnits[3]), myBoundingUnits[0] - (-myBoundingUnits[1])) * 0.5f;

}

void Model::RenderInstance(Transform* aTransform)
{
	myInstances.emplace_back();
	myInstances.back().transform = aTransform->GetWorldTransformMatrix();
	if (myConvertUnits)
	{
		for (int row = 1; row < 4; row++)
		{
			for (int col = 1; col < 4; col++)
			{
				myInstances.back().transform(row, col) *= 0.01f;
			}
		}
	}
}

void Model::SetMaxInstances(int aMaxCount)
{
	myInstances.resize(aMaxCount);
	if (aMaxCount == 0) return;
	HRESULT result = E_FAIL;
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(InstanceData)) * aMaxCount;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = myInstances.data();

	result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, myInstanceBuffer.GetAddressOf());
	assert(!FAILED(result) && "Failed to set instanced vertices");
}
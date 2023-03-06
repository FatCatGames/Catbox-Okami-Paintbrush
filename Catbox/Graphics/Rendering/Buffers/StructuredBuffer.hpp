#pragma once
#include "stdafx.h"
#include <wrl.h>
#include <d3d11.h>
#include "..\DX11\DX11.h"
#include <cassert>

namespace Catbox {
	template <class T>
	class StructuredBuffer
	{
		public:
			Microsoft::WRL::ComPtr<ID3D11Buffer> myBuffer;
		private:
			D3D11_BUFFER_DESC myBufferDescription;
			D3D11_MAPPED_SUBRESOURCE myBufferData;
			D3D11_SUBRESOURCE_DATA subResourceData2;
			ComPtr<ID3D11ShaderResourceView> mySrv;
			D3D11_SHADER_RESOURCE_VIEW_DESC mySrvDesc;

			const T* data;
		public:
			bool Initialize() {
				myBufferDescription = { 0 };
				myBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
				myBufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				myBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				myBufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				myBufferDescription.StructureByteStride = sizeof(T);
				myBufferDescription.ByteWidth = sizeof(T) * 128;
				
				assert(myBufferDescription.ByteWidth % 16 == 0 && "Buffer byte width not dividable by 16");

				
				HRESULT result = DX11::Device->CreateBuffer(&myBufferDescription, NULL, myBuffer.GetAddressOf());
				if (FAILED(result)) return false;

				
				mySrvDesc.Format = DXGI_FORMAT_UNKNOWN;
				mySrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				mySrvDesc.Buffer.ElementOffset = 0;
				mySrvDesc.Buffer.FirstElement = 0;
				mySrvDesc.Buffer.ElementWidth = myBufferDescription.StructureByteStride;
				mySrvDesc.Buffer.NumElements = 128;


				result = DX11::Device->CreateShaderResourceView(myBuffer.Get(), &mySrvDesc, &mySrv);

				if (FAILED(result)) return false;

				return true;
			}
			bool SetData(const T* aData)
			{
				this->data = aData;

				ZeroMemory(&myBufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
				HRESULT result = DX11::Device->CreateShaderResourceView(myBuffer.Get(), &mySrvDesc, &mySrv);
				if (FAILED(result)) return false;
				result = DX11::Context->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &myBufferData);
				if (FAILED(result)) return false;
				memcpy_s(myBufferData.pData, myBufferDescription.ByteWidth, aData, myBufferDescription.ByteWidth);

				DX11::Context->Unmap(myBuffer.Get(), 0);
				return true;
			}
			ID3D11Buffer* const* GetAddress()
			{
				return myBuffer.GetAddressOf();
			}

			ComPtr<ID3D11ShaderResourceView>& GetSRV()
			{
				return mySrv;
			}

			T* GetElement(size_t anIndex) 
			{
				return data[anIndex];
			}
	};
}
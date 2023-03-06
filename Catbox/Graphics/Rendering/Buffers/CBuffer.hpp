#pragma once
#include "stdafx.h"
#include <wrl.h>
#include <d3d11.h>
#include "../DX11\DX11.h"
#include <cassert>

namespace Catbox {
	template <class T>
	class CBuffer
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> myBuffer;
	private:
		D3D11_BUFFER_DESC myBufferDescription;
		D3D11_MAPPED_SUBRESOURCE myBufferData;
		const T* data;
		bool myHasInit = false;
	public:
		CBuffer() 
		{
				Initialize();

		}
		bool Initialize() 
		{
			if (myHasInit) { return true; }
			myBufferDescription = { 0 };
			myBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
			myBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			myBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			myBufferDescription.ByteWidth = sizeof(T);

			assert(myBufferDescription.ByteWidth % 16 == 0 && "Buffer byte width not dividable by 16");

			HRESULT result = DX11::Device->CreateBuffer(&myBufferDescription, nullptr, myBuffer.GetAddressOf());
			if (FAILED(result)) return false;
			myHasInit = true;
			return true;
		}
		bool SetData(const T* aData)
		{
			this->data = aData;
			ZeroMemory(&myBufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			HRESULT result = DX11::Context->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &myBufferData);
			if (FAILED(result)) return false;
			memcpy_s(myBufferData.pData, sizeof(T), aData, sizeof(T));
			DX11::Context->Unmap(myBuffer.Get(), 0);
			return true;
		}
		ID3D11Buffer* const* GetAddress()
		{
			return myBuffer.GetAddressOf();
		}
	};
}
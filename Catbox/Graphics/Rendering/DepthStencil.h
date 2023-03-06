#pragma once
#pragma once
#include <d3d11.h>
#include <wrl.h>


struct ID3D11Texture2D;
class DepthStencil
{
	friend class AssetLoader;

public:
	DepthStencil() = default;
	virtual ~DepthStencil();

	void CreateDepthStencil(int aWidth, int aHeight);
	inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const { return mySRV; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDepthStencilView() const { return myDSV; }
	inline Microsoft::WRL::ComPtr<ID3D11Resource> GetTexture() const { return myTexture; }
	inline const D3D11_VIEWPORT& GetViewport() const { return myViewport; }

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> myTex;
	Microsoft::WRL::ComPtr<ID3D11Resource> myTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDSV;
	D3D11_VIEWPORT myViewport;
};
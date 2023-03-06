#pragma once
#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Asset.h"

struct ID3D11Texture2D;
class Texture : public Asset
{
	friend class AssetLoader;
public:
	Texture() = default;
	virtual ~Texture();

	void CreateEmptyTexture(DXGI_FORMAT aFormat, int aWidth, int aHeight, int aMipLevel = 1);
	void CreateScreenSizeTexture(DXGI_FORMAT aFormat, int aWidthDivider = 1, int aHeightDivider = 1);
	void CreateVideoTexture(DXGI_FORMAT aFormat, int aWidth, int aHeight, int aMipLevel = 1, bool aDynamic = false);
	void CreateRenderTargetView();
	void SetAsResource(unsigned int aSlot);
	void Resize(int newWidth, int newHeight);
	inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const { return mySRV; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const;
	inline Microsoft::WRL::ComPtr<ID3D11Resource> GetTexture() const { return myTexture; }
	inline Microsoft::WRL::ComPtr<ID3D11Resource> GetTex() const { return tex; }

protected:

	void SaveAsset(const char* aPath) override;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	//The texture data itself when loaded on the GPU
	Microsoft::WRL::ComPtr<ID3D11Resource> myTexture;
	//The binding to allow a shader to read from the texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV;
	//The binding to allow a shader to write to the texture
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> myRTV;

	DXGI_FORMAT myFormat;
	int myWidthDivider = 1;
	int myHeightDivider = 1;
};
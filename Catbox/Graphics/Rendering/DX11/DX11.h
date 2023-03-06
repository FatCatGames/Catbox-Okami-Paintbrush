#pragma once
#include <array>
#include <wrl.h>
#include <vector>
#include "ComponentTools/Event.h"

using namespace Microsoft::WRL;

class Texture;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11SamplerState;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct D3D11_TEXTURE2D_DESC;
struct D3D11_VIEWPORT;
class DX11 
{
public:
	DX11() = default;
	~DX11() = default;

	bool Initialize(HWND aWindowHandle, bool aEnableDeviceDebug, int aWidth, int aHeight);
	void BeginFrame(std::array<float, 4> aClearColor = { 0.95f, 0.6f, 0.7f, 1 });
	void EndFrame();

	static ComPtr<ID3D11Device> Device; //Interface for creating resources such as buffers, textures, stencils etc

	//Gives context to the graphics card such as what, how and where to draw something, and sends render commands/draw calls.
	//It handles the transfer of data to/from the graphics card.
	static ComPtr<ID3D11DeviceContext> Context; 

	//An interface which holds data before it is presented, such as what is going to be drawn on screen.
	//Handles vsync, color correction, etc.
	static ComPtr<IDXGISwapChain> SwapChain;

	//Contains information which describes how to sample a texture. Is used every time we read information from a texture.
	//Can be used to create different effects, such as repeating textures, filtering textures.
	static ComPtr<ID3D11SamplerState> SampleStateDefault;
	static ComPtr<ID3D11SamplerState> SampleStateWrap;

	static ComPtr<ID3D11Texture2D> backBufferTexture;
	static ComPtr<ID3D11RenderTargetView> BackBuffer;
	static ComPtr<ID3D11RenderTargetView> SelectionBuffer;
	//static ComPtr<ID3D11DepthStencilView> DepthBuffer;
	static ComPtr<ID3D11Texture2D> selectBufferTexture;
	static ComPtr<ID3D11Texture2D> stagingTexture;
	static UINT GetScreenObjectId(UINT x, UINT y);
	static std::vector<UINT> GetScreenObjectIds(int minX, int minY, int maxX, int maxY);
	static D3D11_VIEWPORT viewPort;
	static D3D11_VIEWPORT viewPortHalfSize;
	static D3D11_VIEWPORT viewPortQuarterSize;
	static void AddResolutionChangedListener(Listener& aListener);
	static void RemoveResolutionChangedListener(Listener& aListener);

	static void SetResolution(int aWidth, int aHeight);
	static void ResizeBackBuffer(int aWidth, int aHeight);
	static inline const Vector2i& GetResolution() { return resolution; }
	static inline float GetAspectRatio() { return { resolution.x / static_cast<float>(resolution.y) }; }

private:
	static D3D11_TEXTURE2D_DESC depthBufferDesc;
	static D3D11_TEXTURE2D_DESC selectionBufferDesc;
	static D3D11_TEXTURE2D_DESC stagingTextureDesc;
	static Vector2i resolution;
	static Event OnResolutionChanged;
};
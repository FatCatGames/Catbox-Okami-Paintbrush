#include "stdafx.h"
#include "DXDeviceStates.h"

std::array<Microsoft::WRL::ComPtr<ID3D11BlendState>, static_cast<unsigned>(BlendState::BS_Count)> BlendStates::myDeviceStates;
std::array<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>, static_cast<unsigned>(DepthStencilState::DSS_Count)> DepthStencilStates::myDepthStencilStates;
std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, static_cast<unsigned>(SamplerState::SS_Count)> SamplerStates::mySamplerStates;
std::array<Microsoft::WRL::ComPtr<ID3D11RasterizerState>, static_cast<unsigned>(RastertizerState::RS_Count)> RastertizerStates::myRasterizerStates;


bool BlendStates::InitBlendStates()
{
	myDeviceStates[static_cast<int>(BlendState::BS_None)] = nullptr;

	D3D11_BLEND_DESC alphaBlendDesc = {};
	alphaBlendDesc.RenderTarget[0].BlendEnable = true;
	alphaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	alphaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT result = DX11::Device->CreateBlendState(&alphaBlendDesc, &myDeviceStates[BlendState::BS_AlphaBlend]);
	if (FAILED(result)) return false;

	D3D11_BLEND_DESC additiveBlendDesc = {};
	additiveBlendDesc.RenderTarget[0].BlendEnable = true;
	additiveBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	additiveBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additiveBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	additiveBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = DX11::Device->CreateBlendState(&additiveBlendDesc, &myDeviceStates[BlendState::BS_Additive]);
	if (FAILED(result)) return false;

	D3D11_BLEND_DESC alphaCoverageDesc = alphaBlendDesc;
	alphaCoverageDesc.AlphaToCoverageEnable = true;
	result = DX11::Device->CreateBlendState(&alphaCoverageDesc, &myDeviceStates[BlendState::BS_AlphaCoverageEnable]);
	if (FAILED(result)) return false;

	return true;
}

bool SamplerStates::InitSamplerStates()
{
	HRESULT result;

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, &mySamplerStates[SamplerState::SS_Default]);
	if (FAILED(result)) return false;

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = DX11::Device->CreateSamplerState(&samplerDesc, &mySamplerStates[SamplerState::SS_Wrap]);
	if (FAILED(result)) return false;

	D3D11_SAMPLER_DESC pointSamplerDesc;
	pointSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	pointSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	pointSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = DX11::Device->CreateSamplerState(&samplerDesc, &mySamplerStates[SamplerState::SS_PointClamp]);
	if (FAILED(result)) return false;


	pointSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pointSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pointSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = DX11::Device->CreateSamplerState(&samplerDesc, &mySamplerStates[SamplerState::SS_PointWrap]);
	if (FAILED(result)) return false;

	DX11::Context->PSSetSamplers(0, SS_Count, &mySamplerStates[0]);

	return true;
}

bool DepthStencilStates::InitDepthStencilStates()
{
	D3D11_DEPTH_STENCIL_DESC readOnlyDepthDesc = {};
	readOnlyDepthDesc.DepthEnable = true;
	readOnlyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	readOnlyDepthDesc.StencilEnable = false;

	HRESULT result = DX11::Device->CreateDepthStencilState(&readOnlyDepthDesc, &myDepthStencilStates[DepthStencilState::DSS_ReadOnly]);
	if (FAILED(result)) return false;

	myDepthStencilStates[DepthStencilState::DSS_ReadWrite] = nullptr;

	D3D11_DEPTH_STENCIL_DESC offDepthDesc = {};
	offDepthDesc.DepthEnable = false;
	offDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	offDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	offDepthDesc.StencilEnable = false;
	result = DX11::Device->CreateDepthStencilState(&offDepthDesc, &myDepthStencilStates[DepthStencilState::DSS_Off]);
	if (FAILED(result)) return false;

	return true;
}

bool RastertizerStates::InitRastertizerStates()
{
	D3D11_RASTERIZER_DESC defaultRasterizer = {};
	defaultRasterizer.CullMode = D3D11_CULL_BACK;
	defaultRasterizer.FillMode = D3D11_FILL_SOLID;

	HRESULT result = DX11::Device->CreateRasterizerState(&defaultRasterizer, &myRasterizerStates[RastertizerState::RS_Default]);
	if (FAILED(result)) return false;

	D3D11_RASTERIZER_DESC noneCullRasterizer = {};
	noneCullRasterizer.CullMode = D3D11_CULL_NONE;
	noneCullRasterizer.FillMode = D3D11_FILL_SOLID;

	result = DX11::Device->CreateRasterizerState(&noneCullRasterizer, &myRasterizerStates[RastertizerState::RS_CullNone]);
	if (FAILED(result)) return false;

	D3D11_RASTERIZER_DESC wireFrameRasterizer = {};
	wireFrameRasterizer.CullMode = D3D11_CULL_BACK;
	wireFrameRasterizer.FillMode = D3D11_FILL_WIREFRAME;

	result = DX11::Device->CreateRasterizerState(&wireFrameRasterizer, &myRasterizerStates[RastertizerState::RS_WireFrame]);
	if (FAILED(result)) return false;

	DX11::Context->RSSetState(myRasterizerStates[RastertizerState::RS_Default].Get());

	return true;
}

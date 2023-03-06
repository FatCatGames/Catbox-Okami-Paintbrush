#pragma once
#include "DX11/DX11.h"

enum SamplerState : int
{
	SS_Default,
	SS_Wrap,
	SS_PointClamp,
	SS_PointWrap,
	SS_Count
};

enum BlendState : int
{
	BS_None,
	BS_AlphaBlend,
	BS_Additive,
	BS_AlphaCoverageEnable,
	BS_Count
};

enum DepthStencilState : int
{
	DSS_ReadWrite,
	DSS_ReadOnly,
	DSS_Off,
	DSS_Count
};


enum RastertizerState : int
{
	RS_Default,
	RS_CullNone,
	RS_WireFrame,
	RS_Count
};


class BlendStates
{
public:
	static bool InitBlendStates();
	inline static const std::array<ComPtr<ID3D11BlendState>, static_cast<unsigned>(BlendState::BS_Count)>& GetBlendStates() { return myDeviceStates; }

private:
	static std::array<ComPtr<ID3D11BlendState>, static_cast<unsigned>(BlendState::BS_Count)> myDeviceStates;
};

class DepthStencilStates 
{
public:
	static bool InitDepthStencilStates();
	inline static const std::array<ComPtr<ID3D11DepthStencilState>, static_cast<unsigned>(DepthStencilState::DSS_Count)>& GetDepthStencilStates() { return myDepthStencilStates; }

private:
	static std::array<ComPtr<ID3D11DepthStencilState>, static_cast<unsigned>(DepthStencilState::DSS_Count)> myDepthStencilStates;
};

class SamplerStates
{
public:
	static bool InitSamplerStates();
	inline static const std::array<ComPtr<ID3D11SamplerState>, static_cast<unsigned>(SamplerState::SS_Count)>& GetSamplerStates() { return mySamplerStates; }

private:
	static std::array<ComPtr<ID3D11SamplerState>, static_cast<unsigned>(SamplerState::SS_Count)> mySamplerStates;
};

class RastertizerStates
{
public:
	static bool InitRastertizerStates();
	inline static const std::array<ComPtr<ID3D11RasterizerState>, static_cast<unsigned>(RastertizerState::RS_Count)>& GetRastertizerStates() { return myRasterizerStates; }

private:
	static std::array<ComPtr<ID3D11RasterizerState>, static_cast<unsigned>(RastertizerState::RS_Count)> myRasterizerStates;
};
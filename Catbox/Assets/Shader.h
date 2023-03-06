#pragma once
#include "Graphics/Rendering/DX11/DX11.h"
#include <wrl.h>
#include "Graphics/Rendering/Buffers/CBuffer.hpp"

class Shader : public Asset
{
public:
	virtual void SaveData() {};
	virtual void LoadData() {};

	virtual void SetData() {};
	virtual void SetResource() {};

	virtual void RenderInProperties() {};


	bool WasEditedThisFrame() 
	{
		bool returnValue = myWasEdited;
		myWasEdited = false;
		return returnValue;
	}

protected:
	bool myWasEdited = false;
};


class PixelShader : public Shader
{
public:

	PixelShader();

	ID3D11PixelShader* pixelShader = NULL;
	virtual std::shared_ptr<PixelShader> MakeInstance() { return std::make_shared<PixelShader>(); };

	virtual void RenderInProperties() override;

	virtual void LoadDefaultValues() override;

private:
	void SaveAsset(const char* /*aPath*/) override
	{
	}
};

class VertexShader : public Shader
{
public:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	ID3D11VertexShader* vertexShader = NULL;
	virtual std::shared_ptr<VertexShader> MakeInstance() { return std::make_shared<VertexShader>(); };

	virtual void RenderInProperties() override;

private:
	void SaveAsset(const char* /*aPath*/) override
	{
	}
};

class GeometryShader : public Shader
{
public:
	ID3D11GeometryShader* geometryShader = NULL;

	virtual void RenderInProperties() override;

private:
	void SaveAsset(const char* /*aPath*/) override
	{
	}
};
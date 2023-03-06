#pragma once
#include "UIElement.h"
#include "Assets/Shader.h"

struct ResourceDataBuffer
{
	float Resource;
	Vector3f Color;
};

class Texture;
class UISprite : public UIElement
{
public:

	UISprite();
	~UISprite();

	void Render() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	inline std::shared_ptr<Texture> GetTexture() { return myTexture; }
	inline std::shared_ptr<PixelShader> GetPixelShader() { if(myPixelShader) return myPixelShader; }
	inline std::shared_ptr<VertexShader> GetVertexShader() { return myVertexShader; }
	void SetPixelShader(std::shared_ptr<PixelShader> aPixelShader) { myPixelShader = aPixelShader; };
	void SetVertexShader(std::shared_ptr<VertexShader> aVertexShader) { myVertexShader = aVertexShader; };
	inline void SetTexture(std::shared_ptr<Texture> aTexture) { myTexture = aTexture; }
	virtual void RenderInPropertiesButtons() {};
	void Awake() override;

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;
	void ReloadShaders();
	std::filesystem::path LoadFolder(const std::filesystem::path& aDirectory, const std::filesystem::path& aPath);
	std::shared_ptr<PixelShader> myPixelShader = nullptr;
	std::shared_ptr<VertexShader> myVertexShader = nullptr;
	std::shared_ptr<Texture> myTexture;
	bool myInstanceShader = false;
};
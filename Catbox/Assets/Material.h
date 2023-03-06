#pragma once
#include "Shader.h"
#include "Asset.h"
#include "CommonUtilities/Color.h"
#include "Graphics\Rendering\DXDeviceStates.h"

class Material : public Asset
{
public:
	enum TextureChannel : unsigned int
	{
		Albedo,
		Normal,
		PbrTex, //metalness, roughness, emissive mask & emissive strength packed together
		Count
	};

	struct MaterialBufferData
	{
		Color color;
		float EmissiveStr = 1;
		Vector3f CustomValues3;
		Vector4f CustomValues4;
	} myData;

	Material();
	~Material();

	void SetAsResource(Catbox::CBuffer<MaterialBufferData>& aMaterialBuffer);

	//Textures & colors
	inline void SetAlbedoTexture(std::shared_ptr<Texture> aTexture) { myTextures[TextureChannel::Albedo] = aTexture; }
	inline const std::shared_ptr<Texture> GetAlbedoTexture() { return myTextures[TextureChannel::Albedo]; }
	inline void SetNormalTexture(std::shared_ptr<Texture> aTexture) { myTextures[TextureChannel::Normal] = aTexture; }
	inline void SetPbrTexture(std::shared_ptr<Texture> aTexture) { myTextures[TextureChannel::PbrTex] = aTexture; }
	inline const Color& GetColor() { return myData.color; }
	inline const BlendState GetBlendState() { return myBlendState; }
	inline const RastertizerState GetRastertizerState() { return myRastertizerState; }
	inline const float GetEmissive() { return myData.EmissiveStr; }
	inline void SetEmissive(float anEmissiveStrength) { myData.EmissiveStr = anEmissiveStrength; }
	inline const int& GetTilingMultiplier() { return myTilingMuliplier; }
	inline void SetColor(const Color& aColor) { myData.color = aColor; }
	std::shared_ptr<Material> MakeInstanceOfMaterial();

	//Shaders
	inline void SetVertexShader(std::shared_ptr<VertexShader> aShader) { myVertexShader = aShader; }
	inline void SetPixelShader(std::shared_ptr<PixelShader> aShader);
	inline void SetGeometryShader(std::shared_ptr<GeometryShader> aShader) { myGeometryShader = aShader; }
	inline std::shared_ptr<VertexShader> GetVertexShader() { return myVertexShader; }
	inline std::shared_ptr<GeometryShader> GetGeometryShader() { return myGeometryShader; }
	inline std::shared_ptr<PixelShader> GetPixelShader() { return myPixelShader; }
	inline bool IsInstance() { return myIsInstance; }
	inline bool IsTiling() { return myIsTiling; }
private:
	static std::shared_ptr<Texture> ourDefaultCTex;
	static std::shared_ptr<Texture> ourDefaultMTex;
	static std::shared_ptr<Texture> ourDefaultNTex;

	void SaveAsset(const char* aPath) override;
	void ReloadShaders();
	std::filesystem::path LoadFolder(const std::filesystem::path& aDirectory, const std::filesystem::path& aPath);
	void LoadFromPath(const char* aPath) override;
	void RenderTextureDropdown(std::vector<Material*>& aMaterialList, Material::TextureChannel aChannel, std::string aChannelName);
	void RenderInProperties(std::vector<Asset*>& anAssetList) override;

	std::array<std::shared_ptr<Texture>, TextureChannel::Count> myTextures;
	BlendState myBlendState = BlendState::BS_None;
	RastertizerState myRastertizerState = RastertizerState::RS_Default;
	bool myIsInstance = false;
	int myTilingMuliplier = 1;
	bool myIsTiling = false;
	bool myHasPhysxMaterial = false;

	//Shaders
	std::shared_ptr<VertexShader> myVertexShader;
	std::shared_ptr<GeometryShader> myGeometryShader;
	std::shared_ptr<PixelShader> myPixelShader;
	Vector3f myPhysxMaterial = {};
};
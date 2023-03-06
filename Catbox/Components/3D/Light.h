#pragma once

#include "GameObjects\Component.h"
#include "CommonUtilities\Color.h"

namespace Catbox
{
	template<typename T>
	class CBuffer;
}

struct LightData
{
	Catbox::Matrix4x4<float> view;
	Catbox::Matrix4x4<float> projection;

	Color color; //16 bytes

	float intensity = 1; //4 bytes
	float maxRange = 10; //4 bytes
	float innerRadius = 0.5f; //4 bytes
	float outerRadius = 1; //4 bytes

	Vector3f pos; //12 bytes
	int type = 0; //4 bytes
	Vector3f forward; //12 bytes
	int index = 0; //4 bytes

	int isActive = true; //4 bytes
	int castShadows = false; //4 bytes
	float nearPlane; //4 bytes
	float farPlane; //4 bytes
};

class DepthStencil;
class Light : public Component
{
public:

	enum class LightType
	{
		Directional,
		Point,
		Spot,
		Environment
	};

	inline LightData& GetLightData() { return myData; }
	inline void OnEnable() override { myData.isActive = myIsEnabled && myGameObject->IsActive(); }
	inline void OnDisable() override { myData.isActive = false; }
	inline void SetTexture(std::shared_ptr<Texture> aTexture) { myTexture = aTexture; }
	inline void SetColor(Color aColor) { myData.color = aColor; }
	inline void SetIntensity(float anIntensity) { myData.intensity = anIntensity; }
	inline void SetMaxRange(float aMaxRange) { myData.maxRange = aMaxRange; }
	std::shared_ptr<Texture> GetTexture() const { return myTexture; }
	void SetType(LightType aType);
	inline std::shared_ptr<DepthStencil> GetShadowMap() { return myShadowMap; }

protected:
	void Update() override;
	void RunInEditor() override;
	void CreateShadowMap();
	void CastShadows();
	void OnTransformChanged() override;
	void Init() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void OnDestroy() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	LightData myData;
	std::shared_ptr<Texture> myTexture;
	std::shared_ptr<DepthStencil> myShadowMap = nullptr;
};
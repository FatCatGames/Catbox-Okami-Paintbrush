#pragma once
#include <vector>
#include "Assets\Texture.h"

class ModelInstance;
class SceneLightData;
class GBuffer 
{
public:
	GBuffer() = default;
	~GBuffer() = default;

	enum GBufferTexture
	{
		Albedo,
		Normal,
		Material,
		VertexNormal,
		Position,
		AmbientOcclusion,
		ScreenUV,
		Count
	};

	bool CreateTextures();
	void SetAsTarget() const;
	void ClearTarget() const;
	void SetAsResource(unsigned int aStartSlot) const;
	void ClearResource(unsigned int aStartSlot) const;
	void Clear() const;
	inline Texture& GetTex(GBufferTexture aTexture) { return myTextures[aTexture]; }

private:
	std::array<Texture, GBufferTexture::Count> myTextures;
};
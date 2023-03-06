#pragma once
class BufferManager
{
public:
	void AddToRenderQueue(ModelInstance* aModel, Renderer aRendererToUse = Deferred);
	void AddToRenderQueue(ParticleSystem* aParticleSystem);
	void AddToRenderQueue(UISprite* aSprite);
	void AddToRenderQueue(DebugLine aLine);
	void AddToRenderQueue(DebugCube aCube);
	void AddToRenderQueue(NavMesh* aNavmesh);
	void AddToRenderQueue(Light* aShadowCastingLight);
private:

};


#pragma once
#include "Assets/Texture.h"


struct VideoTexture 
{
	~VideoTexture();
	static bool createAPI();
	static void destroyAPI();
	const char* myFileName = nullptr;
	struct InternalData;
	InternalData* internal_data = nullptr;

	uint32_t myXres;
	uint32_t myYres;
	bool open();
	bool create(const char* filename);
	void destroy();
	bool update(float dt);
	void ShouldLoop(bool aFlag);

	void ResetVideo();

	void pause();
	void resume();
	bool hasFinished();
	Texture* getTexture();
	float getAspectRatio() const;
};


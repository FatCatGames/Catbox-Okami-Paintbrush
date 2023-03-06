#pragma once
#include "EditorWindow.h"
#include "Graphics\Rendering\PostProcessingVolume.h"

class PostProcessingWindow : public EditorWindow
{
public:
	PostProcessingWindow();

private:
	void Render() override;
	void Save();
	void LoadFromPath(const char* aPath);
	bool myPreviewEnabled = false;
	int mySelectedProfileIndex = -1;
	std::shared_ptr<PostProcessingProfile> mySelectedProfile = nullptr;
};
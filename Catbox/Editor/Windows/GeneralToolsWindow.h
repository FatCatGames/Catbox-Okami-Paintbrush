#pragma once
#include "EditorWindow.h"

class ModelInstance;
class GeneralToolsWindow : public EditorWindow
{
public:
	GeneralToolsWindow();
	~GeneralToolsWindow();

private:
	void Render() override;
	std::shared_ptr<Prefab> mySelectedPrefab;
	GameObject* myPreview;
	ModelInstance* mySelectedPrefabModel;

	bool myRandomizeSize = true;
	float myMinSize = 0.5f;
	float myMaxSize = 1.5f;

	bool myRandomizeRotation = true;
	float myMinRot = 0;
	float myMaxRot = 360;

	float myYPos = 0;
};
#pragma once
#include <vector>
#include "../Physics/Ray.h"
#include "Windows/EditorWindowHandler.h"
#include "EditorCamera.h"
#include "MainMenuBar.h"
#include "Gizmos.h"
#include "UserPreferences.h"

class BlueprintEditor;
class AnimationControllerEditor;
class GameObject;
class SphereCollider;
class Editor
{
public:
	Editor();
	void Setup();
	void LoadDefaultScene();
	static inline Editor* GetInstance() { return Instance; }
	void Update();
	void Render();
	inline EditorCamera& GetEditorCamera() { return myEditorCamera; }
	inline EditorWindowHandler& GetWindowHandler() { return myWindowHandler; }
	inline Gizmos& GetGizmos() { return myGizmos; }
	inline bool IsUIEditMode() { return myUIEditMode; }
	inline bool IsPrefabBrushMode() { return myPrefabBrushMode; }
	void OpenScene(std::shared_ptr<Scene> aScene);
	void SetUIEditMode(bool aEditMode);
	void SetPrefabBrushMode(bool aEditMode);
	inline void SetUIVisible(bool aIsVisible) { myUIVisible = aIsVisible; }
	inline bool IsUIVisible() { return myUIVisible; }
	void DropFile(std::string aPath);
	inline void ResetTimeSinceSave() { myTimeSinceSave = 0; }
	void ClearAllSelectedObjects();
	void ResetHierarchy();
	std::vector<GameObject*> GetSelectedObjects();
	inline void SetCanDeleteObjects(bool aCanDelete) { myCanDeleteObjects = aCanDelete; }
	inline UserPreferences& GetUserPrefs() { return myUserPrefs; }
	std::shared_ptr<BlueprintEditor> GetBlueprintEditor() { return myBlueprintEditor; }
	std::shared_ptr<AnimationControllerEditor> GetAnimationControllerEditor() { return myAnimationControllerEditor; }

private:
	static Editor* Instance;

	void SelectObjectCheck();
	Vector2i mySelectRectMin;
	bool myIsBoxSelecting = false;
	bool myCanDeleteObjects = true;

	std::shared_ptr<BlueprintEditor> myBlueprintEditor;
	std::shared_ptr<AnimationControllerEditor> myAnimationControllerEditor;
	UserPreferences myUserPrefs;
	std::vector<GameObject*> mySelectedObjects;
	MainMenuBar myMainMenuBar;
	EditorWindowHandler myWindowHandler;
	EditorCamera myEditorCamera;
	Ray myRay;
	Gizmos myGizmos;
	std::shared_ptr<Scene> myCurrentScene;
	rapidjson::Document myCopiedAsset;
	bool myUIEditMode = false;
	bool myUIVisible = false;
	bool myPrefabBrushMode = false;
	bool mySceneWindowHovered = false;
	float myTimeSinceSave = 0;
};
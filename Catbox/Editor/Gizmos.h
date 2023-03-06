#pragma once

class SceneWindow;
class Gizmos
{
public:
	Gizmos() = default;
	~Gizmos() = default;

	void Update(Camera* aCamera);
	void DrawGrid(SceneWindow* aViewport);
	void RenderGizmos(SceneWindow* aViewport);
	void ClearSelectedObjects();
	void AddSelectedObject(GameObject* anObject);
	void RemoveSelectedObject(GameObject* anObject);

	void SetMoveSnap(float settings[3]);
	void SetRotateSnap(float settings[3]);
	void SetScaleSnap(float settings[3]);
	void SetSnapToggle(bool aSnapToggle);
	bool IsUsing();

private:
	void DrawTransformGizmos(SceneWindow* aViewport);
	std::vector<Transform*> mySelectedObjects;
	std::array<float, 16> myView;
	std::array<float, 16> myProjection;
	bool myEditLastFrame = false;
};
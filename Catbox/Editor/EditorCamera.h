#pragma once
#include "GameObjects/Transform.h"
#include "Components\3D\Camera.h"

class EditorCamera
{
public:
	EditorCamera();
	~EditorCamera() = default;
	void ScrollZoom();
	void Movement();
	void Rotate();
	Transform& GetTransform();
	inline Camera* GetCamera() { return &myCamera; }
	inline Camera* GetUICamera() { return &myUICamera; }
	inline void SetMoveSpeed(float aSpeed) { myMoveSpeed = aSpeed; }
	inline void SetScrollSpeed(float aSpeed) { myScrollSpeed = aSpeed; }
	inline float GetScrollSpeed() { return myScrollSpeed; }
	inline void SetRotateSpeed(float aSpeed) { myRotateSpeed = aSpeed; }

private:
	float myMoveSpeed = 10;
	float myScrollSpeed = 30;
	float myRotateSpeed = 20;
	Camera myCamera;
	Camera myUICamera;
	Transform myTransform;
	Transform myUICameraTransform;

};
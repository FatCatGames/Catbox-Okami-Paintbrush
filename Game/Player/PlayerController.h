#pragma once
#include "Audio\AudioMonoComponent.h"
#include "Components\Physics\CharacterController.h"

class PlayerCamera;
class Animator;
class PlayerController : public Component
{
public:
	void OnObjectFinishedLoading() override;
	void Update() override;
	void OnCollisionStay(Collider* aCollider) override;

	void RunKeyboardInput();

	void HandleJump();
	void HandleCameraMouseMovement();

private:
	Animator* myAnimator = nullptr;
	CharacterController* myCharacterController = nullptr;
	AudioMonoComponent* myAudioMonoComponent = nullptr;
	PlayerCamera* myCamera = nullptr;
	Vector3f myVelocity = { 0,0,0 };
	float myMoveSpeed = 5;

	const float myCameraYOffset = 1.5f;
	const float myCameraSensitivity = 3;

	float myZoomStartTime = 0.0f;
	float myInvertedCameraFactor = -1.0f;

	const float myDefaultGravity = 40.0f;
	const float myMaxJumpDuration = 0.2f;
	float myJumpTime = 0.0f;
	float myJumpStrength = 10;

	const float myRespawnCooldown = 3.0f;
	float myRespawnTimer = 0.0f;

	bool myIsGrounded = true;
	bool myIsJumping = false;
	bool myHasSetup = false;
};
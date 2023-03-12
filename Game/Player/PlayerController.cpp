#include "Game.pch.h"
#include "PlayerController.h"
#include "Audio\AudioMonoComponent.h"
#include "Player\PlayerCamera.h"
#include "Components\CameraController.h"
#include "Physics\PhysXUtilities.h"
#include "..\Catbox\ComponentTools\UIEventHandler.h"
#include "Components\Animator.h"
#include "Canvas.h"

void PlayerController::OnObjectFinishedLoading()
{
	myAnimator = myGameObject->GetTransform()->GetChildren()[0]->GetGameObject()->GetComponent<Animator>();

	myCharacterController = myGameObject->GetComponent<CharacterController>();
	myCharacterController->SetMovementSpeed(myMoveSpeed);


	myAudioMonoComponent = myTransform->GetGameObject()->GetComponent<AudioMonoComponent>();
	if (myAudioMonoComponent)
	{
		myAudioMonoComponent->PlaySoundEffect(6);

		std::string daThing = "bus:/Music + Ambience";
		Engine::GetInstance()->GetAudioManager()->SetNewMusicBusPath(daThing);
		Engine::GetInstance()->GetAudioManager()->SetMusicVolume(2);
	}
}

void PlayerController::Update()
{
	if (!myHasSetup)
	{
		myCamera = Engine::GetInstance()->GetCameraController()->GetPlayerCamera();
		myCamera->SetFollowTarget(myTransform);
		myHasSetup = true;
	}

	RunKeyboardInput();
}

void PlayerController::RunKeyboardInput()
{
	if (Engine::GetInstance()->IsViewportHovered())
	{
		float rotInDegrees = myCharacterController->GetTransform()->worldRot().y;
		Vector3f toTarget;
		bool left = Input::GetKeyHeld(KeyCode::A);
		bool right = Input::GetKeyHeld(KeyCode::D);
		bool forward = Input::GetKeyHeld(KeyCode::W);
		bool back = Input::GetKeyHeld(KeyCode::S);
		bool givingInput = left || right || forward || back;

		/*if (givingInput)
		{
		}

		rotInDegrees += (left * -90) + (right * 90) + (back * 180);*/


		if (Input::GetKeyHeld(KeyCode::W))
		{
			toTarget += Engine::GetInstance()->GetMainCamera()->GetTransform()->forward();
		}
		if (Input::GetKeyHeld(KeyCode::S))
		{
			toTarget += Engine::GetInstance()->GetMainCamera()->GetTransform()->back();
		}
		if (Input::GetKeyHeld(KeyCode::A))
		{
			toTarget += Engine::GetInstance()->GetMainCamera()->GetTransform()->left();
		}
		if (Input::GetKeyHeld(KeyCode::D))
		{
			toTarget += Engine::GetInstance()->GetMainCamera()->GetTransform()->right();
		}

		rotInDegrees = (std::atan2(toTarget.x, toTarget.z) * 57.3f);

		HandleCameraMouseMovement();
		HandleJump();
		HandlePainting();

		myAnimator->SetFloat("Speed", givingInput ? myMoveSpeed : 0);

		if (givingInput)
		{
			toTarget.y = 0;
			toTarget.Normalize();
			myCharacterController->Move(toTarget);
			myCharacterController->Slerp({ 0, rotInDegrees, 0 }, 10.0f);
		}
		else
		{
			myCharacterController->Move({ 0,0,0 });
		}
	}
}

void PlayerController::HandlePainting()
{
	if (Input::GetKeyPress(KeyCode::CTRL))
	{
		myIsPainting = true;
		Canvas::GetInstance()->StartPainting();
		Engine::GetInstance()->SetGamePaused(true);
	}
	else if (Input::GetKeyReleased(KeyCode::CTRL))
	{
		myIsPainting = false;
		Engine::GetInstance()->SetGamePaused(false);
	}
}


void PlayerController::HandleCameraMouseMovement()
{
	Vector2i mouseDelta;
	if (EDITORMODE)
	{
		mouseDelta = Input::GetMouseDelta();
	}
	else
	{
		mouseDelta = Input::GetRawMouseDelta();
	}
	myCamera->RotateAroundObject(-mouseDelta.x * myCameraSensitivity, -mouseDelta.y * myCameraSensitivity);
}

void PlayerController::HandleJump()
{
	if (Input::GetKeyPress(KeyCode::SPACE) && myIsGrounded)
	{
		myJumpTime = 0.0f;
		myIsGrounded = false;
		myIsJumping = true;
		myCharacterController->AddForce({ 0, 1, 0 }, myJumpStrength);
		//myAnimator->SetBool("Grounded", false);
		//myAnimator->SetBool("HasJumped", true);

		if (myAudioMonoComponent) { myAudioMonoComponent->PlaySoundEffect(2); }
	}

	if (myIsJumping == true)
	{
		if (Input::GetKeyHeld(KeyCode::SPACE))
		{
			myJumpTime += deltaTime;

			const Vector3f velocity = myCharacterController->GetVelocity();
			myCharacterController->SetVelocity({ velocity.x, myJumpStrength, velocity.z });

			if (myJumpTime > myMaxJumpDuration)
			{
				myIsJumping = false;
				myJumpTime = 0.0f;
			}
		}
		if (Input::GetKeyReleased(KeyCode::SPACE))
		{
			myIsJumping = false;
			myJumpTime = 0.0f;
		}
	}
}

void PlayerController::OnCollisionStay(Collider* aCollider)
{
	CollisionDirection direction = myCharacterController->GetCollisionDirection();

	if (direction == CollisionDirection::PxCF_Down)
	{
		//myAnimator->SetBool("Grounded", true);

		myIsGrounded = true;
		myCharacterController->SetGravity(myDefaultGravity);
	}
}
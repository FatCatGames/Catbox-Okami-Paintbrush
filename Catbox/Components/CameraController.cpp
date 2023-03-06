#include "stdafx.h"
#include "CameraController.h"
#include "../../Game/Player/PlayerCamera.h"

CameraController::CameraController()
{
	myCameras = {};
	myCurrentCamera = nullptr;
	myDelayStartTime = 0.0f;
	myTempStartTime = 0.0f;
	myTemporaryCamera = nullptr;
	myIsTransitioning = false;
}

CameraController::~CameraController()
{
	myTemporaryCamera = nullptr;
}

void CameraController::AddCamera(Camera* aCamera)
{
	myCameras.push_back(aCamera);
}

std::vector<Camera*>& CameraController::GetCameras()
{
	return myCameras;
}

void CameraController::RemoveCamera(Camera* aCamera)
{
	if (myTemporaryCamera != nullptr)
	{
		if (myTemporaryCamera->GetComponent<Camera>() == aCamera)
		{
			myTemporaryCamera->Destroy();
		}
	}

	if (myCameras.size() > 0)
	{
		// Could be important to add back.
		/*if (Engine::GetInstance()->GetMainCamera() == aCamera)
		{
			Engine::GetInstance()->SetMainCamera(myCameras[0]);
			myCurrentCamera = Engine::GetInstance()->GetActiveCamera();
		}

		if (Engine::GetInstance()->GetActiveCamera() == aCamera)
		{
			Engine::GetInstance()->SetActiveCamera(Engine::GetInstance()->GetMainCamera());
			myCurrentCamera = Engine::GetInstance()->GetActiveCamera();
		}*/
	}

	for (int i = 0; i < myCameras.size(); i++)
	{
		if (aCamera == myCameras[i])
		{
			myCameras.erase(myCameras.begin() + i);
		}
	}
}

void CameraController::SetPlayerCamera(PlayerCamera* aCamera)
{
	myPlayerCamera = aCamera;
	myCurrentCamera = aCamera;
}

PlayerCamera* CameraController::GetPlayerCamera()
{
	if (myPlayerCamera == nullptr) { printerror("No Player Camera in scene"); }
	return dynamic_cast<PlayerCamera*>(myPlayerCamera);
}

void CameraController::Update()
{
	CinematicCamera* currentCam = dynamic_cast<CinematicCamera*>(myCurrentCamera);

	if (currentCam != nullptr && myTemporaryCamera == nullptr)
	{
		if (currentCam->GetNextCamera() != -1)
		{
			if (currentCam->GetIsInstant())
			{
				TransitionTo(myCameras[currentCam->GetNextCamera()]);
			}
			else
			{
				if (currentCam->GetDelay() > 0.0f && myDelayStartTime < currentCam->GetDelay())
				{
					myDelayStartTime += deltaTime;
				}
				else
				{
					SmoothTransitionTo(myCameras[currentCam->GetNextCamera()], currentCam->GetSpeed());
				}
			}
		}
	}

	if (myTemporaryCamera != nullptr)
	{
		myTempStartTime += deltaTime;
		myTempStartTime = Catbox::Clamp(myTempStartTime, 0.0f, mySecondsToTransition);

		if (LerpFoV() + LerpPosition() + LerpRotation() <= 0.01f)
		{
			myIsTransitioning = false;
			RemoveCamera(myTemporaryCamera->GetComponent<Camera>());
			TransitionTo(myCurrentCamera);
		}
	}
	
	// For Debug purposes
	/*if (Input::GetKeyPress(KeyCode::G))
	{
		if (myCameras.size() > 0)
		{
			SmoothTransitionTo(myCameras[1], 3.0f);
		}
	}*/
}

void CameraController::Clear()
{
	myCameras.clear();

	myTemporaryCamera = nullptr;

	myCurrentCamera = Engine::GetInstance()->GetMainCamera();
}

void CameraController::TransitionTo(Camera* aCamera)
{
	myTempStartTime = 0.0f;
	myTemporaryCamera = nullptr;

	for (size_t i = 0; i < myCameras.size(); i++)
	{
		if (myCameras[i] == aCamera)
		{
			myCurrentCamera = myCameras[i];
		}
	}

	Engine::GetInstance()->SetActiveCamera(aCamera);

	/*if (dynamic_cast<CinematicCamera*>(aCamera))
	{
		CinematicCamera* cam = dynamic_cast<CinematicCamera*>(aCamera);

		if (cam->GetNextCamera() != -1)
		{
			SmoothTransitionTo(myCameras[cam->GetNextCamera()], dynamic_cast<CinematicCamera*>(aCamera)->GetDelay());
		}
	}*/
}

void CameraController::SmoothTransitionTo(Camera* aCameraTo, float someSeconds)
{
	if (myCurrentCamera == nullptr) { myCurrentCamera = Engine::GetInstance()->GetActiveCamera(); }
	myDelayStartTime = 0.0f;
	myTempStartTime = 0.0f;
	myIsTransitioning = true;
	myTemporaryCamera = nullptr;
	for (size_t i = 0; i < myCameras.size(); i++)
	{
		if (myCameras[i] == aCameraTo)
		{
			myTemporaryCamera = Instantiate();
			myTemporaryCamera->AddComponent<Camera>();
			myTemporaryCamera->GetComponent<Camera>()->SetFoV(myCurrentCamera->GetFoV());
			myTemporaryCamera->GetTransform()->SetWorldPos(myCurrentCamera->GetTransform()->worldPos());
			myTemporaryCamera->GetTransform()->SetWorldRot(myCurrentCamera->GetTransform()->worldRot());
			myTemporaryCamera->SetName("Temporary camera (DON'T CLICK)");

			//myTemporaryCamera = myCurrentCamera->GetGameObject().Duplicate();
			//myTemporaryCamera->SetName("Temporary camera (DON'T CLICK)");

			myTemporaryCameraOrigin.fov = myTemporaryCamera->GetComponent<Camera>()->GetFoV();
			myTemporaryCameraOrigin.offset = myTemporaryCamera->GetTransform()->worldPos();
			myTemporaryCameraOrigin.rotation = myTemporaryCamera->GetTransform()->worldRot();

			myCurrentCamera = aCameraTo;


			mySecondsToTransition = someSeconds;
			Engine::GetInstance()->SetActiveCamera(myTemporaryCamera->GetComponent<Camera>());
		}
	}
}

float CameraController::LerpFoV()
{
	myTemporaryCamera->GetComponent<Camera>()->SetFoV(Catbox::Lerp(myTemporaryCameraOrigin.fov, myCurrentCamera->GetFoV(), myTempStartTime / mySecondsToTransition));

	float diff = myCurrentCamera->GetFoV() - myTemporaryCamera->GetComponent<Camera>()->GetFoV();

	return abs(diff);
}

float CameraController::LerpPosition()
{
	Vector3f cameraPos = myCurrentCamera->GetTransform()->worldPos();
	Vector3f newPos;
	newPos.x = Catbox::Lerp(myTemporaryCameraOrigin.offset.x, cameraPos.x, myTempStartTime / mySecondsToTransition);
	newPos.y = Catbox::Lerp(myTemporaryCameraOrigin.offset.y, cameraPos.y, myTempStartTime / mySecondsToTransition);
	newPos.z = Catbox::Lerp(myTemporaryCameraOrigin.offset.z, cameraPos.z, myTempStartTime / mySecondsToTransition);

	myTemporaryCamera->GetTransform()->SetWorldPos(newPos);

	float diff = (cameraPos - myTemporaryCamera->GetTransform()->worldPos()).Length();

	return diff;
}

float CameraController::LerpRotation()
{
	Vector3f cameraRot = myCurrentCamera->GetTransform()->worldRot();
	Vector3f newRot;
	newRot.x = Catbox::Lerp(myTemporaryCameraOrigin.rotation.x, cameraRot.x, myTempStartTime / mySecondsToTransition);
	newRot.y = Catbox::Lerp(myTemporaryCameraOrigin.rotation.y, cameraRot.y, myTempStartTime / mySecondsToTransition);
	newRot.z = Catbox::Lerp(myTemporaryCameraOrigin.rotation.z, cameraRot.z, myTempStartTime / mySecondsToTransition);

	myTemporaryCamera->GetTransform()->SetWorldRot(newRot);

	float diff = (cameraRot - myTemporaryCamera->GetTransform()->worldRot()).Length();

	return diff;
}

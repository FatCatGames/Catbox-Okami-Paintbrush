#include "Game.pch.h"
#include "PlayerCamera.h"
#include "../Catbox/Components/CameraController.h"
#include "Components/Physics/Collisions/BoxCollider.h"
#include "Physics/PhysXUtilities.h"

PlayerCamera::PlayerCamera()
{
	Engine::GetInstance()->GetCameraController()->SetPlayerCamera(this);
	myTargetPosition = { 0.0f, 0.0f, 0.0f };

	mySpeed = 10.0f;
	myMinAngleRadian = -1.4f;
	myMaxAngleRadian = 1.4f;

	//myDynamicYOffset = 0; // Currently not in use
}

PlayerCamera::~PlayerCamera()
{
	Engine::GetInstance()->GetCameraController()->SetPlayerCamera(nullptr);
}

void PlayerCamera::Update()
{
	if (myFollowTarget != nullptr)
	{
		Vector3f followPos = myFollowTarget->worldPos() + Vector3f{ 0.0f, myYFollowOffset, 0.0f };

		float xRotationPos = sinf(myXRotationRadians) * cosf(myYRotationRadians) * myCurrentFollowOffset.z;
		float yRotationPos = sinf(myYRotationRadians) * myCurrentFollowOffset.y;
		float zRotationPos = cosf(myXRotationRadians) * cosf(myYRotationRadians) * myCurrentFollowOffset.z;

		// For slerping, not in use
		/*Catbox::Quaternion targetQuat = Catbox::ToQuaternion({ xRotation, yRotation, zRotation });
		Catbox::Quaternion currectQuat = Catbox::ToQuaternion(myTransform->localRot());
		physx::PxQuat targetPXQuat(targetQuat.x, targetQuat.y, targetQuat.z, targetQuat.w);
		physx::PxQuat currectPXQuat(currectQuat.x, currectQuat.y, currectQuat.z, currectQuat.w);
		myTargetRotation = physx::PxSlerp(-1.0f * deltaTime, currectPXQuat, targetPXQuat);
		Catbox::Quaternion newRotation({ myTargetRotation.x, myTargetRotation.y, myTargetRotation.z, myTargetRotation.w });
		myTransform->SetLocalRot(Catbox::ToEulerAngles(newRotation));*/

		Vector3f target = (followPos + Vector3f{ xRotationPos, yRotationPos, zRotationPos });


		Vector3f aDir = (followPos - Vector3f(0, 1.0f, 0) - target) * -1.0f;
		HitReport hit = PhysXUtilities::RayCast(followPos + (aDir.GetNormalized() * 0.8f), aDir, 3.0f, 2.0f, { 1, 1, 1, 1 }, 1 << 0);
		HitReport reverseHit = PhysXUtilities::RayCast(myTransform->worldPos() + (aDir.GetNormalized() * -0.8f), aDir * -1.0f, 3.0f, 2.0f, { 1, 1, 1, 1 }, 1 << 0);

		//print(std::to_string(hit.Hit) + " - " + std::to_string(reverseHit.Hit));
		//print(std::to_string(myCurrentFollowOffset.x) + " - " + std::to_string(myCurrentFollowOffset.y) + " - " + std::to_string(myCurrentFollowOffset.z));

		float followOffsetMin = 0.2f;
		float currentFollowDirection = 0.0f;

		static float timer;

		if (hit.NormalHit.Dot(myTransform->forward()) > 0.01f || reverseHit.NormalHit.Dot(myTransform->forward()) > 0.01f)
		{
			if (hit.Hit || reverseHit.Hit)
			{
				currentFollowDirection = -1.0f;
			}
			else
			{
				currentFollowDirection = 1.0f;
			}

			timer = 0.0f;
		}
		else
		{
			timer += deltaTime;

			if (timer >= 0.3f)
			{
				currentFollowDirection = 0.3f * timer;
			}
		}

		//print(std::to_string(hit.NormalHit.Dot(myTransform->forward()) > 0.01f) + " - " + std::to_string(reverseHit.NormalHit.Dot(myTransform->forward()) > 0.01f));

		myCurrentFollowOffset += Vector3f{ 1.0f, 1.0f, 1.0f } *currentFollowDirection * deltaTime * mySpeed;

		myCurrentFollowOffset.x = Catbox::Clamp(myCurrentFollowOffset.x, followOffsetMin, myFollowOffset.x);
		myCurrentFollowOffset.y = Catbox::Clamp(myCurrentFollowOffset.y, followOffsetMin, myFollowOffset.y);
		myCurrentFollowOffset.z = Catbox::Clamp(myCurrentFollowOffset.z, followOffsetMin, myFollowOffset.z);


		myTargetPosition = target /*+ hit.NormalHit * 2.0f*/;


		Vector3f distance = myTargetPosition - myTransform->worldPos();
		Vector3f dir = distance.GetNormalized();

		Vector3f lerpPos = (myTransform->worldPos() + (dir * mySpeed * deltaTime * distance.Length()));

		float xRot = myXRotationRadians;
		xRot -= std::floor(xRot / (2.0f * Catbox::Pi())) * 2.0 * Catbox::Pi();
		xRot = abs(sinf(xRot));
		xRot = Catbox::SmoothStep(0.0f, 1.0f, xRot);

		if ((lerpPos - myTargetPosition).Length() < 0.1f)
		{
			lerpPos = myTransform->worldPos();
		}

		Vector3f newPos;
		newPos.x = Catbox::Lerp(myTargetPosition.x, lerpPos.x, xRot);
		newPos.y = myTargetPosition.y;
		newPos.z = Catbox::Lerp(lerpPos.z, myTargetPosition.z, xRot);

		// Debug
		/*if (Input::GetKeyHeld(KeyCode::G))
		{
			print(std::to_string(0) + " - " + std::to_string(myCurrentFollowOffset.x) + "," + std::to_string(myCurrentFollowOffset.y) + "," + std::to_string(myCurrentFollowOffset.z));
		}*/


		myTransform->SetWorldPos(newPos);

		myTransform->LookAt(followPos);

		/*if (myTargetPosition == hit.PositionHit)
		{
			myTransform->LookAtHorizontal(followPos);
		}
		else
		{
			myTransform->LookAt(followPos);
		}*/
	}


	/*if (Input::GetController().IsConnected())
	{
		Vector3f transformPos = myTransform->worldPos();

		Vector3f distance = myTargetPosition - myTransform->worldPos();
		Vector3f dir = distance.GetNormalized();

		transformPos += dir * mySpeed * deltaTime * distance.Length();

		myTransform->SetWorldPos(transformPos);
	}
	else
	{

	}*/

	if (Engine::GetInstance()->GetActiveCamera() == this)
	{
		CreateProjectionMatrix();
	}
}

void PlayerCamera::Init()
{
	Engine::GetInstance()->GetCameraController()->AddCamera(this);
}

void PlayerCamera::Awake()
{
	if (!Engine::GetInstance()->GetMainCamera())
	{
		Engine::GetInstance()->SetMainCamera(this);
	}

	if (!Engine::GetInstance()->GetActiveCamera())
	{
		Engine::GetInstance()->SetActiveCamera(this);
	}

	myCurrentFollowOffset = myFollowOffset;
}

void PlayerCamera::SetFollowTarget(Transform* aTarget)
{
	myFollowTarget = aTarget;
}

Vector3f PlayerCamera::GetFollowOffset()
{
	return myFollowOffset;
}

void PlayerCamera::SetFollowOffset(Vector3f anOffset)
{
	myFollowOffset = anOffset;
}

void PlayerCamera::RotateAroundObject(float someDegreesX, float someDegreesY)
{
	myXRotationRadians += -someDegreesX * deltaTime;

	myYRotationRadians -= someDegreesY * deltaTime;
	myYRotationRadians = Catbox::Clamp<float>(myYRotationRadians, myMinAngleRadian, myMaxAngleRadian);
}

void PlayerCamera::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto& cameras = ComponentVectorCast<PlayerCamera>(aComponentList);

	if (Catbox::Checkbox("View through camera", &myIsPreviewing))
	{
		if (myIsPreviewing)
		{
			Engine::GetInstance()->GetGraphicsEngine()->SetCamera(this);
		}
		else
		{
			Engine::GetInstance()->GetGraphicsEngine()->SetCamera(Editor::GetInstance()->GetEditorCamera().GetCamera());
		}
	}

	if (Catbox::DragFloat("Field of View", &myFoV, 0.1f, 45, 120))
	{
		for (auto& cam : cameras)
		{
			cam->myFoV = myFoV;
			cam->CreateProjectionMatrix();
		}
	}

	ImGui::Text("Post Processing");
	std::shared_ptr<PostProcessingVolume> sharedPPV = myPostProcessingVolume;
	for (auto& cam : cameras)
	{
		if (cam->myPostProcessingVolume != sharedPPV)
		{
			sharedPPV = nullptr;
			break;
		}
	}


	if (!myPostProcessingVolume)
	{
		if (ImGui::Button("Add New Volume"))
		{
			myPostProcessingVolume = std::make_shared<PostProcessingVolume>();
		}
	}
	else if (sharedPPV)
	{
		myPostProcessingVolume->RenderInProperties();

		if (ImGui::Button("Remove Volume"))
		{
			myPostProcessingVolume = nullptr;
		}
	}


	ImGui::Text("Offset to player");
	if (Catbox::InputFloat("OffsetX", &myFollowOffset.x))
	{
		for (auto& cam : cameras)
		{
			cam->myFollowOffset.x = myFollowOffset.x;
		}
	}
	if (Catbox::InputFloat("OffsetY", &myFollowOffset.y))
	{
		for (auto& cam : cameras)
		{
			cam->myFollowOffset.y = myFollowOffset.y;
		}
	}
	if (Catbox::InputFloat("OffsetZ", &myFollowOffset.z))
	{
		for (auto& cam : cameras)
		{
			cam->myFollowOffset.z = myFollowOffset.z;
		}
	}

	ImGui::Text("Y Angle Clamping (Radians)");
	if (Catbox::InputFloat("Angle Min", &myMinAngleRadian))
	{
		for (auto& cam : cameras)
		{
			cam->myMinAngleRadian = myMinAngleRadian;
		}
	}
	if (Catbox::InputFloat("Angle Max", &myMaxAngleRadian))
	{
		for (auto& cam : cameras)
		{
			cam->myMaxAngleRadian = myMaxAngleRadian;
		}
	}

	ImGui::Text("Camera Lerp Speed");
	if (Catbox::InputFloat("Speed", &mySpeed))
	{
		for (auto& cam : cameras)
		{
			cam->mySpeed = mySpeed;
		}
	}
}

void PlayerCamera::Save(rapidjson::Value& aComponentData)
{
	int version = 4;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Float>("FoV", myFoV);
	wrapper.SaveValue<DataType::Float>("NearPlane", myNearPlane);
	wrapper.SaveValue<DataType::Float>("FarPlane", myFarPlane);
	wrapper.SaveValue<DataType::Float>("Speed", mySpeed);
	wrapper.SaveValue<DataType::Float>("AngleMax", myMaxAngleRadian);
	wrapper.SaveValue<DataType::Float>("AngleMin", myMinAngleRadian);

	if (myPostProcessingVolume)
	{
		auto volume = wrapper.CreateObject();
		myPostProcessingVolume->Save(volume);
		wrapper.SetTarget(aComponentData);
		wrapper.SaveObject("Post Processing Volume", volume);
	}
	wrapper.SaveValue<DataType::Float>("OffsetX", myFollowOffset.x);
	wrapper.SaveValue<DataType::Float>("OffsetY", myFollowOffset.y);
	wrapper.SaveValue<DataType::Float>("OffsetZ", myFollowOffset.z);
}

void PlayerCamera::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	myFoV = aComponentData["FoV"].GetFloat();
	myNearPlane = aComponentData["NearPlane"].GetFloat();
	myFarPlane = aComponentData["FarPlane"].GetFloat();

	if (version == 4)
	{
		if (aComponentData.HasMember("Post Processing Volume"))
		{
			myPostProcessingVolume = std::make_shared<PostProcessingVolume>();
			myPostProcessingVolume->Load(aComponentData["Post Processing Volume"].GetObj());
		}

		mySpeed = aComponentData["Speed"].GetFloat();
		myMaxAngleRadian = aComponentData["AngleMax"].GetFloat();
		myMinAngleRadian = aComponentData["AngleMin"].GetFloat();
	}
	else
	{
		printerror("Wrong json version on camera");
	}
	if (aComponentData.HasMember("OffsetX"))
	{
		myFollowOffset.x = aComponentData["OffsetX"].GetFloat();
	}
	if (aComponentData.HasMember("OffsetY"))
	{
		myFollowOffset.y = aComponentData["OffsetY"].GetFloat();
	}
	if (aComponentData.HasMember("OffsetZ"))
	{
		myFollowOffset.z = aComponentData["OffsetZ"].GetFloat();
	}
}

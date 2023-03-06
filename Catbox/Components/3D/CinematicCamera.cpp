#include "stdafx.h"
#include "CinematicCamera.h"
#include "CameraController.h"

CinematicCamera::CinematicCamera()
{
}

CinematicCamera::~CinematicCamera()
{
}

void CinematicCamera::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto& cameras = ComponentVectorCast<CinematicCamera>(aComponentList);

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

	if (!sharedPPV)
	{
		if (!myPostProcessingVolume)
		{
			if (ImGui::Button("Add New Volume"))
			{
				myPostProcessingVolume = std::make_shared<PostProcessingVolume>();
			}
		}
		else
		{
			myPostProcessingVolume->RenderInProperties();

			if (ImGui::Button("Remove Volume"))
			{
				myPostProcessingVolume = nullptr;
			}
		}
	}

	auto& cams = Engine::GetInstance()->GetCameraController()->GetCameras();
	static const char* selectedItem = "Select Camera";
	if (myNextCamera == -1)
	{
		selectedItem = "Select Camera";
	}
	else
	{
		if (myNextCamera < cams.size())
		{
			selectedItem = cams[myNextCamera]->GetGameObject().GetName().c_str();
		}
	}

	if (ImGui::BeginCombo("Next Camera", selectedItem))
	{
		for (size_t i = 0; i < cams.size(); i++)
		{
			if (ImGui::Selectable(cams[i]->GetGameObject().GetName().c_str(), false))
			{
				myNextCamera = i;
				selectedItem = cams[i]->GetGameObject().GetName().c_str();
			}
		}
		ImGui::EndCombo();
	}

	if (myNextCamera >= 0)
	{
		Catbox::Checkbox("Instant transition", &myIsInstant);
		if (!myIsInstant)
		{
			Catbox::InputFloat("Delay before next camera", &myDelay);
			Catbox::InputFloat("Transition time in seconds", &mySpeed);
		}
	}
}

void CinematicCamera::Save(rapidjson::Value& aComponentData)
{
	int version = 3;
	auto& wrapper = *RapidJsonWrapper::GetInstance();

	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Float>("FoV", myFoV);
	wrapper.SaveValue<DataType::Float>("NearPlane", myNearPlane);
	wrapper.SaveValue<DataType::Float>("FarPlane", myFarPlane);

	wrapper.SaveValue<DataType::Int>("NextCamera", myNextCamera);
	wrapper.SaveValue<DataType::Bool>("Instant", myIsInstant);
	wrapper.SaveValue<DataType::Float>("Delay", myDelay);
	wrapper.SaveValue<DataType::Float>("Speed", mySpeed);

	if (myPostProcessingVolume)
	{
		auto volume = wrapper.CreateObject();
		myPostProcessingVolume->Save(volume);
		wrapper.SetTarget(aComponentData);
		wrapper.SaveObject("Post Processing Volume", volume);
	}
}

void CinematicCamera::Load(rapidjson::Value& aComponentData)
{
	int version = aComponentData["Version"].GetInt();

	myFoV = aComponentData["FoV"].GetFloat();
	myNearPlane = aComponentData["NearPlane"].GetFloat();
	myFarPlane = aComponentData["FarPlane"].GetFloat();

	if (version == 3)
	{
		if (aComponentData.HasMember("Post Processing Volume"))
		{
			myPostProcessingVolume = std::make_shared<PostProcessingVolume>();
			myPostProcessingVolume->Load(aComponentData["Post Processing Volume"].GetObj());
		}

		myNextCamera = aComponentData["NextCamera"].GetInt();
		myIsInstant = aComponentData["Instant"].GetBool();
		myDelay = aComponentData["Delay"].GetFloat();
		mySpeed = aComponentData["Speed"].GetFloat();
	}
	else
	{
		printerror("Wrong json version on camera");
	}
}

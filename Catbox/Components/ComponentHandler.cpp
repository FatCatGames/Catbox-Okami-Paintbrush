#include "stdafx.h"
#include "ComponentHandler.h"

#include "Components/UI/VideoPlayer.h"
#include "3D\ModelInstance.h"
#include "Animator.h"
#include "Components\Physics\RigidBody.h"
#include "3D\Light.h"
#include "Components/ParticleSystem.h"
#include "Components\3D\Camera.h"
#include "Components\3D\CinematicCamera.h"
#include "Audio/Audio3DComponent.h"
#include "Audio/AudioMonoComponent.h"
#include "Components\StateMachine.h"
#include "Components\Physics/Collisions\BoxCollider.h"
#include "Components\Physics/Collisions\SphereCollider.h"
#include "Navigation\NavAgent.h"
#include "Components/SplashScreen.h"
#include "Components/CameraShake.h"
#include "Components/FollowTarget.h"
#include "Components/UI/UISprite.h"
#include "Components/BlueprintComponent.h"
#include "Components/Physics/CharacterController.h"
#include "Components/Physics/Collisions/ConvexCollider.h"
#include "Components/Physics/Collisions/MeshCollider.h"
#include "Components/Physics/LiquidSimulation.h"

std::map<std::string, std::function<Component* (GameObject*)>> ComponentHandler::AddComponentFunc;
std::map<const char*, std::string> ComponentHandler::ComponentNames;
std::string ComponentHandler::ErrorStr = "error";

void ComponentHandler::ManuallyRegisterComponents()
{
	RegisterComponent<ModelInstance>();
	RegisterComponent<Animator>();
	RegisterComponent<RigidBody>();
	RegisterComponent<Light>();
	RegisterComponent<ParticleSystem>();
	RegisterComponent<Camera>();
	RegisterComponent<CinematicCamera>();
	RegisterComponent<StateMachine>();
	RegisterComponent<BoxCollider>();
	RegisterComponent<SphereCollider>();
	RegisterComponent<VideoPlayer>();
	RegisterComponent<NavAgent>();
	RegisterComponent<SplashScreen>();
	RegisterComponent<CameraShake>();
	RegisterComponent<FollowTarget>();
	RegisterComponent<Audio3DComponent>();
	RegisterComponent<AudioMonoComponent>();
	RegisterComponent<UISprite>();
	RegisterComponent<BlueprintComponent>();
	RegisterComponent<CharacterController>();
	RegisterComponent<ConvexCollider>();
	RegisterComponent<MeshCollider>();
	RegisterComponent<LiquidSimulation>();
}

void ComponentHandler::RenderComponentProperties(Component* /*aComponent*/)
{
	ImGui::Text("Imgui not implemented for component");
}

Component* ComponentHandler::AddComponentToObjectByName(GameObject* anObject, string aComponentType)
{
	if (ComponentHandler::AddComponentFunc.count(aComponentType)) 
	{
		return ComponentHandler::AddComponentFunc[aComponentType](anObject);
	}
	printerror("Component type " + aComponentType + " not registered");
	return nullptr;
}

#include "Game.pch.h"
#include "GameScene.h"
#include <PopupManager.h>
#include "Components/Physics/Collisions/CollisionManager.h"
#include "Components/Physics/Collisions/Collider.h"

GameScene* GameScene::Instance;

GameScene::GameScene()
{
	Instance = this;
}

void GameScene::Update()
{
	myDayNightCycle.Update();
}

void GameScene::PerformAction(const std::string& anAction, Vector2i& aPosition)
{
	std::string symbolName = "";

	//SUN
	if (anAction == "o")
	{
		symbolName = "Sun";
		myDayNightCycle.SetTime(true);
	}
	//MOON
	else if (anAction == "c")
	{
		symbolName = "Moon";
		myDayNightCycle.SetTime(false);
	}
	//BOMB
	else if (anAction == "b")
	{
		Ray ray;
		auto cam = Engine::GetInstance()->GetActiveCamera();


		Vector2i mousePos = Engine::GetInstance()->ViewportToScreenPos(aPosition);
		float mouseX = mousePos.x / static_cast<float>(DX11::GetResolution().x);
		float mouseY = mousePos.y / static_cast<float>(DX11::GetResolution().y);

		auto matInv = Catbox::Matrix4x4<float>::GetFastInverse(Catbox::Matrix4x4<float>::GetFastInverse(cam->GetTransform()->GetWorldTransformMatrix()) * cam->GetProjectionMatrix());
		Vector4f rayOrigin = Vector4f(mouseX * 2 - 1, 1 - mouseY * 2, 0, 1) * matInv;
		Vector4f rayEnd = Vector4f(mouseX * 2 - 1, 1 - mouseY * 2, 1, 1) * matInv;

		rayOrigin /= rayOrigin.w;
		rayEnd /= rayEnd.w;

		Vector3f rayEnd2 = cam->MouseToWorldPos(Input::GetMousePosition());



		Vector4f rayDir4 = (rayEnd - rayOrigin).GetNormalized();
		Vector3f rayDir = Vector3f(rayDir4.x, rayDir4.y, rayDir4.z);


		ray.InitWithOriginAndDirection(cam->GetTransform()->worldPos(), rayDir);
		Vector3f intersectionOut;
		Collider* colliderHit = Engine::GetInstance()->GetCollisionManager()->RayIntersect(ray, 100, { 0 }, intersectionOut, true);
		if (colliderHit)
		{
			printmsg(colliderHit->GetGameObject().GetName());
		}

		auto bomb = InstantiatePrefab("Bomb");
		bomb->GetTransform()->SetWorldPos(cam->GetTransform()->worldPos() + rayDir * 3.f);
	}
	//SLASH
	else if (anAction == "-")
	{
		symbolName = "Slash";
	}

	if (!symbolName.empty())
	{
		PopupManager::CreatePopup("Symbol", 2, { 400,150 }, symbolName, { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 });
	}
	else
	{
		printmsg("Failed");
	}
}

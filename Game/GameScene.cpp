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

void GameScene::PerformAction(BrushSymbol& anAction)
{
	std::string symbolName = "";

	//SUN
	if (anAction.name == "o")
	{
		symbolName = "Sun";
		myDayNightCycle.SetTime(true);
	}
	//MOON
	else if (anAction.name == "c")
	{
		symbolName = "Moon";
		myDayNightCycle.SetTime(false);
	}
	//BOMB
	else if (anAction.name == "b")
	{
		Ray ray;
		auto cam = Engine::GetInstance()->GetActiveCamera();

		Vector2i center = Vector2i((anAction.minX + anAction.maxX) / 2, (anAction.minY + anAction.maxY) / 2);

		Vector4f rayEnd2 = cam->MouseToWorld(center, 1);
		Vector4f rayOrigin2 = cam->MouseToWorld(center, 0);
		Vector4f rayDir4 = (rayEnd2 - rayOrigin2).GetNormalized();
		Vector3f rayDir = Vector3f(rayDir4.x, rayDir4.y, rayDir4.z);


		ray.InitWithOriginAndDirection(cam->GetTransform()->worldPos(), rayDir);
		Vector3f intersectionOut;
		Collider* colliderHit = Engine::GetInstance()->GetCollisionManager()->RayIntersect(ray, 5, { 0 }, intersectionOut, true);
		if (colliderHit)
		{
			printmsg(colliderHit->GetGameObject().GetName());
		}

		auto bomb = InstantiatePrefab("Bomb");
		bomb->GetTransform()->SetWorldPos(Vector3f::up () + (cam->GetTransform()->worldPos() + rayDir * 5.f));
	}
	//SLASH
	else if (anAction.name == "-")
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

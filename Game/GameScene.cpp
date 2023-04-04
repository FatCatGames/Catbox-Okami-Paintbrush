#include "Game.pch.h"
#include "GameScene.h"
#include <PopupManager.h>
#include "Components/Physics/Collisions/CollisionManager.h"
#include "Components/Physics/Collisions/Collider.h"
#include "Tree.h"
#include "Player\PlayerController.h"

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

	auto cam = Engine::GetInstance()->GetActiveCamera();
	Vector2i center = Vector2i((anAction.minX + anAction.maxX) / 2, (anAction.minY + anAction.maxY) / 2);
	Vector2i symbolCenterScreen = Engine::GetInstance()->ScreenToViewportPos(center);

	Vector4f rayOrigin = cam->MouseToWorld(symbolCenterScreen, 1);
	Vector4f rayOrigin2 = cam->MouseToWorld(symbolCenterScreen, 0);
	Vector4f rayDir4 = (rayOrigin - rayOrigin2).GetNormalized();
	Vector3f rayDir = Vector3f(rayDir4.x, rayDir4.y, rayDir4.z);

	Ray ray;
	ray.InitWithOriginAndDirection(cam->GetTransform()->worldPos(), rayDir);
	Vector3f intersectionOut;
	Collider* colliderHit = Engine::GetInstance()->GetCollisionManager()->RayIntersect(ray, 20, { 0 }, intersectionOut, true);


	//SUN, LILYPAD
	if (anAction.name == "o")
	{
		if (anAction.target == BrushTarget::Sky)
		{
			symbolName = "Sun";
			myDayNightCycle.SetTime(true);
		}
		else if (anAction.target == BrushTarget::Water && colliderHit)
		{
			symbolName = "Water Lily";
			auto lily = InstantiatePrefab("LilyPad");
			Vector3f lilyOrigin = cam->MouseToWorldPos(symbolCenterScreen, colliderHit->GetTransform()->worldPos().y);
			lily->GetTransform()->SetWorldPos(lilyOrigin);
		}
	}
	//MOON
	else if (anAction.name == "c")
	{
		if (anAction.target == BrushTarget::Sky)
		{
			symbolName = "Moon";
			myDayNightCycle.SetTime(false);
		}
	}
	//BOMB
	else if (anAction.name == "b")
	{
		symbolName = "Bomb";
		auto bomb = InstantiatePrefab("Bomb");
		bomb->GetTransform()->SetWorldPos(Vector3f::up() + (cam->GetTransform()->worldPos() + rayDir * 5.f));
	}
	//SLASH
	else if (anAction.name == "-")
	{
		symbolName = "Slash";

		Ray ray;

		const unsigned int treeLayer = Engine::GetInstance()->GetCollisionManager()->GetLayerByName("Tree");

		std::unordered_map<int, GameObject*> hitObjects;

		bool success = false;
		for (size_t x = anAction.minX; x < anAction.maxX; x += 10)
		{
			Vector4f rayEnd2 = cam->MouseToWorld(Vector2i(x, center.y), 1);
			Vector4f rayOrigin2 = cam->MouseToWorld(Vector2i(x, center.y), 0);
			Vector4f rayDir4 = (rayEnd2 - rayOrigin2).GetNormalized();
			Vector3f rayDir = Vector3f(rayDir4.x, rayDir4.y, rayDir4.z);


			ray.InitWithOriginAndDirection(cam->GetTransform()->worldPos(), rayDir);
			Vector3f intersectionOut;
			Collider* colliderHit = Engine::GetInstance()->GetCollisionManager()->RayIntersect(ray, 10, { treeLayer }, intersectionOut, true);
			if (colliderHit)
			{
				success = true;
				auto& gObj = colliderHit->GetGameObject();
				if (hitObjects.find(gObj.GetObjectInstanceID()) == hitObjects.end())
				{
					hitObjects.insert({ gObj.GetObjectInstanceID(), &gObj });
					colliderHit->GetGameObject().GetComponent<Tree>()->Slash(intersectionOut, rayDir);

					auto slash = InstantiatePrefab("Slash");
					slash->GetTransform()->SetWorldPos(colliderHit->GetTransform()->worldPos() + Vector3f::up() * 1.5f - rayDir * 0.5f);
				}

			}

			if (success) 
			{
				GameManager::GetInstance()->GetPlayer()->GetGameObject()->GetComponent<PlayerController>()->ResetActionTimer();
			}
		}
	}

	if (!symbolName.empty())
	{
		PopupManager::CreatePopup("Symbol", 1, { 400,150 }, symbolName, { Engine::GetInstance()->GetWindowSize().x * 0.5f, 400 });
	}
	else
	{
		printmsg("Failed");
	}
}

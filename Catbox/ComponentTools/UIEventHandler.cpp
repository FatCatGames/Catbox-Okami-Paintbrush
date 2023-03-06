#include "stdafx.h"
#include "UIEventHandler.h"
#include "Components\UI\UIElement.h"
#include "CommonUtilities\InputHandler.h"
#include "GameObjects\GameObjectSelection.h"
#include "SceneManager.h"

std::vector<UIElement*> UIEventHandler::myUIElements;
std::string UIEventHandler::myNextLevel;
bool UIEventHandler::myShouldChangeLevel;

void UIEventHandler::Update()
{
	if (myShouldChangeLevel)
	{
		myShouldChangeLevel = false;
		Engine::GetInstance()->GetSceneManager()->LoadScene(AssetRegistry::GetInstance()->GetAsset<Scene>(myNextLevel));
		return;
	}
	GameObject* hoveredObj = GameObjectSelection::GetHoveredObject();
	if (hoveredObj)
	{
		for (size_t i = 0; i < myUIElements.size(); i++)
		{
			if (&myUIElements[i]->GetGameObject() == hoveredObj)
			{
				myUIElements[i]->SetIsHovered(true);
				myUIElements[i]->OnMouseOver();
				if (Input::GetKeyPress(KeyCode::MOUSELEFT))
				{
					myUIElements[i]->OnClick();
				}
				break;
			}
			else myUIElements[i]->SetIsHovered(false);
		}
	}
	else 
	{
		for (size_t i = 0; i < myUIElements.size(); i++)
		{
			myUIElements[i]->SetIsHovered(false);
		}
	}
}

void UIEventHandler::SetNextLevel(std::string aNextLevel)
{
	myNextLevel = aNextLevel;
	myShouldChangeLevel = true;
}

void UIEventHandler::ResizeUIElements()
{
	for (size_t i = 0; i < myUIElements.size(); i++)
	{
		myUIElements[i]->OnTransformChanged();
	}
}

void UIEventHandler::Init()
{
	//DX11::AddResolutionChangedListener([] {ResizeUIElements(); });
	myShouldChangeLevel = false;
}

void UIEventHandler::RemoveUIElement(UIElement* aUIElement)
{
	for (size_t i = 0; i < myUIElements.size(); i++)
	{
		if (myUIElements[i] == aUIElement)
		{
			myUIElements.erase(myUIElements.begin() + i);
			break;
		}
	}
}
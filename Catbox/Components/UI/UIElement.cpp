#include "stdafx.h"
#include "UIElement.h"
#include "ComponentTools\UIEventHandler.h"

void UIElement::Init() 
{
	myUIRenderData.id = myGameObject->GetObjectInstanceID();
	UIEventHandler::AddUIElement(this);
	OnTransformChanged();
}

UIElement::~UIElement() 
{
	UIEventHandler::RemoveUIElement(this);
}

void UIElement::Update()
{
	Render();
}

void UIElement::SetIsHovered(bool aIsHovered)
{

	if (myIsHovered != aIsHovered) 
	{
		if (myIsHovered) OnMouseExit();
		else OnMouseEnter();
	}
	myIsHovered = aIsHovered;
}

void UIElement::RunInEditor()
{
	Render();
}

void UIElement::OnTransformChanged()
{
	auto pos = myTransform->worldPos();
	auto scale = myTransform->worldScale();

	//float multiplierX = myReferenceResolution.x / static_cast<float>(DX11::GetResolution().x);
	//float multiplierY = myReferenceResolution.y / static_cast<float>(DX11::GetResolution().y);
	myUIRenderData.position = { pos.x , pos.y };
	myUIRenderData.scale = { scale.x / static_cast<float>(myReferenceResolution.x) * 2.f , scale.y / static_cast<float>(myReferenceResolution.y) * 2.f };
}
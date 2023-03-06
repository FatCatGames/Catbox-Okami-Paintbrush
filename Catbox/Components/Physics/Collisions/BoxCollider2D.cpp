#include "stdafx.h"
#include "BoxCollider2D.h"

BoxCollider2D::BoxCollider2D()
{
	myColliderType = ColliderType::Box2D;
	myWidth = 0;
	myHeight = 0;
	myOffset = { 0,0 };
}

void BoxCollider2D::SetInfo(const float& aWidth, const float& aHeight, const Vector2f& anOffset)
{
	myWidth = aWidth;
	myHeight = aHeight;
	myOffset = anOffset;
}

bool BoxCollider2D::IsInside(const Vector2f& aPos) const
{
	Vector2f myPos = GetPosition();
	Vector2f mySize = GetSize();

	if (aPos.x < (myPos.x - mySize.x / 2.0f)) return false;
	if (aPos.x > (myPos.x + mySize.x / 2.0f)) return false;
	if (aPos.y < (myPos.y - mySize.y / 2.0f)) return false;
	if (aPos.y > (myPos.y + mySize.y / 2.0f)) return false;
	return true;
}

bool BoxCollider2D::IsInside(const Vector3f& aPoint) const
{
	return IsInside(Vector2f(aPoint.x, aPoint.y));
}

const Vector2f BoxCollider2D::GetSize() const
{
	Vector3f worldScale = myGameObject->GetTransform()->worldScale();
	return Vector2f(abs(myWidth * worldScale.x), abs(myHeight * worldScale.y));
}

const Vector2f BoxCollider2D::GetPosition() const
{
	Vector3f pos = myTransform->worldPos();
	return { pos.x + myOffset.x, pos.y + myOffset.y };
	/*Vector3f right = myTransform->right();
	Vector3f up = myTransform->up();
	pos += right * myOffset.x;
	pos += up * myOffset.y;
	std::string name = myGameObject->GetName();
	if (name == "PlayerCharacter") {
		std::cout << "player up (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
	}
	return { pos.x, pos.y };*/
}

const Vector2f BoxCollider2D::GetMinCorner() const
{
	return GetPosition() - GetSize()/2.f;
}

const Vector2f BoxCollider2D::GetMaxCorner() const
{
	return GetPosition() + GetSize() / 2.f;
}

void BoxCollider2D::DebugDraw()
{
	//Engine::GetInstance()->GetDebugDrawer()->DrawBox(GetMinCorner(), GetMaxCorner());
}

void BoxCollider2D::Save(rapidjson::Value& /*aComponentData*/)
{
	print("BoxCollider2D component saving not implemented.");
}

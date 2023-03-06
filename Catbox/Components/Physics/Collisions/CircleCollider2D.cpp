#include "stdafx.h"
#include "CircleCollider2D.h"


CircleCollider2D::CircleCollider2D()
{
	myRadius = 0;
	myOffset = { 0,0 };
	myColliderType = ColliderType::Circle2D;
}

void CircleCollider2D::SetInfo(const float& aRadius, const Vector2f& anOffset)
{
	myRadius = aRadius;
	myOffset = anOffset;
}


void CircleCollider2D::SetRadius(float aRadius)
{
	myRadius = aRadius;
}

const float CircleCollider2D::GetRadius() const
{
	Vector3f worldScale = myTransform->worldScale();
	return myRadius * ((worldScale.x + worldScale.y) / 2.f);
}

const Vector2f CircleCollider2D::GetPosition() const
{
	Vector3f pos = myTransform->worldPos();
	/*pos += myTransform->right() * myOffset.x;
	pos += myTransform->up() * myOffset.y;
	return { pos.x, pos.y};*/
	return { pos.x + myOffset.x, pos.y + myOffset.y };
}

void CircleCollider2D::DebugDraw()
{
	//Engine::GetInstance()->GetDebugDrawer()->DrawCircle(GetPosition(), GetRadius());
}

bool CircleCollider2D::IsInside(const Vector2f& aPoint) const
{
	Vector2f dist = aPoint - GetPosition();
	return dist.LengthSqr() <= myRadius * myRadius;
}

bool CircleCollider2D::IsInside(const Vector3f& aPoint) const
{
	return IsInside(Vector2f(aPoint.x, aPoint.y));
}

void CircleCollider2D::Save(rapidjson::Value& /*aComponentData*/)
{
	print("CircleCollider2D component saving not implemented.");
}

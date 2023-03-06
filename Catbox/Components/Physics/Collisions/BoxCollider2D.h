#pragma once
#include "Collider.h"

class BoxCollider2D : public Collider
{
public:
	BoxCollider2D();
	~BoxCollider2D() = default;
	void SetInfo(const float& aWidth, const float& aHeight, const Vector2f& anOffset = { 0,0 });
	bool IsInside(const Vector2f& aPoint) const;
	bool IsInside(const Vector3f& aPoint) const override;
	const Vector2f GetSize() const;
	const Vector2f GetPosition() const;
	const Vector2f GetMinCorner() const;
	const Vector2f GetMaxCorner() const;
	void DebugDraw() override;

private:
	void Save(rapidjson::Value& aComponentData) override;
	float myWidth;
	float myHeight;
	Vector2f myOffset;
};
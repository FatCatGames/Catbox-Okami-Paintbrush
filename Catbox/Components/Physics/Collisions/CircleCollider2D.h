#pragma once
#include "Collider.h"

class CircleCollider2D : public Collider
{
public:
	CircleCollider2D();
	~CircleCollider2D() = default;
	void SetInfo(const float& aRadius, const Vector2f& anOffset = { 0,0 });
	void SetRadius(float aRadius);
	const float GetRadius() const;
	const Vector2f GetPosition() const;
	void DebugDraw() override;
	bool IsInside(const Vector2f& aPoint) const;
	bool IsInside(const Vector3f& aPoint) const override;

private:
	void Save(rapidjson::Value& aComponentData) override;
	float myRadius;
	Vector2f myOffset;
};
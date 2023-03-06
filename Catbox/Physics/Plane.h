#pragma once

class Plane
{
public:
	Plane() = default;

	void InitWith3Points(const Vector3f& aPoint0, const Vector3f& aPoint1, const Vector3f& aPoint2);
	void InitWithPointAndNormal(const Vector3f& aPoint, const Vector3f& aNormal);
	
	bool IsInside(const Vector3f& aPosition) const;

	// Returns the normal of the plane.
	const Vector3f& GetNormal() const;
	const Vector3f& GetPoint() const;

private:
	Vector3f myNormal;
	Vector3f myPoint;
};
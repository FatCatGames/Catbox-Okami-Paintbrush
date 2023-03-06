#pragma once

class Ray
{
public:
	Ray();
	Ray(const Vector3f& aOrigin, const Vector3f& aPoint);
	void InitWith2Points(const Vector3f& aOrigin, const Vector3f& aPoint);

	void InitWithOriginAndDirection(const Vector3f& aOrigin, const Vector3f& aDirection);

	const Vector3f& GetOrigin() const;
	const Vector3f& GetDirection() const;

private:
	Vector3f myOrigin;
	Vector3f myDirection;
};
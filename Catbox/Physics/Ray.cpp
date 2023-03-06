#include "stdafx.h"
#include "Ray.h"

Ray::Ray()
{
	myOrigin = { 0,0,0 };
	myDirection = { 0,0,0 };
}

Ray::Ray(const Vector3f& aOrigin, const Vector3f& aPoint)
{
	InitWith2Points(aOrigin, aPoint);
}
void Ray::InitWith2Points(const Vector3f& aOrigin, const Vector3f& aPoint)
{
	myOrigin = aOrigin;
	myDirection = (aPoint - aOrigin).GetNormalized();
}
void Ray::InitWithOriginAndDirection(const Vector3f& aOrigin, const Vector3f& aDirection)
{
	myOrigin = aOrigin;
	myDirection = aDirection.GetNormalized();
}
const Vector3f& Ray::GetOrigin() const
{
	return myOrigin;
}
const Vector3f& Ray::GetDirection() const
{
	return myDirection;
}

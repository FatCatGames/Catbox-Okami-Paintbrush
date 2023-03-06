#include "stdafx.h"
#include "Plane.h"

void Plane::InitWith3Points(const Vector3f& aPoint0, const Vector3f& aPoint1, const Vector3f& aPoint2)
{
	myNormal = (aPoint1 - aPoint0).Cross((aPoint2 - aPoint0));
	myPoint = aPoint0;
}


void Plane::InitWithPointAndNormal(const Vector3f& aPoint, const Vector3f& aNormal)
{
	myPoint = aPoint;
	myNormal = aNormal;
}


bool Plane::IsInside(const Vector3f& aPosition) const
{
	Vector3f diff = aPosition - myPoint;
	if (myNormal.Dot(diff) < 0)
	{
		return true;
	}
	return false;
}


const Vector3f& Plane::GetNormal() const
{
	return myNormal;
}


const Vector3f& Plane::GetPoint() const
{
	return myPoint;
}
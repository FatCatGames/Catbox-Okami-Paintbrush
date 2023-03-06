#pragma once
#include "Vector3.hpp"																						

namespace Catbox
{

	struct Sphere
	{
	public:

		Sphere() = default;
		Sphere(const Vector3f& aCenter, float aRadius)
		{
			position = aCenter;
			radius = aRadius;
		}

		void InitWithCenterAndRadius(const Vector3f& aCenter, float aRadius)
		{
			position = aCenter;
			radius = aRadius;
		}

		bool IsInside(const Vector3f& aPosition) const
		{
			float dist = (position - position).LengthSqr();
			return dist <= (radius * radius);
		}


		float radius = 0;
		Vector3f position = { 0,0,0 };
	};
}
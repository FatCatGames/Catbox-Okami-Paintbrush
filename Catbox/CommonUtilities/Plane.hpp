#pragma once
#include "Vector3.hpp"

namespace Catbox {
	template<typename T>
	class Plane {
	public:
		Plane() = default;
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal);
		void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
		void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal);
		void SetDistance(float aDistance);
		bool IsInside(const Vector3<T>& aPosition) const;
		bool IsSphereInside(const Vector3<T>& aPosition, const float aRadius) const;
		bool IsSphereInFront(const Vector3<T>& aPosition, const float aRadius) const;
		const Vector3<T>& GetNormal() const;

	private:
		Vector3<T> myPoint;
		Vector3<T> myNormal;
		float myDistanceToOrigo;
	};
	

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		myPoint = aPoint0;
		myNormal = (aPoint1 - aPoint0).Cross((aPoint2 - aPoint0));
	}

	template<typename T>
	inline Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal)
	{
		myPoint = aPoint0;
		myNormal = aNormal;
	}

	template<typename T>
	inline void Plane<T>::InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		myPoint = aPoint0;
		myNormal = (aPoint1 - aPoint0).Cross((aPoint2 - aPoint0));
		myNormal = myNormal.GetNormalized();
	}

	template<typename T>
	inline void Plane<T>::InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
	{
		myPoint = aPoint;
		myNormal = aNormal;
	}

	template<typename T>
	inline void Plane<T>::SetDistance(float aDistance)
	{
		myDistanceToOrigo = aDistance;
	}

	template<typename T>
	inline bool Plane<T>::IsInside(const Vector3<T>& aPosition) const
	{
		return myNormal.Dot(aPosition - myPoint) <= 0;
	}
	template<typename T>
	inline bool Plane<T>::IsSphereInside(const Vector3<T>& aPosition, const float aRadius) const
	{
		return myNormal.Dot(aPosition - myPoint) <= aRadius;
	}
	template<typename T>
	inline bool Plane<T>::IsSphereInFront(const Vector3<T>& aPosition, const float aRadius) const
	{
		T aNumber = myNormal.Dot(aPosition - myPoint) + aRadius;
		if (aNumber > 0) 
		{
			return true;
		}
		else 
		{
			return false;
		}
	}
	template<typename T>
	inline const Vector3<T>& Plane<T>::GetNormal() const
	{
		return myNormal;
	}
}
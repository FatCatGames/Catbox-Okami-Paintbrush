#pragma once
#include "Vector3.hpp"
namespace Catbox
{
	template<typename T>
	class Ray
	{
	public:
		//Default constructor: There is no ray, the origin and direction area the zero vector
		Ray();

		//Copy contructor
		Ray(const Ray<T>& aRay);

		//Constructor that takes two points that define the ray, the direction is 
		//aPoint - aOrigin and and the origin is aOrigin
		Ray(const Vector3<T>& aOrigin, const Vector3<T>& aPoint );

		//Init with two points, the same as the constructor above
		void InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);

		//Init the ray with an origin and a direction
		void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);

		const Vector3<T>& GetOrigin() const;
		const Vector3<T>& GetDirection() const;

	private:
		Vector3<T> myOrigin;
		Vector3<T> myDirection;
	};

	template<typename T>
	inline Ray<T>::Ray()
	{
		myOrigin = { 0,0 };
		myDirection = { 0,0 };
	}
	template<typename T>
	inline Ray<T>::Ray(const Ray<T>& aRay)
	{
		myOrigin = aRay.myOrigin;
		myDirection = aRay.myDirection;
	}
	template<typename T>
	inline Ray<T>::Ray(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		InitWith2Points(aOrigin, aPoint);
	}
	template<typename T>
	inline void Ray<T>::InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		myOrigin = aOrigin;
		myDirection = (aPoint - aOrigin).GetNormalized();
	}
	template<typename T>
	inline void Ray<T>::InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection.GetNormalized();
	}
	template<typename T>
	inline const Vector3<T>& Ray<T>::GetOrigin() const
	{
		return myOrigin;
	}
	template<typename T>
	inline const Vector3<T>& Ray<T>::GetDirection() const
	{
		return myDirection;
	}
}
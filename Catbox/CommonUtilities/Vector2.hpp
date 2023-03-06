#ifndef Vector2_H
#define Vector2_H

#include <math.h>
#include <cassert>

namespace Catbox
{
	template<typename T>
	class Vector2
	{
	public:
		T x;
		T y;
		Vector2<T>();
		Vector2<T>(const T& aX, const T& aY);
		Vector2<T>(const Vector2<T>& aVector) = default;
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;
		~Vector2<T>() = default;

		T LengthSqr() const;
		T Length() const;
		Vector2<T> GetNormalized() const;
		void Normalize();
		T Dot(const Vector2<T>& aVector) const;

		inline Vector2<T> right() { return Vector2<T>(1, 0); }
		inline Vector2<T> left() { return Vector2<T>(-1, 0); }
		inline Vector2<T> up() { return Vector2<T>(0, 1); }
		inline Vector2<T> down() { return Vector2<T>(0, -1); }
		inline Vector2<T> zero() { return Vector2<T>(0, 0); }
		inline Vector2<T> one() { return Vector2<T>(1, 1); }
	};

	template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
	}

	template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y);
	}

	template <class T> Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y);
	}
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar) 
	{
		return Vector2<T>(aVector.x / aScalar, aVector.y / aScalar);
	}
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector0, const Vector2 <T>& aVector1)
	{
		return Vector2<T>(aVector0.x / aVector1.x, aVector0.y / aVector1.y);
	}
	template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
	}
	template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
	}
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}
	template <class T> void operator*=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x *= aVector1.x;
		aVector0.y *= aVector1.y;
	}
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
	}
	template <class T> void operator/=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x /= aVector1.x;
		aVector0.y /= aVector1.y;
	}


	template<typename T>
	inline Vector2<T>::Vector2()
	{
		x = 0;
		y = 0;
	}
	template<typename T>
	inline Vector2<T>::Vector2(const T& aX, const T& aY)
	{
		x = aX;
		y = aY;
	}
	template<typename T>
	inline T Vector2<T>::LengthSqr() const
	{
		return x * x + y * y;
	}
	template<typename T>
	inline T Vector2<T>::Length() const
	{
		return sqrt(x*x + y*y);
	}
	template<typename T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		T length = Length();
		assert(length != 0 && "Tried to normalize a vector where length was 0!");
		return Vector2<T>(x / length, y / length);
	}
	template<typename T>
	inline void Vector2<T>::Normalize()
	{
		T length = Length();
		assert(length != 0 && "Tried to normalize a vector where length was 0!");
		x = x / length;
		y = y / length;
	}
	template<typename T>
	inline T Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y;
	}
};
#endif
#pragma once
#ifndef Vector4_H
#define Vector4_H

#include <math.h>
#include <cassert>
#include "Color.h"
#include "Vector3.hpp"

namespace Catbox
{
	template<typename T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;

		Vector4<T>();
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);
		Vector4<T>(const Vector4<T>& aVector) = default;
		Vector4<T>(const Color& aColor);
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;
		T& operator[](int anIndex);
		const T& operator[](int anIndex) const;
		~Vector4<T>() = default;

		T LengthSqr() const;
		T Length() const;
		Vector4<T> GetNormalized() const;
		void Normalize();
		T Dot(const Vector4<T>& aVector) const;
		Vector3<T> GetVector3() const;

		static Vector4<T> right() { return Vector4<T>(1, 0, 0, 0); }
		static Vector4<T> left() { return Vector4<T>(-1, 0, 0, 0); }
		static Vector4<T> up() { return Vector4<T>(0, 1, 0, 0); }
		static Vector4<T> down() { return Vector4<T>(0, -1, 0, 0); }
		static Vector4<T> forward() { return Vector4<T>(0, 0, 1, 0); }
		static Vector4<T> back() { return Vector4<T>(0, 0, -1, 0); }
		static Vector4<T> zero() { return Vector4<T>(0, 0, 0, 0); }
		static Vector4<T> one() { return Vector4<T>(1, 1, 1, 1); }
	};

	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w);
	}

	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z, aVector0.w - aVector1.w);
	}

	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar, aVector.w / aScalar);
	}

	template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
		aVector0.w += aVector1.w;
	}
	template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
		aVector0.w -= aVector1.w;
	}
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
		aVector.w /= aScalar;
	}

	template<typename T>
	inline Vector4<T>::Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	template<typename T>
	inline Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template<typename T>
	inline Vector4<T>::Vector4(const Color& aColor)
	{
		x = aColor.r;
		y = aColor.g;
		z = aColor.b;
		w = aColor.a;
	}

	template<typename T>
	inline T& Vector4<T>::operator[](int anIndex)
	{
		if (anIndex == 0) return x;
		if (anIndex == 1) return y;
		if (anIndex == 2) return z;
		if (anIndex == 3) return w;
		assert(true && "Index was out of range");
	}

	template<typename T>
	inline const T& Vector4<T>::operator[](int anIndex) const
	{
		if (anIndex == 0) return x;
		if (anIndex == 1) return y;
		if (anIndex == 2) return z;
		if (anIndex == 3) return w;
		assert(true && "Index was out of range");
		return x;
	}

	template<typename T>
	inline T Vector4<T>::LengthSqr() const
	{
		return x * x + y * y + z * z + w * w;
	}
	template<typename T>
	inline T Vector4<T>::Length() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}
	template<typename T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		T length = Length();
		assert(length != 0 && "Tried to normalize a vector where length was 0!");
		return Vector4<T>(x / length, y / length, z / length, w / length);
	}
	template<typename T>
	inline void Vector4<T>::Normalize()
	{
		T length = Length();
		assert(length != 0 && "Tried to normalize a vector where length was 0!");
		x = x / length;
		y = y / length;
		z = z / length;
		w = w / length;
	}
	template<typename T>
	inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
	}
	template<typename T>
	inline Vector3<T> Vector4<T>::GetVector3() const
	{
		Vector3<T> returnVector;
		returnVector.x = x;
		returnVector.y = y;
		returnVector.z = z;
		return returnVector;
	}
};
#endif
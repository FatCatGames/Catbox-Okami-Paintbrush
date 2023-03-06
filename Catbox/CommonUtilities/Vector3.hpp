#pragma once
#include <math.h>
#include <cassert>
#include <string>

namespace Catbox
{
	template<typename T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		Vector3<T>();
		Vector3<T>(const T& aX, const T& aY, const T& aZ);
		Vector3<T>(const Vector3<T>& aVector) = default;
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
		bool operator==(const Vector3<T>& aVector3) const;
		bool operator!=(const Vector3<T>& aVector3) const;
		~Vector3<T>() = default;

		T LengthSqr() const;
		T Length() const;
		Vector3<T> GetNormalized() const;
		void Normalize();
		T Dot(const Vector3<T>& aVector) const;
		Vector3<T> Cross(const Vector3<T>& aVector) const;
		float Distance(const Vector3<T>& aVector) const;
		static Vector3<float> GetPointFromAngleHorizontal(Vector3<float> anOrigin, Vector3<float> aForwardVector, float anAngle, float aLength);
		static Vector3<float> GetPointFromAngleVertical(Vector3<float> anOrigin, Vector3<float> aForwardVector, float anAngle, float aLength);

		static Vector3<T> right() { return Vector3<T>(1, 0, 0); }
		static Vector3<T> left() { return Vector3<T>(-1, 0, 0); }
		static Vector3<T> up() { return Vector3<T>(0, 1, 0); }
		static Vector3<T> down() { return Vector3<T>(0, -1, 0); }
		static Vector3<T> forward() { return Vector3<T>(0, 0, 1); }
		static Vector3<T> back() { return Vector3<T>(0, 0, -1); }
		static Vector3<T> zero() { return Vector3<T>(0, 0, 0); }
		static Vector3<T> one() { return Vector3<T>(1, 1, 1); }

		std::string ToString();
	};

	template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
	}

	template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
	}

	template <class T> Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z);
	}

	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar);
	}

	template <class T> Vector3<T> operator/(const Vector3<T>& aVector0, const Vector3 <T>& aVector1)
	{
		return Vector3<T>(aVector0.x / aVector1.x, aVector0.y / aVector1.y, aVector0.z / aVector1.z);
	}

	template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
	}

	template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
	}

	template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}

	template <class T> void operator*=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x *= aVector1.x;
		aVector0.y *= aVector1.y;
		aVector0.z *= aVector1.z;
	}

	template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;		aVector.y /= aScalar;
		aVector.z /= aScalar;
	}

	template <class T> void operator/=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x /= aVector1.x;
		aVector0.y /= aVector1.y;
		aVector0.z /= aVector1.z;
	}

	template<typename T>
	inline Vector3<T>::Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	template<typename T>
	inline Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	}

	template<typename T>
	inline bool Vector3<T>::operator==(const Vector3<T>& aVector3) const
	{
		return x == aVector3.x && y == aVector3.y && z == aVector3.z;
	}

	template<typename T>
	inline bool Vector3<T>::operator!=(const Vector3<T>& aVector3) const
	{
		return x != aVector3.x && y != aVector3.y && z != aVector3.z;
	}

	template<typename T>
	inline T Vector3<T>::LengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	template<typename T>
	inline T Vector3<T>::Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
	{
		T length = Length();
		if (length == 0) return Vector3<T>::zero();
		return Vector3<T>(x / length, y / length, z / length);
	}

	template<typename T>
	inline void Vector3<T>::Normalize()
	{
		T length = Length();
		if (length == 0) return;
		x = x / length;
		y = y / length;
		z = z / length;
	}

	template<typename T>
	inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z;
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		return Vector3<T>((y * aVector.z) - (aVector.y * z), (z * aVector.x) - (aVector.z * x), (x * aVector.y) - (aVector.x * y));
	}

	template<typename T>
	inline float Vector3<T>::Distance(const Vector3<T>& aVector) const
	{
		return float(std::sqrt(std::powf((x - aVector.x), 2) + std::powf((y - aVector.y), 2) + std::powf((z - aVector.z), 2)));
	}

	template<typename T>
	inline Vector3<float> Vector3<T>::GetPointFromAngleHorizontal(Vector3<float> anOrigin, Vector3<float> aForwardVector, float anAngle, float aLength)
	{
		float angleRad = Deg2Rad(anAngle);
		float rotX = aForwardVector.x * std::cos(angleRad) - aForwardVector.z * std::sin(angleRad);
		float rotZ = aForwardVector.x * std::sin(angleRad) + aForwardVector.z * std::cos(angleRad);

		rotX *= aLength;
		rotZ *= aLength;

		return { anOrigin.x + rotX, anOrigin.y, anOrigin.z + rotZ };
	}

	template<typename T>
	inline Vector3<float> Vector3<T>::GetPointFromAngleVertical(Vector3<float> anOrigin, Vector3<float> aForwardVector, float anAngle, float aLength)
	{
		float angleRad = Deg2Rad(anAngle);
		float rotX = aForwardVector.x * std::cos(angleRad) - aForwardVector.y * std::sin(angleRad);
		float rotY = aForwardVector.x * std::sin(angleRad) + aForwardVector.y * std::cos(angleRad);

		rotX *= aLength;
		rotY *= aLength;

		return { anOrigin.x + rotX, anOrigin.y + rotY, anOrigin.z };
	}
	template<typename T>
	inline std::string Vector3<T>::ToString()
	{
		std::string s = (std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z));
		return s;
	}
};
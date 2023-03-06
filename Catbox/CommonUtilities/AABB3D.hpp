#pragma once

namespace Catbox
{

	template<typename T>
	class AABB3D
	{
	public:
		// Default constructor: there is no AABB, both min and max points are the zero vector.
		AABB3D();
		// Copy constructor.
		AABB3D(const AABB3D<T>& aAABB3D);
		// Constructor taking the positions of the minimum and maximum corners.
		AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax);
		// Init the AABB with the positions of the minimum and maximum corners, same as
		// the constructor above.
		void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax);
		// Returns whether a point is inside the AABB: it is inside when the point is on any
		// of the AABB's sides or inside of the AABB.
		bool IsInside(const Vector3<T>& aPosition) const;

		const Catbox::Vector3<T>& GetMinCorner() const;
		const Catbox::Vector3<T>& GetMaxCorner() const;
	private:
		Catbox::Vector3<T> myMinCorner;
		Catbox::Vector3<T> myMaxCorner;

	};
	template<typename T>
	inline AABB3D<T>::AABB3D()
	{
		myMinCorner = { 0,0,0 };
		myMaxCorner = { 0,0,0 };
	}

	template<typename T>
	inline AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D)
	{
		myMinCorner = aAABB3D.myMinCorner;
		myMaxCorner = aAABB3D.myMaxCorner;
	}

	template<typename T>
	inline AABB3D<T>::AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		myMinCorner = aMin;
		myMaxCorner = aMax;
	}

	template<typename T>
	inline void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		myMinCorner = aMin;
		myMaxCorner = aMax;
	}

	template<typename T>
	inline bool AABB3D<T>::IsInside(const Vector3<T>& aPosition) const
	{
		float epsilon = 0.0001f;
		if (aPosition.x < myMinCorner.x - epsilon) return false;
		if (aPosition.x > myMaxCorner.x + epsilon) return false;
		if (aPosition.y < myMinCorner.y - epsilon) return false;
		if (aPosition.y > myMaxCorner.y + epsilon) return false;
		if (aPosition.z < myMinCorner.z - epsilon) return false;
		if (aPosition.z > myMaxCorner.z + epsilon) return false;
		
		return true;
	}
	template<typename T>
	inline const Catbox::Vector3<T>& AABB3D<T>::GetMinCorner() const
	{
		return myMinCorner;
	}
	template<typename T>
	inline const Catbox::Vector3<T>& AABB3D<T>::GetMaxCorner() const
	{
		return myMaxCorner;
	}
}
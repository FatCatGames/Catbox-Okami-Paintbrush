#pragma once
#include "Vector2.hpp"
namespace Catbox
{
	template<typename T>
	class Line
	{
	public:
		Line() = default;
		Line(const Line<T>& aLine);
		Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
		void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
		void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection);
		bool IsInside(const Vector2<T>& aPosition) const;
		const Vector2<T>& GetDirection() const;
		const Vector2<T>& GetNormal() const;
		
	private:
		Vector2<T> myDirection;
		Vector2<T> myPoint;
		Vector2<T> myNormal;
	};

	template<typename T>
	inline Line<T>::Line(const Line<T>& aLine)
	{
		myDirection = aLine.myDirection;
		myPoint = aLine.myPoint;
		myNormal = { -myDirection.y, myDirection.x };
	}

	template<typename T>
	inline Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		InitWith2Points(aPoint0, aPoint1);
	}

	template<typename T>
	inline void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint = aPoint0;
		myDirection = aPoint1 - aPoint0;
		myNormal = { -myDirection.y, myDirection.x };
	}

	template<typename T>
	inline void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
	{
		myPoint = aPoint;
		myDirection = aDirection;
		myNormal = { -myDirection.y, myDirection.x };
	}

	template<typename T>
	inline bool Line<T>::IsInside(const Vector2<T>& aPosition) const
	{
		return GetNormal().Dot(aPosition - myPoint) <= 0;
	}

	template<typename T>
	inline const Vector2<T>& Line<T>::GetDirection() const
	{
		return myDirection;
	}
	template<typename T>
	inline const Vector2<T>& Line<T>::GetNormal() const
	{
		return myNormal;
	}
}
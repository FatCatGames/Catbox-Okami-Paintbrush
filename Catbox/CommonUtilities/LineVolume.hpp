#pragma once
#include <vector>
#include "Line.hpp"
namespace Catbox 
{
	template<typename T>
	class LineVolume 
	{
	public:
		LineVolume() = default;
		LineVolume(const std::vector<Line<T>>& aLineList);
		void AddLine(const Line<T>& aLine);
		bool IsInside(const Vector2<T>& aPosition);
	private:
		std::vector<Line<T>> myLineList;
	};
	template<typename T>
	inline LineVolume<T>::LineVolume(const std::vector<Line<T>>& aLineList)
	{
		myLineList = aLineList;
	}

	template<typename T>
	inline void LineVolume<T>::AddLine(const Line<T>& aLine)
	{
		myLineList.push_back(aLine);
	}

	template<typename T>
	inline bool LineVolume<T>::IsInside(const Vector2<T>& aPosition)
	{
		for (size_t i = 0; i < myLineList.size(); i++)
		{
			if (!myLineList[i].IsInside(aPosition)) 
			{
				return false;
			}
		}
		return true;
	}
}
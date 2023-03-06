#pragma once
#pragma once
#include <vector>
#include <cassert>

namespace Catbox
{
	template <class T>
	class Queue
	{
	public:
		Queue();
		int GetSize() const;
		const T& GetFront() const;
		T& GetFront();
		void Enqueue(const T& aValue);
		T Dequeue();
		inline bool IsEmpty() { return myVector.empty(); }

	private:
		std::vector<T> myVector;
		int mySize;
	};

	template<class T>
	inline Queue<T>::Queue()
	{
		mySize = 0;
	}

	template<class T>
	inline int Queue<T>::GetSize() const
	{
		return mySize;
	}

	template<class T>
	inline const T& Queue<T>::GetFront() const
	{
		assert(mySize > 0 && "Queue is empty");
		return myVector.front();
	}

	template<class T>
	inline T& Queue<T>::GetFront()
	{
		assert(mySize > 0 && "Queue is empty");
		return myVector.front();
	}

	template<class T>
	inline void Queue<T>::Enqueue(const T& aValue)
	{
		myVector.push_back(aValue);
		++mySize;
	}

	template<class T>
	inline T Queue<T>::Dequeue()
	{
		assert(mySize > 0 && "Queue is empty");
		T element = myVector.front();
			myVector.erase(myVector.begin());
		--mySize;
		return element;
	}
}
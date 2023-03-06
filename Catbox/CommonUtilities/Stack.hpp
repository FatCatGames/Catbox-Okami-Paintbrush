#pragma once
#include <vector>
#include <cassert>

namespace Catbox
{
	template <class T>
	class Stack
	{
	public:
		Stack();
		int GetSize() const;
		const T& GetTop() const;
		T& GetTop();
		void Push(const T& aValue);
		T Pop();

	private:
		std::vector<T> myVector;
		int mySize;
	};

	template<class T>
	inline Stack<T>::Stack()
	{
		mySize = 0;
	}

	template<class T>
	inline int Stack<T>::GetSize() const
	{
		return mySize;
	}

	template<class T>
	inline const T& Stack<T>::GetTop() const
	{
		assert(mySize > 0 && "Stack is empty");
		return myVector.back();
	}

	template<class T>
	inline T& Stack<T>::GetTop()
	{
		assert(mySize > 0 && "Stack is empty");
		return myVector.back();
	}

	template<class T>
	inline void Stack<T>::Push(const T& aValue)
	{
		myVector.push_back(aValue);
		++mySize;
	}

	template<class T>
	inline T Stack<T>::Pop()
	{
		assert(mySize > 0 && "Stack is empty");
		T element = myVector.back();
			myVector.erase(myVector.end()-1);
		--mySize;
		return element;
	}
}
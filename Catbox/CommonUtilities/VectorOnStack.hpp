#pragma once
#include <cassert>

namespace Catbox
{
	template <typename Type, int CAPACITY, typename CountType = unsigned short, bool SAFEMODE = true>
		
	class VectorOnStack
	{
	public:
		VectorOnStack();
		VectorOnStack(const VectorOnStack& aVectorOnStack);
		~VectorOnStack();
		VectorOnStack& operator=(const VectorOnStack& aVectorOnStack);
		inline const Type& operator[](const CountType aIndex) const;
		inline Type& operator[](const CountType aIndex);
		inline void Add(const Type& aObject);
		inline void Insert(const CountType aIndex, const Type& aObject);
		inline void RemoveCyclic(const Type& aObject);
		inline void RemoveCyclicAtIndex(const CountType aIndex);
		inline void Clear();
		__forceinline CountType Size() const;

	private:
		CountType myCurrentSize;
		Type myElements[CAPACITY];
	};


	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::VectorOnStack()
	{
		myCurrentSize = 0;
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::VectorOnStack(const VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>& aVectorOnStack)
	{
		myCurrentSize = aVectorOnStack.myCurrentSize;
		for (size_t i = 0; i < myCurrentSize; i++)
		{
			myElements[i] = aVectorOnStack.myElements[i];
		}
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::~VectorOnStack()
	{
		myCurrentSize = 0;
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>& VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::operator=(const VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>& aVectorOnStack)
	{
		myCurrentSize = aVectorOnStack.myCurrentSize;
		for (size_t i = 0; i < myCurrentSize; i++)
		{
			myElements[i] = aVectorOnStack.myElements[i];
		}
		return *this;
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline const Type& VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::operator[](const CountType aIndex) const
	{
		assert((aIndex >= 0 && aIndex < myCurrentSize) && L"Index was out of bounds of array.");
		return myElements[aIndex];
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline Type& VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::operator[](const CountType aIndex)
	{
		assert((aIndex >= 0 && aIndex < myCurrentSize) && "Index was out of bounds of array.");
		return myElements[aIndex];
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline void VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::Add(const Type& aObject)
	{
		assert((myCurrentSize < CAPACITY)  && "Tried adding new element while array was full.");
		myElements[myCurrentSize] = aObject;
		++myCurrentSize;
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline void VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::Insert(const CountType aIndex, const Type& aObject)
	{
		assert((aIndex >= 0 && aIndex < CAPACITY) && "Index was out of bounds of array.");
		assert((myCurrentSize < CAPACITY) &&"Tried inserting element while array was full.");
		assert((aIndex <= myCurrentSize) &&"Tried inserting element at a position larger than current array size.");

		for (size_t i = ++myCurrentSize; i > aIndex; i--)
		{
			myElements[i] = myElements[i-1];
		}
		myElements[aIndex] = aObject;
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline void VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::RemoveCyclic(const Type& aObject)
	{
		bool success = false;
		for (size_t i = 0; i < myCurrentSize; i++)
		{
			if (myElements[i] == aObject)
			{
				myElements[i] = myElements[--myCurrentSize];
				success = true;
				break;
			}
		}
		assert(success && "Requested element did not exist in array.");
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline void VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::RemoveCyclicAtIndex(const CountType aIndex)
	{
		assert((aIndex >= 0 && aIndex < CAPACITY) &&"Index was out of bounds of array.");
		assert((myCurrentSize > 0) &&"Tried to remove when array size was 0.");
		myElements[aIndex] = myElements[--myCurrentSize];
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	inline void VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::Clear()
	{
		myCurrentSize = 0;
	}

	template<typename Type, int CAPACITY, typename CountType, bool SAFEMODE>
	__forceinline CountType VectorOnStack<Type, CAPACITY, CountType, SAFEMODE>::Size() const
	{
		return myCurrentSize;
	}
}

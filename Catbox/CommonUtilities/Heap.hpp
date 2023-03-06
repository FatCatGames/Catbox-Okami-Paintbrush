#pragma once
#include <array>
#include <assert.h>

namespace Catbox
{
	template <typename T, int maxSize = 1000>
	class Heap
	{
	public:

		struct HeapNode
		{
			T value = 0;
			int heapIndex = 0;
		};

		Heap<T, maxSize>();
		~Heap<T, maxSize>() = default;
		int GetSize() const;
		void Enqueue(const T& aElement);
		const T& GetTop() const;
		T Dequeue();

	private:
		int myCurrentItemsCount = 0;
		std::array<HeapNode, maxSize> myHeap;
	};

	template<typename T, int maxSize>
	inline Heap<T, maxSize>::Heap()
	{
		for (size_t i = 0; i < maxSize; i++)
		{
			myHeap[i].heapIndex = i;
		}
	}

	template<typename T, int maxSize>
	inline int Heap<T, maxSize>::GetSize() const
	{
		return myCurrentItemsCount;
	}

	template<typename T, int maxSize>
	inline void Heap<T, maxSize>::Enqueue(const T& aElement)
	{
		assert(myCurrentItemsCount < maxSize && "Elements exceeded heap max size");
		HeapNode* item = &myHeap[myCurrentItemsCount];
		item->value = aElement;

		int parentIndex = (item->heapIndex - 1) / 2;
		while (true)
		{
			T& parentItem = myHeap[parentIndex].value;
			if (parentItem < item->value)
			{
				std::swap(item->value, parentItem);
			}
			else
			{
				break;
			}

			item = &myHeap[parentIndex];
			parentIndex = (item->heapIndex - 1) / 2;
		}

		++myCurrentItemsCount;
	}

	template<typename T, int maxSize>
	inline const T& Heap<T, maxSize>::GetTop() const
	{
		return myHeap.front().value;
		//return myHeap[myCurrentItemsCount - 1].value;
	}

	template<typename T, int maxSize>
	inline T Heap<T, maxSize>::Dequeue()
	{
		T front = myHeap[0].value;
		--myCurrentItemsCount;
		myHeap[0].value = myHeap[myCurrentItemsCount].value;

		HeapNode* item = &myHeap[0];
		while (true)
		{
			int leftChildIndex = item->heapIndex * 2 + 1;
			int rightChildIndex = item->heapIndex * 2 + 2;
			int swapIndex = 0;

			if (leftChildIndex < myCurrentItemsCount)
			{
				swapIndex = leftChildIndex;

				if (rightChildIndex < myCurrentItemsCount)
				{
					if (myHeap[leftChildIndex].value < myHeap[rightChildIndex].value)
					{
						swapIndex = rightChildIndex;
					}
				}

				if (item->value < myHeap[swapIndex].value)
				{
					std::swap(item->value, myHeap[swapIndex].value);
					item = &myHeap[swapIndex];
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}



		return front;
	}
}
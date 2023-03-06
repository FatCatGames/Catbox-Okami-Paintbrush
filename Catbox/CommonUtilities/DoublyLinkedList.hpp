#pragma once
#include <cassert>
#include <vector>
#include "DoublyLinkedListNode.h"
#include <functional>

namespace Catbox
{
	template <typename T>
	class DoublyLinkedList
	{
	public:
		DoublyLinkedList<T>() = default;
		~DoublyLinkedList<T>();
		int GetSize() const;
		DoublyLinkedListNode<T>* GetFirst();
		DoublyLinkedListNode<T>* GetLast();
		void InsertFirst(const T& aValue);
		void InsertLast(const T& aValue);
		void InsertBefore(DoublyLinkedListNode<T>* aNode, const T& aValue);
		void InsertAfter(DoublyLinkedListNode<T>* aNode, const T& aValue);
		void Remove(DoublyLinkedListNode<T>* aNode);
		DoublyLinkedListNode<T>* FindFirst(const T& aValue);
		DoublyLinkedListNode<T>* FindLast(const T& aValue);
		void Insert(const T& aValue, int anIndex);
		bool RemoveFirst(const T& aValue);
		bool RemoveLast(const T& aValue);
		void RemoveAt(const int& anIndex);
		void Sort(std::function<bool()> aComparison);
		void Clear();

	private:
		DoublyLinkedListNode<T>* myHead;
		DoublyLinkedListNode<T>* myTail;
		int mySize;
	};

	template<typename T>
	inline DoublyLinkedList<T>::~DoublyLinkedList()
	{
		Clear();
	}

	template <typename T>
	bool DoublyLinkedList<T>::RemoveFirst(const T& aValue)
	{
		DoublyLinkedListNode<T>** current = &myHead;
		int index = 0;
		while (current != nullptr)
		{
			if ((*current)->myData == aValue)
			{
				RemoveAt(index);
				return true;
			}
			current = &(*current)->myNextNode;
			++index;
		}
		return false;
	}

	template<typename T>
	inline bool DoublyLinkedList<T>::RemoveLast(const T& aValue)
	{
		DoublyLinkedListNode<T>** current = &myTail;
		int index = mySize - 1;
		while (current != nullptr)
		{
			if ((*current)->myData == aValue)
			{
				RemoveAt(index);
				return true;
			}
			current = &(*current)->myPreviousNode;
			--index;
		}
		return false;
	}


	template <typename T>
	void DoublyLinkedList<T>::InsertLast(const T& aValue)
	{
		if (myHead == nullptr)
		{
			myHead = new DoublyLinkedListNode<T>();
			myHead->myData = aValue;
			myTail = myHead;
		}
		else
		{
			DoublyLinkedListNode<T>* newTail = new DoublyLinkedListNode<T>();
			newTail->myData = aValue;
			myTail->SetNext(newTail);
			myTail = newTail;
		}
		++mySize;
	}

	template<typename T>
	inline void DoublyLinkedList<T>::InsertBefore(DoublyLinkedListNode<T>* aNode, const T& aValue)
	{
		DoublyLinkedListNode<T>* newNode = new DoublyLinkedListNode<T>();
		newNode->myData = aValue;
		if (aNode->myPreviousNode)
		{
			aNode->myPreviousNode->SetNext(newNode);
		}
		else
		{
			myHead = newNode;
		}
		newNode->SetNext(aNode);
		++mySize;
	}

	template<typename T>
	inline void DoublyLinkedList<T>::InsertAfter(DoublyLinkedListNode<T>* aNode, const T& aValue)
	{
		DoublyLinkedListNode<T>* newNext = aNode->myNextNode;
		DoublyLinkedListNode<T>* newNode = new DoublyLinkedListNode<T>();
		if (aNode == myTail) myTail = newNode;
		newNode->myData = aValue;
		aNode->SetNext(newNode);
		newNode->SetNext(newNext);
		++mySize;
	}

	template<typename T>
	inline void DoublyLinkedList<T>::Remove(DoublyLinkedListNode<T>* aNode)
	{
		if (aNode->myPreviousNode)
		{
			aNode->myPreviousNode->SetNext(aNode->myNextNode);
			if (aNode == myTail)
			{
				myTail = aNode->myPreviousNode;
			}
		}
		else
		{
			myHead = aNode->myNextNode;
			myHead->myPreviousNode = nullptr;
		}
		delete aNode;
		assert(true && "Attempted removal of node that was not found in list");
	}



	template<typename T>
	inline void DoublyLinkedList<T>::RemoveAt(const int& anIndex)
	{
		if (anIndex == 0)
		{
			DoublyLinkedListNode<T>* newHead = myHead->myNextNode;
			delete(myHead);
			myHead = newHead;
			myHead->myPreviousNode = nullptr;
			--mySize;
			return;
		}

		if (anIndex == mySize - 1)
		{
			DoublyLinkedListNode<T>* newTail = myTail->myPreviousNode;
			delete(myTail);
			myTail = newTail;
			myTail->myNextNode = nullptr;
			--mySize;
			return;
		}

		DoublyLinkedListNode<T>* node = myHead;

		for (int i = 0; i < anIndex - 1; i++) //loop through nodes until index-1
		{
			node = node->myNextNode;
			if (node == nullptr)
			{
				return;
			}
		}

		DoublyLinkedListNode<T>* newNext = nullptr;
		if (node->myNextNode != nullptr)
		{
			newNext = node->myNextNode->myNextNode;
		}
		delete(node->myNextNode); //delete the node at the index
		node->SetNext(newNext); //relink node at index-1 to index+1

		--mySize;
		return; //return the new node at index
	}


	template<typename T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindFirst(const T& aValue)
	{
		if (!myHead) return nullptr;
		DoublyLinkedListNode<T>* node = myHead;
		do
		{
			if (node->myData == aValue)
			{
				return node;
			}
			node = node->myNextNode;
		} while (node != myTail);

		return nullptr;
	}

	template<typename T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindLast(const T& aValue)
	{
		if (!myTail) return nullptr;
		DoublyLinkedListNode<T>* node = myTail;
		do
		{
			if (node->myData == aValue)
			{
				return node;
			}
			node = node->myPreviousNode;
		} while (node != myHead);

		return nullptr;
	}

	template <typename T>
	void DoublyLinkedList<T>::Insert(const T& aValue, int anIndex)
	{
		if (myHead == nullptr)
		{
			myHead = new DoublyLinkedListNode<T>();
			myHead->myData = aValue;
			myTail = myHead;
		}
		else
		{
			DoublyLinkedListNode<T>* node = myHead;
			for (int i = 0; i < anIndex; i++) //loop through nodes until index-1
			{
				node = node->myNextNode;
				assert(node != nullptr && "Tried to insert at index out of range");
			}

			DoublyLinkedListNode<T>* newNode = new DoublyLinkedListNode<T>();
			newNode->myData = aValue;
			if (node->myPreviousNode)
			{
				node->myPreviousNode->SetNext(newNode);
			}
			newNode->SetNext(node);
			if (anIndex == 0) myHead = newNode;
			else if (anIndex == mySize) myTail = newNode;
		}
		++mySize;
	}

	template<typename T>
	inline void DoublyLinkedList<T>::InsertFirst(const T& aValue)
	{
		Insert(aValue, 0);
	}

	template<typename T>
	inline void DoublyLinkedList<T>::Sort(std::function<bool()> aComparison)
	{
		//not implemented
		/*LinkedListNode<T>* currentNode = myHead;
		LinkedListNode<T>* endNode = myTail;
		while (currentNode != endNode)
		{
			if(aComparison)
		}*/
	}

	template<typename T>
	inline int DoublyLinkedList<T>::GetSize() const
	{
		return mySize;
	}

	template<typename T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetFirst()
	{
		return myHead;
	}

	template<typename T>
	inline DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetLast()
	{
		return myTail;
	}




	template<typename T>
	inline void DoublyLinkedList<T>::Clear()
	{
		DoublyLinkedListNode<T>* current = myHead;
		while (current != nullptr)
		{
			DoublyLinkedListNode<T>* next = current->GetNext();
			delete current;
			current = next;
		}
		myHead = nullptr;
		myTail = nullptr;
		mySize = 0;
	}
}
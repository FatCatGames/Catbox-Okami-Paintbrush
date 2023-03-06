#pragma once
#include <cassert>
#include <vector>

namespace Catbox
{
	template <typename T>
	struct LinkedListNode
	{
		T data;
		LinkedListNode* next;
	};

	template <typename T>
	class LinkedList
	{
	public:
		LinkedList<T>() = default;
		~LinkedList<T>() = default;
		void RemoveFirst();
		LinkedListNode<T>* RemoveAt(int anIndex);
		void Add(T anElement);
		void Insert(T anElement, int anIndex);
		LinkedListNode<T>* GetHead();

	private:
		LinkedListNode<T>* myHead;
		LinkedListNode<T>* myTail;
	};

	template <typename T>
	void LinkedList<T>::RemoveFirst()
	{
		if (myHead != nullptr)
		{
			LinkedListNode<T>* newHead = myHead->next;
			delete(myHead);
			myHead = newHead;
		}
	}

	template <typename T>
	void LinkedList<T>::Add(T anElement)
	{
		if (myHead == nullptr) 
		{
			myHead = new LinkedListNode<T>();
			myHead->data = anElement;
			myTail = myHead;
		}
		else 
		{
			myTail->next = new LinkedListNode<T>();
			myTail = myTail->next;
			myTail->data = anElement;
		}
	}

	template <typename T>
	void LinkedList<T>::Insert(T anElement, int anIndex)
	{
		if (myHead == nullptr)
		{
			myHead = new LinkedListNode<T>();
			myHead->data = anElement;
			myTail = myHead;
		}
		else
		{
			LinkedListNode<T>* node = myHead;
			for (int i = 0; i < anIndex - 1; i++) //loop through nodes until index-1
			{
				node = node->next;
				assert(node != nullptr && "Tried to insert at index out of range");
			}

			LinkedListNode<T>* newNode = new LinkedListNode<T>();
			newNode->data = anElement;
			newNode->next = node->next;
			node->next = newNode;
		}
	}

	template <typename T>
	inline LinkedListNode<T>* LinkedList<T>::RemoveAt(int anIndex)
	{
		if (anIndex == 0)
		{
			LinkedListNode<T>* newHead = myHead->next;
			delete(myHead);
			myHead = newHead;
			return nullptr;
		}

		LinkedListNode<T>* node = myHead;

		for (int i = 0; i < anIndex - 1; i++) //loop through nodes until index-1
		{
			node = node->next;
			if (node == nullptr) return nullptr;
		}

		LinkedListNode<T>* newNext = nullptr;
		if (node->next != nullptr)
		{
			newNext = node->next->next;
		}
		delete(node->next); //delete the node at the index
		node->next = newNext; //relink node at index-1 to index+1

		return newNext; //return the new node at index
	}

	template <typename T>
	inline LinkedListNode<T>* LinkedList<T>::GetHead()
	{
		return myHead;
	}
}
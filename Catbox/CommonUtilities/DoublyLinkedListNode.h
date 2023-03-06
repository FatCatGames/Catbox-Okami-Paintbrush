#pragma once

namespace Catbox 
{
	template <typename T>
	class DoublyLinkedListNode
	{
	public:
		DoublyLinkedListNode<T>(const DoublyLinkedListNode<T>&) = delete;
		DoublyLinkedListNode<T>& operator=(const DoublyLinkedListNode<T>&) = delete;
		const T& GetValue() const;
		T& GetValue();
		void SetValue(const T& aValue);
		DoublyLinkedListNode<T>* GetNext() const;
		DoublyLinkedListNode<T>* GetPrevious() const;

	private:
		template <typename> friend class DoublyLinkedList;
		void SetNext(DoublyLinkedListNode* aNewNext);
		T myData;
		DoublyLinkedListNode* myNextNode = nullptr;
		DoublyLinkedListNode* myPreviousNode = nullptr;

		DoublyLinkedListNode();
		DoublyLinkedListNode(const T& aValue);
		~DoublyLinkedListNode() {}
	};

	template<typename T>
	inline const T& DoublyLinkedListNode<T>::GetValue() const
	{
		return myData;
	}

	template<typename T>
	inline T& DoublyLinkedListNode<T>::GetValue()
	{
		return myData;
	}

	template<typename T>
	inline void DoublyLinkedListNode<T>::SetValue(const T& aValue)
	{
		myData = aValue;
	}

	template<typename T>
	inline DoublyLinkedListNode<T>* DoublyLinkedListNode<T>::GetNext() const
	{
		return myNextNode;
	}

	template<typename T>
	inline DoublyLinkedListNode<T>* DoublyLinkedListNode<T>::GetPrevious() const
	{
		return myPreviousNode;
	}

	template<typename T>
	inline void DoublyLinkedListNode<T>::SetNext(DoublyLinkedListNode* aNewNext)
	{
		myNextNode = aNewNext;
		aNewNext->myPreviousNode = this;
	}

	template<typename T>
	inline DoublyLinkedListNode<T>::DoublyLinkedListNode()
	{
	}

	template<typename T>
	inline DoublyLinkedListNode<T>::DoublyLinkedListNode(const T& aValue)
	{
		myData = aValue;
	}
}
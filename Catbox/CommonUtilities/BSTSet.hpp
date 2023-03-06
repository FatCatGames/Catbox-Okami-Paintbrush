#pragma once
#include <vector>

namespace Catbox
{
	template <class T>
	struct BSTSetNode
	{
		BSTSetNode* leftBranch;
		BSTSetNode* rightBranch;
		T value;
		BSTSetNode(const T& aValue)
		{
			value = aValue;
			leftBranch = nullptr;
			rightBranch = nullptr;
		}
	};

	template <class T>
	class BSTSet
	{
	public:
		BSTSet() = default;
		~BSTSet();
		bool HasElement(const T& aValue);
		void Insert(const T& aValue);
		void Remove(const T& aValue);

	private:
		void SetNewSuccessor(BSTSetNode<T>* aNodeToRemove, BSTSetNode<T>* aNodeParent);

		BSTSetNode<T>* myRoot = nullptr;
	};

	template <class T>
	void DeleteBranch(BSTSetNode<T>* aRoot)
	{
		if (aRoot == nullptr) return;
		BSTSetNode<T>* leftBranch = aRoot->leftBranch;
		BSTSetNode<T>* rightBranch = aRoot->rightBranch;

		if (leftBranch != nullptr)
		{
			DeleteBranch(leftBranch);
		}
		if (rightBranch != nullptr)
		{
			DeleteBranch(rightBranch);
		}

		delete aRoot;
		aRoot = nullptr;
	}

	template<class T>
	inline BSTSet<T>::~BSTSet()
	{
		DeleteBranch(myRoot);
	}

	template<class T>
	inline bool BSTSet<T>::HasElement(const T& aValue)
	{
		BSTSetNode<T>* node = myRoot;
		while (node != nullptr)
		{
			if (aValue < node->value)
			{
				if (node->leftBranch == nullptr)
				{
					return false;
				}
				node = node->leftBranch;
			}
			else if (aValue > node->value)
			{
				if (node->rightBranch == nullptr)
				{
					return false;
				}
				node = node->rightBranch;
			}
			else return true;
		}
		return false;
	}

	template<class T>
	inline void BSTSet<T>::Insert(const T& aValue)
	{
		if (myRoot == nullptr)
		{
			myRoot = new BSTSetNode<T>(aValue);
			return;
		}

		BSTSetNode<T>* node = myRoot;
		while (true)
		{
			if (aValue < node->value)
			{
				if (node->leftBranch != nullptr)
				{
					node = node->leftBranch;
				}
				else
				{
					node->leftBranch = new BSTSetNode<T>(aValue);
					return;
				}
			}
			else if (aValue > node->value)
			{
				if (node->rightBranch != nullptr)
				{
					node = node->rightBranch;
				}
				else
				{
					node->rightBranch = new BSTSetNode<T>(aValue);
					return;
				}
			}
			else return; //value already exists, cannot be added
		}
	}

	template<class T>
	inline void BSTSet<T>::SetNewSuccessor(BSTSetNode<T>* aNodeToRemove, BSTSetNode<T>* aNodeParent)
	{
		BSTSetNode<T>* successor = aNodeToRemove->rightBranch;
		if (successor == nullptr) successor = aNodeToRemove->leftBranch;

		if (aNodeToRemove->rightBranch != nullptr && aNodeToRemove->leftBranch != nullptr) //node has 2 children
		{
			BSTSetNode<T>* successorParent = successor;
			while (successor->leftBranch != nullptr) //find the inorder successor
			{
				successorParent = successor;
				successor = successor->leftBranch;
			}

			

			successorParent->leftBranch = nullptr;

			if (successor != aNodeToRemove->leftBranch)
			{
				successor->leftBranch = aNodeToRemove->leftBranch;
			}
			if (successor != aNodeToRemove->rightBranch)
			{
				BSTSetNode<T>* relinkNode = successor;
				while (relinkNode->rightBranch != nullptr)
				{
					relinkNode = relinkNode->rightBranch;
				}

				relinkNode->rightBranch = aNodeToRemove->rightBranch;
			}
		}

		if (aNodeParent != nullptr)
		{
			if (aNodeToRemove == aNodeParent->leftBranch)
			{
				aNodeParent->leftBranch = successor;
			}
			else aNodeParent->rightBranch = successor;
		}
		else //node to be deleted is root
		{
			myRoot = successor;
		}
		
		delete aNodeToRemove;
		aNodeToRemove = nullptr;
	}

	template<class T>
	inline void BSTSet<T>::Remove(const T& aValue)
	{
		BSTSetNode<T>* node = myRoot;
		BSTSetNode<T>* previous = nullptr;

		while (node != nullptr)
		{
			if (aValue < node->value)
			{
				previous = node;
				node = node->leftBranch;
			}
			else if (aValue > node->value)
			{
				previous = node;
				node = node->rightBranch;
			}
			else //value matches node, should be deleted 
			{
				SetNewSuccessor(node, previous);
				return;
			}
		}
	}
}
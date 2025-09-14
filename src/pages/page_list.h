
#pragma once

#include <stdint.h>

template<typename T>
struct IntrusiveDoublyLinkedList
{
	//Depending on the mode; root-set: holds the addresses of adjacent root set nodes. child: the address of our parent node
	T* BackwardNode = static_cast<T*>(this);
	T* ForwardNode = static_cast<T*>(this);

	//Child Data
	T** PagesMemory = nullptr;
	uint8_t ChildPagesCount = 0;

	T* Next() {return ForwardNode;}
	T* Prev() {return BackwardNode;}
	T* GetParent() {return BackwardNode;}

	T* AddAsRootSet(T& InNode)
	{
		T* Loopback = ForwardNode;
		ForwardNode = &InNode;

		InNode.BackwardNode = static_cast<T*>(this);
		InNode.ForwardNode = Loopback;

		//complete the chain.
		InNode.ForwardNode->BackwardNode = ForwardNode;

		return &InNode;
	}

	void RemoveFromRootSet(T& InNode)
	{
		if (InNode.ForwardNode == nullptr || InNode.BackwardNode == nullptr)
		{
			return;
		}

		InNode.BackwardNode->ForwardNode = InNode.ForwardNode;
		InNode.ForwardNode->BackwardNode = InNode.BackwardNode;
		InNode.ForwardNode = nullptr;
		InNode.BackwardNode = nullptr;
	}

	void AddChild(T& InNode, uint8_t ID)
	{
		if (ChildPagesCount != 0 && ID < ChildPagesCount)
		{
			PagesMemory[ID] = &InNode;

			//child nodes can't be on the root set.
			InNode.BackwardNode = nullptr;
			InNode.ForwardNode = nullptr;
		}
	}
};

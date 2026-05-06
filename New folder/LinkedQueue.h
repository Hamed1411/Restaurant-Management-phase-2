#ifndef LINKED_QUEUE_
#define LINKED_QUEUE_
#include <iostream>
using namespace std;

#include "Node.h"
#include "QueueADT.h"
#include "order.h"

template <typename T>
class LinkedQueue :public QueueADT<T>
{
private:
	void printRec(Node<T>* curr) const
	{
		// Base Case
		if (curr == nullptr) {
			return;
		}
		cout << curr->getItem();

		if (curr->getNext() != nullptr) {
			cout << ", ";
			printRec(curr->getNext());
		}
	}


	void printRecursive2(Node<T>* curr) const {
		if (curr == nullptr) return; // Base Case: End of list

		// Use the wrapper to ensure only IDs print
		cout << OrderIDOnly(curr->getItem());

		if (curr->getNext() != nullptr) {
			cout << ", ";
			printRecursive2(curr->getNext()); // Recursive Step: Move to next
		}
	}
protected:
	Node<T>* backPtr;
	Node<T>* frontPtr;
	int count = 0; // Additional data member to keep track of the number of items in the queue (Initialized to 0)
public:
	LinkedQueue();
	bool isEmpty() const;
	bool enqueue(const T& newEntry);
	bool dequeue(T& frntEntry);
	bool peek(T& frntEntry)  const;
	void printQueue() const;
	void print() const;
	void print2() const;
	int getCount() const;
	~LinkedQueue();
};
/////////////////////////////////////////////////////////////////////////////////////////

/*
Function: Queue()
The constructor of the Queue class.

*/

template <typename T>
LinkedQueue<T>::LinkedQueue()
{
	backPtr = nullptr;
	frontPtr = nullptr;

}
/////////////////////////////////////////////////////////////////////////////////////////

/*
Function: isEmpty
Sees whether this queue is empty.

Input: None.
Output: True if the queue is empty; otherwise false.
*/
template <typename T>
bool LinkedQueue<T>::isEmpty() const
{
	return (frontPtr == nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

/*Function:enqueue
Adds newEntry at the back of this queue.

Input: newEntry .
Output: True if the operation is successful; otherwise false.
*/

template <typename T>
bool LinkedQueue<T>::enqueue(const T& newEntry)
{
	Node<T>* newNodePtr = new Node<T>(newEntry);
	// Insert the new node
	if (isEmpty())	//special case if this is the first node to insert
		frontPtr = newNodePtr; // The queue is empty
	else
		backPtr->setNext(newNodePtr); // The queue was not empty

	backPtr = newNodePtr; // New node is the last node now
	count++; // Increment the count of items in the queue
	return true;
} // end enqueue


/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Function: dequeue
Removes the front of this queue. That is, removes the item that was added
earliest.

Input: None.
Output: True if the operation is successful; otherwise false.
*/

template <typename T>
bool LinkedQueue<T>::dequeue(T& frntEntry)
{
	if (isEmpty())
		return false;

	Node<T>* nodeToDeletePtr = frontPtr;
	frntEntry = frontPtr->getItem();
	frontPtr = frontPtr->getNext();
	// Queue is not empty; remove front
	if (nodeToDeletePtr == backPtr)	 // Special case: last node in the queue
		backPtr = nullptr;

	// Free memory reserved for the dequeued node
	delete nodeToDeletePtr;

	count--; // Decrement the count of items in the queue
	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////

/*
Function: peek
copies the front of this queue to the passed param. The operation does not modify the queue.

Input: None.
Output: The front of the queue.
*/

template <typename T>
bool LinkedQueue<T>::peek(T& frntEntry) const
{
	if (isEmpty())
		return false;

	frntEntry = frontPtr->getItem();
	return true;

}

///////////////////////////////////////////////////////////////////////////////////


template<typename T>
inline void LinkedQueue<T>::printQueue() const
{
	Node<T>* current = frontPtr;
	if (isEmpty()) {
		cout << "The list is empty." << endl;
		return;
	}
	else {
		while (current != nullptr) {
			cout << current->getItem() << ", ";
			current = current->getNext();
		}
		cout << endl;
	}
}


template <typename T>
void LinkedQueue<T>::print() const
{
	if (isEmpty()) {
		cout << "The list is empty.";
	}
	else {
		printRec(frontPtr); 
	}
	cout << endl;
}

template<typename T>
inline void LinkedQueue<T>::print2() const
{
	printRecursive2(frontPtr);
}

///////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline int LinkedQueue<T>::getCount() const
{
	return count;
}
///////////////////////////////////////////////////////////////////////////////////

template <typename T>
LinkedQueue<T>::~LinkedQueue()
{
	T temp;
	while (dequeue(temp));
}

#endif
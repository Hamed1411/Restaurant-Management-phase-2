#include "QueueWithCancel.h"

Node<order*>* QueueWithCancel::recursiveRemove(Node<order*>*& curr, int id, order*& foundOrd)
{
	if (curr == nullptr) return nullptr; // Base case: ID not found

	if (curr->getItem()->getID() == id) {
		foundOrd = curr->getItem();      // Capture the order
		Node<order*>* temp = curr;
		curr = curr->getNext();          
		return temp;                     // Return node to be deleted
	}

	return recursiveRemove(curr->getNextPointerReference(), id, foundOrd);
}

bool QueueWithCancel::cancelOrderByID(int id, order*& removed)
{
	
	Node<order*>* toDelete = recursiveRemove(frontPtr, id, removed);
	if (toDelete) {
		delete toDelete; 
		return true;
	}
	return false;
}





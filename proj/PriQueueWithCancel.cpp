#include "PriQueueWithCancel.h"



priNode<order*>* PriQueueWithCancel::recursiveRemove(priNode<order*>*& curr, int id, order*& foundOrd)
{
	int pri;
	// Base Case 1: ID not found/End of list
	if (curr == nullptr) return nullptr;

	// Base Case 2: ID Match
	if (curr->getItem(pri)->getID() == id) {
		foundOrd = curr->getItem(pri);      // Save the order pointer
		priNode<order*>* temp = curr;    // Save the node to delete
		curr = curr->getNext();          
		return temp;                     
	}

	
	return recursiveRemove(curr->getNextPointerReference(), id, foundOrd);
}

bool PriQueueWithCancel::cancelOrderByID(int id, order*& removed)
{
	removed = nullptr;
	if (isEmpty()) return false;

	// Start recursion from the head 
	priNode<order*>* toDelete = recursiveRemove(head, id, removed);

	if (toDelete != nullptr) {
		delete toDelete; // Delete 
		return true;
	}

	return false; // ID was not found in the list
}


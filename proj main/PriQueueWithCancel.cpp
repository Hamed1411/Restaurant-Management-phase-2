#include "PriQueueWithCancel.h"



bool PriQueueWithCancel::cancelOrderByID(int id, order*& removed)
{
	if (this->isEmpty()) return false; 

	priNode<order*>* prev = nullptr;
	priNode<order*>* curr = this->head;

	while (curr != nullptr) {
		int pri;
		order* pOrd = curr->getItem(pri);
		if (pOrd->getID() == id) {
			removed = pOrd; 
			if (prev == nullptr) { 
				this->head = curr->getNext();
			}
			else { 
				prev->setNext(curr->getNext());
			}
			delete curr; 
			this->count--; 
			return true; 


		}
		prev = curr;
		curr = curr->getNext();
	}
	removed = nullptr; 
	return false; 
}


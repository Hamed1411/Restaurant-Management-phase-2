#include "QueueWithCancel.h"

bool QueueWithCancel::cancelOrderByID(int id, order*& removed)
{
	
	removed = nullptr;

	if (this->isEmpty()) return false; 

	Node<order*>* prev = nullptr;
	Node<order*>* curr = this->frontPtr;

	while (curr != nullptr) {
		order* item = curr->getItem();
		if (item != nullptr && item->getID() == id) { 
			removed = item; 

			if (prev == nullptr) { 
				this->frontPtr = curr->getNext();

				
				if (this->frontPtr == nullptr) this->backPtr = nullptr;
			}
			else { 
				prev->setNext(curr->getNext());
			}

			if (curr == this->backPtr) { 
				this->backPtr = prev;
			}

			delete curr; 
			--this->count; 
			return true; 
		}
		prev = curr;
		curr = curr->getNext();
	}
	
	return false; 
}



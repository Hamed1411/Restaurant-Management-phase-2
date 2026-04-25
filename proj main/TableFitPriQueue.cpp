#include "TableFitPriQueue.h"

bool TableFitPriQueue::getBest(int groupSize, table*& foundTable)
{
    if (this->isEmpty()) return false;

   
    priNode<table*>* curr = this->head;
    priNode<table*>* prev = nullptr;

    
    while (curr != nullptr)
    {
        int pri; 
        table* pTable = curr->getItem(pri);

        if (pTable->getCapacity() >= groupSize)
        {
            
            foundTable = pTable;


            if (prev == nullptr)
                this->head = curr->getNext();
            else
                prev->setNext(curr->getNext());

            delete curr; 
            this->count--;
            return true;
        }

        prev = curr;
        curr = curr->getNext();
    }

    return false; 
}
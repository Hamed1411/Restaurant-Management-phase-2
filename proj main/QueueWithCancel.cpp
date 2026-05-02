#include "QueueWithCancel.h"

Node<order*>* QueueWithCancel::recursiveRemove(Node<order*>*& curr, int id, order*& foundOrd)
{
    if (curr == nullptr)
        return nullptr;

    if (curr->getItem()->getID() == id)
    {
        foundOrd = curr->getItem();
        Node<order*>* temp = curr;
        curr = curr->getNext();
        return temp;
    }

    return recursiveRemove(curr->getNextPointerReference(), id, foundOrd);
}

bool QueueWithCancel::cancelOrderByID(int id, order*& removed)
{
    removed = nullptr;

    Node<order*>* toDelete = recursiveRemove(frontPtr, id, removed);

    if (toDelete != nullptr)
    {
        delete toDelete;
        count--;   

        if (frontPtr == nullptr)
            backPtr = nullptr;

        return true;
    }

    return false;
}
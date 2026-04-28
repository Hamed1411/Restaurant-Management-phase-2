#pragma once
#include "LinkedQueue.h"
#include "order.h"

class QueueWithCancel : public LinkedQueue<order*>
{
private:
    Node<order*>* recursiveRemove(Node<order*>*& curr, int id, order*& foundOrd);
public:
    bool cancelOrderByID(int id, order*& removed);
   
};



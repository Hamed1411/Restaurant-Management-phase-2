#pragma once
#include "LinkedQueue.h"
#include "order.h"

class QueueWithCancel : public LinkedQueue<order*>
{
public:
    bool cancelOrderByID(int id, order*& removed);
   
};



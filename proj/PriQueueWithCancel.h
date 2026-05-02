#pragma once
#include "PriQueue.h"
#include "order.h"

class PriQueueWithCancel
	:public priQueue <order*>
{
private:
	priNode<order*>* recursiveRemove(priNode<order*>*& curr, int id, order*& foundOrd);
public:
	bool cancelOrderByID(int id, order*& removed);
};



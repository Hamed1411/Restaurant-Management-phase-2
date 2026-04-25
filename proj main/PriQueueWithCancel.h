#pragma once
#include "PriQueue.h"
#include "order.h"

class PriQueueWithCancel
	:public priQueue <order*>
{
public:
	bool cancelOrderByID(int id, order*& removed);
	

};



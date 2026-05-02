#pragma once
#include "Action.h"
#include "Restaurant.h"
#include "order.h"

class CancelAction : public Action {
	
private:
    int orderID;

public:
    CancelAction(int TS, int id, Restaurant* r)
        : Action(TS, r), orderID(id) {
    }

    virtual void ACT() override
    {
       pRest->HandleCancelOrder(orderID);
    }


};
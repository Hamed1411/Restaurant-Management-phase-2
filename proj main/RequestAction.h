#pragma once
#include "Action.h"
#include "order.h"
#include "Restaurant.h"


class RequestAction : public Action {
private:
    ORDER_TYPE type; 
    int ID;
    int size;      
    int money;     


public:
    RequestAction(int TS, ORDER_TYPE t, int id, int s, int m, Restaurant* r)
        : Action(TS, r), type(t), ID(id), size(s), money(m) {
    }

    virtual void ACT() override
    {
        order* pOrd = new order(ID, type, ActionTime, size, money);
        pRest->AddOrderToPending(pOrd);
    }
};
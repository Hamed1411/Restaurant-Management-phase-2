#pragma once
#include "Action.h"
#include "order.h"

class Restaurant;

class RequestAction : public Action
{
private:
    order* pOrder;

public:
    RequestAction(int TS, order* ord, Restaurant* r)
        : Action(TS, r)
    {
        pOrder = ord;
    }

    virtual void ACT() override
    {
        if (pOrder != nullptr)
        {
            pRest->AddOrderToPending(pOrder);
        }
    }
};
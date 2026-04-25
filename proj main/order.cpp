#include "order.h"
#include "chef.h"
#include <iostream>
using namespace std;

order::order()
{
    ID = 0;
    type = ODN;

    TQ = -1;
    TA = -1;
    TR = -1;
    TS = -1;
    TF = -1;

    size = 0;
    price = 0;

    distance = 0;
    seats = 0;
    duration = 0;
    canShare = false;

    assignedChef = nullptr;
}

order::order(int id, ORDER_TYPE t, int tq, int Size, double P)
{
    ID = id;
    type = t;
    size = Size;
    price = P;

    TQ = tq;
    TA = -1;
    TR = -1;
    TS = -1;
    TF = -1;

    distance = 0;
    seats = 0;
    duration = 0;
    canShare = false;

    assignedChef = nullptr;
}

int order::getID() const
{
    return ID;
}

ORDER_TYPE order::getType() const
{
    return type;
}

int order::getSize() const
{
    return size;
}

double order::getPrice() const
{
    return price;
}

int order::getTQ() const
{
    return TQ;
}

int order::getTA() const
{
    return TA;
}

int order::getTR() const
{
    return TR;
}

int order::getTS() const
{
    return TS;
}

int order::getTF() const
{
    return TF;
}

int order::getDistance() const
{
    return distance;
}

int order::getSeats() const
{
    return seats;
}

int order::getDuration() const
{
    return duration;
}

bool order::getCanShare() const
{
    return canShare;
}

void order::setType(ORDER_TYPE t)
{
    type = t;
}

void order::setSize(int s)
{
    size = s;
}

void order::setPrice(double p)
{
    price = p;
}

void order::setTQ(int t)
{
    TQ = t;
}

void order::setTA(int t)
{
    TA = t;
}

void order::setTR(int t)
{
    TR = t;
}

void order::setTS(int t)
{
    TS = t;
}

void order::setTF(int t)
{
    TF = t;
}

void order::setDistance(int d)
{
    distance = d;
}

void order::setSeats(int s)
{
    seats = s;
}

void order::setDuration(int d)
{
    duration = d;
}

void order::setCanShare(bool c)
{
    canShare = c;
}

void order::setChef(chef* c)
{
    assignedChef = c;
}

chef* order::getChef() const
{
    return assignedChef;
}

bool order::isDineIn() const
{
    return (type == ODG || type == ODN);
}

bool order::isTakeaway() const
{
    return (type == OT);
}

bool order::isDelivery() const
{
    return (type == OVC || type == OVG || type == OVN);
}

bool order::isGrilled() const
{
    return (type == ODG || type == OVG);
}

int order::getIdleTime() const
{
    if (TA == -1 || TS == -1 || TR == -1 || TQ == -1)
        return -1;

    return (TA - TQ) + (TS - TR);
}

int order::getCookPeriod() const
{
    if (TR == -1 || TA == -1)
        return -1;

    return TR - TA;
}

int order::getWaitTime() const
{
    int idle = getIdleTime();
    int cook = getCookPeriod();

    if (idle == -1 || cook == -1)
        return -1;

    return idle + cook;
}

int order::getServiceDuration() const
{
    if (TF == -1 || TS == -1)
        return -1;

    return TF - TS;
}

const char* order::getTypeAsString() const
{
    switch (type)
    {
    case ODG: return "ODG";
    case ODN: return "ODN";
    case OT:  return "OT";
    case OVC: return "OVC";
    case OVG: return "OVG";
    case OVN: return "OVN";
    default:  return "UNKNOWN";
    }
}

void order::Print() const
{
    cout << "[" << getTypeAsString() << ", " << ID << "]";
}

double order::getPriority() const
{
    if (type == OVG || type == ODG)
    {
        if (size <= 0 || TQ <= 0)
            return 0;

        return (double)price / (size * TQ);
    }

    return 0;
}

ostream& operator<<(ostream& out, const order* pOrd)
{
    if (pOrd == nullptr)
    {
        out << "null";
        return out;
    }

    out << "[" << pOrd->getTypeAsString() << ", " << pOrd->getID() << "]";
    return out;
}
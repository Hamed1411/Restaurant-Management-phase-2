#include "order.h"
#include "chef.h"
#include "scooter.h"
#include "table.h"
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

    actualChefsCount = 0;
    numChefsRequired = 1;
    for (int i = 0; i < 4; i++) assignedChefs[i] = nullptr;

    actualScootersCount = 0;
    numScootersRequired = 1;
    for (int i = 0; i < 4; i++) assignedScooters[i] = nullptr;

    assignedTable = nullptr;

    isRescue = false;
    rescueScooter = nullptr;
    failed = false;
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

    actualChefsCount = 0;
    numChefsRequired = 1;
    for (int i = 0; i < 4; i++) assignedChefs[i] = nullptr;

    actualScootersCount = 0;
    numScootersRequired = 1;
    for (int i = 0; i < 4; i++) assignedScooters[i] = nullptr;

    assignedTable = nullptr;

    isRescue = false;
    rescueScooter = nullptr;
    failed = false;
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

int order::getTW() const
{
    return TW;
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

void order::setTW(int t)
{
    TW = t;
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

void order::addChef(chef* c)
{
    if (actualChefsCount < 4)
        assignedChefs[actualChefsCount++] = c;
}

chef* order::getChef(int idx) const
{
    if (idx >= 0 && idx < actualChefsCount)
        return assignedChefs[idx];
    return nullptr;
}

int order::getChefsCount() const
{
    return actualChefsCount;
}

void order::setNumChefsRequired(int n)
{
    numChefsRequired = (n > 4) ? 4 : n;
}

int order::getNumChefsRequired() const
{
    return numChefsRequired;
}

void order::addScooter(scooter* s)
{
    if (actualScootersCount < 4)
        assignedScooters[actualScootersCount++] = s;
}

scooter* order::getScooter(int idx) const
{
    if (idx >= 0 && idx < actualScootersCount)
        return assignedScooters[idx];
    return nullptr;
}

int order::getScootersCount() const
{
    return actualScootersCount;
}

void order::setNumScootersRequired(int n)
{
    numScootersRequired = (n > 4) ? 4 : n;
}

int order::getNumScootersRequired() const
{
    return numScootersRequired;
}

void order::setTable(table* t)
{
    assignedTable = t;
}

table* order::getTable() const
{
    return assignedTable;
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
    return (type == OVC || type == OVG || type == OVN || type == OVB);
}

bool order::isGrilled() const
{
    return (type == ODG || type == OVG);
}

bool order::isCombo() const
{
    return (type == OVB);
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
    case OVB: return "OVB";
    default:  return "UNKNOWN";
    }
}

void order::Print() const
{
    cout << "[" << getTypeAsString() << ", " << ID << "]";
}

double order::getPriority() const
{
    if (type == OVG || type == OVB)
    {
        if (size <= 0 || TQ <= 0)
            return 0;

        // High price and small distance = High priority
        // Weights: Price (40%), Distance (30%), Size (30%)
        double priority = getPrice() * 0.4 + (100.0 / (getDistance() + 1)) * 0.3 + getSize() * 0.3;
        return priority;
    }

    return 0;
}

void order::setRescue(bool r) { isRescue = r; }
bool order::isRescueMission() const { return isRescue; }
void order::setRescueScooter(scooter* s) { rescueScooter = s; }
scooter* order::getRescueScooter() const { return rescueScooter; }
void order::setFailed(bool f) { failed = f; }
bool order::isFailed() const { return failed; }

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
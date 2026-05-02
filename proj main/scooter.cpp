#include "scooter.h"
#include <iostream>
using namespace std;

scooter::scooter()
{
    ID = 0;
    speed = 0;
    mainDur = 0;
    ordersSinceMaintenance = 0;
    tripsDone = 0;
    maxTripsBeforeMaintenance = 3;
}

scooter::scooter(int id, int spd, int mDur)
{
    ID = id;
    speed = spd;
    mainDur = mDur;
    ordersSinceMaintenance = 0;
}

int scooter::getID() const
{
    return ID;
}

int scooter::getSpeed() const
{
    return speed;
}

int scooter::getMainDur() const
{
    return mainDur;
}

int scooter::getOrdersSinceMaintenance() const
{
    return ordersSinceMaintenance;
}

void scooter::setSpeed(int s)
{
    speed = s;
}

void scooter::incrementOrders()
{
    ordersSinceMaintenance++;
}

void scooter::resetOrders()
{
    ordersSinceMaintenance = 0;
}

void scooter::Print() const
{
    cout << "S" << ID;
}

ostream& operator<<(ostream& out, const scooter* pScooter)
{
    if (pScooter != nullptr)
    {
        out << "S" << pScooter->getID();
    }
    return out;
}

void scooter::incrementTrips()
{
    tripsDone++;
}

bool scooter::needsMaintenance() const
{
    return tripsDone >= maxTripsBeforeMaintenance;
}

void scooter::resetTrips()
{
    tripsDone = 0;
}
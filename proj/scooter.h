#pragma once
#include <iostream>
using namespace std;

class scooter
{
private:
    int ID;
    int speed;
    int mainDur;
    int ordersSinceMaintenance;
    int tripsDone;
    int maxTripsBeforeMaintenance;

public:
    scooter();
    scooter(int id, int spd, int mDur);
    int getSpeed() const;
    void incrementTrips();
    bool needsMaintenance() const;
    void resetTrips();

    int getID() const;
    int getMainDur() const;
    int getOrdersSinceMaintenance() const;

    void setSpeed(int s);
    void incrementOrders();
    void resetOrders();

    void Print() const;

    friend ostream& operator<<(ostream& out, const scooter* pScooter);
};


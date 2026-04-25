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

public:
    scooter();
    scooter(int id, int spd, int mDur);

    int getID() const;
    int getSpeed() const;
    int getMainDur() const;
    int getOrdersSinceMaintenance() const;

    void setSpeed(int s);
    void incrementOrders();
    void resetOrders();

    void Print() const;

    friend ostream& operator<<(ostream& out, const scooter* pScooter);
};


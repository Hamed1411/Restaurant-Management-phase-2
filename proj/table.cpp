#include "table.h"
#include <iostream>
using namespace std;

table::table()
{
    tableNum = 0;
    capacity = 0;
    freeSeats = 0;
}

table::table(int id, int cap)
{
    tableNum = id;
    capacity = cap;
    freeSeats = cap;
}

int table::getID() const
{
    return tableNum;
}

int table::getCapacity() const
{
    return capacity;
}

int table::getFreeSeats() const
{
    return freeSeats;
}

void table::setFreeSeats(int seats)
{
    freeSeats = seats;
}

void table::resetFreeSeats()
{
    freeSeats = capacity;
}

void table::Print() const
{
    cout << "[T" << tableNum << ", " << capacity << ", " << freeSeats << "]";
}

ostream& operator<<(ostream& out, const table* pTable)
{
    if (pTable != nullptr)
    {
        out << "[T" << pTable->getID()
            << ", " << pTable->getCapacity()
            << ", " << pTable->getFreeSeats() << "]";
    }
    return out;
}
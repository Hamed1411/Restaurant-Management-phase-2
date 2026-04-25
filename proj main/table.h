#pragma once
#include <iostream>
using namespace std;

class table
{
private:
    int tableNum;
    int capacity;
    int freeSeats;

public:
    table();
    table(int id, int cap);

    int getID() const;
    int getCapacity() const;
    int getFreeSeats() const;

    void setFreeSeats(int seats);
    void resetFreeSeats();

    void Print() const;

    friend ostream& operator<<(ostream& out, const table* pTable);
};

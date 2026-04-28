#pragma once
#include <iostream>
using namespace std;
// test 22 
enum CHEF_TYPE
{
    CS,
    CN
};

class chef
{
private:
    int ID;
    CHEF_TYPE type;
    int speed;

public:
    chef();
    chef(int id, CHEF_TYPE t, int s);

    int getID() const;
    CHEF_TYPE getType() const;
    int getSpeed() const;

    void setSpeed(int s);

    void Print() const;

    friend ostream& operator<<(ostream& out, const chef* pChef);
};
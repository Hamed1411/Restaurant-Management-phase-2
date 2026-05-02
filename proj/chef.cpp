#include "chef.h"
#include <iostream>
using namespace std;

chef::chef()
{
    ID = 0;
    type = CN;
    speed = 0;
}

chef::chef(int id, CHEF_TYPE t, int s)
{
    ID = id;
    type = t;
    speed = s;
}

int chef::getID() const
{
    return ID;
}

CHEF_TYPE chef::getType() const
{
    return type;
}

int chef::getSpeed() const
{
    return speed;
}

void chef::setSpeed(int s)
{
    speed = s;
}

void chef::Print() const
{
    cout << ID;
}

ostream& operator<<(ostream& out, const chef* pChef)
{
    if(pChef != nullptr)
    {
        out << "[" << pChef->getID() << "]";
	}
	return out;
}
#include "Restaurant.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
using namespace std;

int main()
{
    srand((unsigned)time(0));


    Restaurant myRest;
    myRest.InitializePhase1();
    myRest.GenerateRandomOrders(500);
    myRest.RunPhase1Simulation();

    system("pause");
    return 0;
}
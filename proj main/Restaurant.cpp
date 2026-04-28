#include "Restaurant.h"
#include "Action.h"
#include "CancelAction.h"
#include "RequestAction.h"
#include <cstdlib>
#include <ctime>

Restaurant::Restaurant()
{
    currentTime = 1;
    totalGeneratedOrders = 0;
}


int Restaurant::RandomInt(int minVal, int maxVal) const
{
    return minVal + rand() % (maxVal - minVal + 1);
}


void Restaurant::InitializePhase1()
{
    currentTime = 1;
    totalGeneratedOrders = 0;

    // Example chefs
    for (int i = 1; i <= 15; i++)
    {
        chef* c = new chef(100+i, CN, RandomInt(2, 5));
        Free_CN.enqueue(c);
    }

    for (int i = 1; i <= 15; i++)
    {
        chef* c = new chef(200 + i, CS, RandomInt(2, 5));
        Free_CS.enqueue(c);
    }

    // Example scooters
    for (int i = 1; i <= 10; i++)
    {
        scooter* s = new scooter(i, RandomInt(10, 20), RandomInt(2, 4));
        Free_Scooters.enqueue(s, 100 - i);
    }

    // Example tables
    int tableID = 1;
    int capacities[8] = { 2, 2, 4, 4, 6, 8, 10, 12 };

    for (int i = 0; i < 8; i++)
    {
        table* t = new table(tableID, capacities[i]);
        Free_Tables.enqueue(t, 100 - capacities[i]);
        tableID++;
    }
}


void Restaurant::GenerateRandomOrders(int count)
{
    totalGeneratedOrders = 0;

    for (int i = 0; i < count; i++)
    {
        int randomType = rand() % 6;
        ORDER_TYPE type = OT;

        if (randomType == 0)
            type = ODG;
        else if (randomType == 1)
            type = ODN;
        else if (randomType == 2)
            type = OT;
        else if (randomType == 3)
            type = OVC;
        else if (randomType == 4)
            type = OVN;
        else
            type = OVG;

        order* pOrd = new order(
            i + 1,   
            type,
            currentTime,
            RandomInt(1, 8),
            RandomInt(50, 300)
        );

        if (pOrd->isDelivery())
        {
            pOrd->setDistance(RandomInt(100, 2000));
        }

        if (pOrd->isDineIn())
        {
            pOrd->setSeats(RandomInt(1, 6));
            pOrd->setDuration(RandomInt(2, 6));
            pOrd->setCanShare(rand() % 2 == 0);
        }

        AddOrderToPending(pOrd);
        totalGeneratedOrders++;
    }
}

void Restaurant::ReadInputFile(string fileName)
{
    ifstream inFile(fileName);
    char actionType;

    while (inFile >> actionType)
    {
        if (actionType == 'R')
        {
            int TS, ID, size, money;
            char typeChar;

            inFile >> TS >> typeChar >> ID >> size >> money;

            ORDER_TYPE type = OT;

            if (typeChar == 'G')
                type = ODG;
            else if (typeChar == 'D')
                type = ODN;
            else if (typeChar == 'T')
                type = OT;
            else if (typeChar == 'V')
                type = OVG;
            else if (typeChar == 'N')
                type = OVN;
            else if (typeChar == 'C')
                type = OVC;

            Action* pAct = new RequestAction(TS, type, ID, size, money, this);
            ACTIONS_LIST.enqueue(pAct);
        }
        else if (actionType == 'X')
        {
            int TS, ID;
            inFile >> TS >> ID;

            Action* pAct = new CancelAction(TS, ID, this);
            ACTIONS_LIST.enqueue(pAct);
        }
    }
}


void Restaurant::ExecuteCurrentActions()
{
    Action* pAct = nullptr;

    while (ACTIONS_LIST.peek(pAct))
    {

        if (pAct->getActionTime() == currentTime) {
            ACTIONS_LIST.dequeue(pAct);
            pAct->ACT();
            delete pAct;
        }
        else {
            break; // No more actions for the current timestep
        }

    }
}


void Restaurant::AddOrderToPending(order* pOrd)
{
    switch (pOrd->getType())
    {
    case ODG:
        PEND_ODG.enqueue(pOrd);
        break;

    case ODN:
        PEND_ODN.enqueue(pOrd);
        break;

    case OT:
        PEND_OT.enqueue(pOrd);
        break;

    case OVG:
        PEND_OVG.enqueue(pOrd, static_cast<int>(pOrd->getPriority()));
        break;

    case OVN:
        PEND_OVN.enqueue(pOrd);
        break;

    case OVC:
        PEND_OVC.enqueue(pOrd);
        break;

    default:
        PEND_OT.enqueue(pOrd);
        break;
    }
}


void Restaurant::HandleCancelOrder(int id)
{
    order* pRemoved = nullptr;

    if (PEND_OVC.cancelOrderByID(id, pRemoved))
    {
        Cancelled_orders.enqueue(pRemoved);
        cout << "Successfully cancelled Order ID: " << id << endl;
    }
    else
    {
        cout << "Cancel Failed: Order " << id << " not found in Cancellable list." << endl;
    }
}


void Restaurant::Simulate()
{
    ReadInputFile("input.txt");

    while (!ACTIONS_LIST.isEmpty())
    {
        cout << "\n--- Timestep " << currentTime << " ---" << endl;

        ExecuteCurrentActions();
        OutputStatusBar();

        cout << "Press Enter to continue...";
        cin.get();

        currentTime++;
    }

    cout << "Simulation Finished." << endl;
}


void Restaurant::RunPhase1Simulation()
{
    while (!AllOrdersDone())
    {
        SimulateOneTimeStep();
        OutputStatusBar();
        cin.get();
        currentTime++;
    }
}


void Restaurant::SimulateOneTimeStep()
{
     MovePendingToCooking();
    MoveCookingToReady();
    MoveReadyToService();

    TryCancelPendingOVC();
    TryCancelReadyOVC();
    TryCancelCookingOV();

    MoveInServiceToFinish();
    HandleBackScooters();
    HandleMaintenanceScooters();
}


void Restaurant::MovePendingToCooking()
{
    for (int i = 0; i < 30; i++)
    {
        int choice = rand() % 6;
        order* pOrd = nullptr;
        bool done = false;

        switch (choice)
        {
        case 0:
            done = PEND_ODG.dequeue(pOrd);
            break;

        case 1:
            done = PEND_ODN.dequeue(pOrd);
            break;

        case 2:
            done = PEND_OT.dequeue(pOrd);
            break;

        case 3:
            done = PEND_OVN.dequeue(pOrd);
            break;

        case 4:
            done = PEND_OVC.dequeue(pOrd);
            break;

        case 5:
        {
            int pri = 0;
            done = PEND_OVG.dequeue(pOrd, pri);
            break;
        }
        }

        if (!done || pOrd == nullptr)
            continue;

        chef* pChef = nullptr;
        bool chefAssigned = false;

       
        if (pOrd->isGrilled())
        {
            chefAssigned = Free_CS.dequeue(pChef);
        }
        else
        {
            if (rand() % 2 == 0)
                chefAssigned = Free_CN.dequeue(pChef);
            else
                chefAssigned = Free_CS.dequeue(pChef);

            if (!chefAssigned)
                chefAssigned = Free_CN.dequeue(pChef);

            if (!chefAssigned)
                chefAssigned = Free_CS.dequeue(pChef);
        }

        if (!chefAssigned || pChef == nullptr)
        {
            AddOrderToPending(pOrd);
            continue;
        }

        pOrd->setChef(pChef);   
        pOrd->setTA(currentTime);
        pOrd->setTR(currentTime + RandomInt(1, 4));

        Cooking_Orders.enqueue(pOrd, 100 - pOrd->getTR());
    }
}


void Restaurant::MoveCookingToReady()
{

    PriQueueWithCancel tempQueue;

    order* pOrd = nullptr;
    int pri = 0;
    int movedCount = 0;

    while (Cooking_Orders.dequeue(pOrd, pri))
    {
        if (pOrd == nullptr)
            continue;

        
        if (pOrd->getTR() <= currentTime )
        {
            chef* pChef = pOrd->getChef();

            if (pChef != nullptr)
            {
                if (pChef->getType() == CS)
                    Free_CS.enqueue(pChef);
                else
                    Free_CN.enqueue(pChef);
            }

            if (pOrd->isTakeaway())
            {
                pOrd->setTF(currentTime); // Finish time is exactly now
                Finished_orders.push(pOrd); // Move directly to finished
            }
            else if (pOrd->isDineIn())
            {
                RDY_OD.enqueue(pOrd);
            }
            else
            {
                RDY_OV_List.enqueue(pOrd);
            }

            movedCount++;
        }
        else
        {
            
            tempQueue.enqueue(pOrd, pri);
        }
    }

    // restore remaining cooking orders
    while (tempQueue.dequeue(pOrd, pri))
    {
        Cooking_Orders.enqueue(pOrd, pri);
    }
}


void Restaurant::MoveReadyToService()
{
    for (int i = 0; i < 10; i++)
    {
        order* pOrd = nullptr;

        //if (RDY_OT.dequeue(pOrd))
        //{
        //    if (pOrd != nullptr)
        //    {
        //        pOrd->setTS(currentTime);
        //        pOrd->setTF(currentTime + 1);
        //        Finished_orders.push(pOrd);
        //    }
        //    continue;
        //}

        if (RDY_OD.dequeue(pOrd))
        {
            if (pOrd != nullptr)
            {
                table* pTable = nullptr;

                int neededSeats = pOrd->getSeats();
                if (neededSeats <= 0)
                    neededSeats = 1;

                if (Free_Tables.getBest(neededSeats, pTable))
                {
                    pOrd->setTS(currentTime);

                    int duration = pOrd->getDuration();
                    if (duration <= 0)
                        duration = RandomInt(2, 5);

                    pOrd->setTF(currentTime + duration);

                    Busy_No_Share.enqueue(pTable, 100 - pTable->getCapacity());
                    InServ_Orders.enqueue(pOrd, 100 - pOrd->getTF());
                }
                else
                {
                    RDY_OD.enqueue(pOrd);
                }
            }
            continue;
        }

        if (RDY_OV_List.dequeue(pOrd))
        {
            if (pOrd != nullptr)
            {
                scooter* pScooter = nullptr;
                int pri = 0;

                if (Free_Scooters.dequeue(pScooter, pri))
                {
                    pOrd->setTS(currentTime);
                    pOrd->setTF(currentTime + RandomInt(2, 6));

                    Back_Scooters.enqueue(pScooter, RandomInt(1, 50));
                    InServ_Orders.enqueue(pOrd, 100 - pOrd->getTF());
                }
                else
                {
                    RDY_OV_List.enqueue(pOrd);
                }
            }
        }
    }
}


void Restaurant::TryCancelPendingOVC()
{
    if (totalGeneratedOrders <= 0)
        return;

    int id = RandomInt(1, totalGeneratedOrders);
    order* pOrd = nullptr;

    if (PEND_OVC.cancelOrderByID(id, pOrd))
    {
        if (pOrd != nullptr)
            Cancelled_orders.enqueue(pOrd);
    }
}


void Restaurant::TryCancelReadyOVC()
{
    if (totalGeneratedOrders <= 0)
        return;

    int id = RandomInt(1, totalGeneratedOrders);
    order* pOrd = nullptr;

    if (RDY_OV_List.cancelOrderByID(id, pOrd))
    {
        if (pOrd != nullptr)
        {
            if (pOrd->getType() == OVC)
                Cancelled_orders.enqueue(pOrd);
            else
                RDY_OV_List.enqueue(pOrd);
        }
    }
}


void Restaurant::TryCancelCookingOV()
{
    if (totalGeneratedOrders <= 0)
        return;

    int id = RandomInt(1, totalGeneratedOrders);
    order* pOrd = nullptr;

    if (Cooking_Orders.cancelOrderByID(id, pOrd))
    {
        if (pOrd != nullptr)
        {
            chef* pChef = pOrd->getChef();
            if (pChef != nullptr)
            {
                if (pChef->getType() == CS)
                    Free_CS.enqueue(pChef);
                else
                    Free_CN.enqueue(pChef);
            }

            if (pOrd->isDelivery())
                Cancelled_orders.enqueue(pOrd);
            else
                Cooking_Orders.enqueue(pOrd, 100 - pOrd->getTR());
        }
    }
}


void Restaurant::MoveInServiceToFinish()
{
    if (rand() % 100 >= 25)
        return;

    order* pOrd = nullptr;
    int pri = 0;

    if (InServ_Orders.dequeue(pOrd, pri))
    {
        if (pOrd != nullptr)
        {
            pOrd->setTF(currentTime);
            Finished_orders.push(pOrd);

            if (pOrd->isDineIn())
            {
                table* pTable = nullptr;
                int tablePri = 0;

                if (Busy_No_Share.dequeue(pTable, tablePri))
                {
                    if (pTable != nullptr)
                    {
                        pTable->resetFreeSeats();
                        Free_Tables.enqueue(pTable, 100 - pTable->getCapacity());
                    }
                }
            }
        }
    }
}


void Restaurant::HandleBackScooters()
{
    if (rand() % 100 >= 50)
        return;

    scooter* pScooter = nullptr;
    int pri = 0;

    if (Back_Scooters.dequeue(pScooter, pri))
    {
        if (pScooter != nullptr)
        {
            if (rand() % 2 == 0)
                Free_Scooters.enqueue(pScooter, 100 - pScooter->getID());
            else
                Maint_Scooters.enqueue(pScooter);
        }
    }
}


void Restaurant::HandleMaintenanceScooters()
{
    if (rand() % 100 >= 50)
        return;

    scooter* pScooter = nullptr;

    if (Maint_Scooters.dequeue(pScooter))
    {
        if (pScooter != nullptr)
            Free_Scooters.enqueue(pScooter, 100 - pScooter->getID());
    }
}


bool Restaurant::AllOrdersDone() const
{
    return Finished_orders.getCount() + Cancelled_orders.getCount() >= totalGeneratedOrders;
}


void Restaurant::OutputStatusBar()
{
    cout << "Current Timestep:" << currentTime << endl;

    cout << "================ Actions List ================\n";
    cout << "For reQuest action: print [Order Type, TQ, order ID], For cancel print (X, Tcancel, order ID)\n";
    cout << "Random simulator mode: no input file actions loaded\n";
    cout << "--> Print ONLY the first 10 actions currently in the actions list\n\n";

    cout << "------------- Pending Orders IDs -----------------\n";
    cout << "For each pending list print\n";
    cout << "List count, order type, IDs of all orders in the list\n";

    cout << PEND_ODG.getCount() << " ODG: ";
    PEND_ODG.print();
    cout << endl;

    cout << PEND_ODN.getCount() << " ODN: ";
    PEND_ODN.print();
    cout << endl;

    cout << PEND_OT.getCount() << " OT: ";
    PEND_OT.print();
    cout << endl;

    cout << PEND_OVC.getCount() << " OVC: ";
    PEND_OVC.print();
    cout << endl;

    cout << PEND_OVN.getCount() << " OVN: ";
    PEND_OVN.print();
    cout << endl;

    cout << PEND_OVG.getCount() << " OVG: ";
    PEND_OVG.print();
    cout << endl << endl;

    cout << "------------- Available chefs IDs -----------------\n";
    cout << Free_CS.getCount() << " CS: ";
    Free_CS.print();
    cout << endl;

    cout << Free_CN.getCount() << " CN: ";
    Free_CN.print();
    cout << endl << endl;

    cout << "------------- Cooking orders [Orders ID, chef ID] -----------------\n";
    cout << Cooking_Orders.getCount() << " cooking orders: ";

    PriQueueWithCancel tempQueue;
    order* pOrd = nullptr;
    int pri = 0;

    while (Cooking_Orders.dequeue(pOrd, pri))
    {
        if (pOrd != nullptr)
        {
            if (pOrd->getChef() != nullptr)
            {
                cout << "[" << pOrd->getID() << ", " << pOrd->getChef()->getID() << "]";
            }
            else
            {
                cout << "[" << pOrd->getID() << ", " << "NoChef" << "]";
            }

            cout << ", ";
            tempQueue.enqueue(pOrd, pri);
        }
    }

    while (tempQueue.dequeue(pOrd, pri))
    {
        Cooking_Orders.enqueue(pOrd, pri);
    }

    cout << endl << endl;

    cout << "------------- Ready Orders IDs -----------------\n";
    cout << "For each Ready list print\n";
    cout << "List count, order type, IDs of all orders in the list\n";

    cout << RDY_OD.getCount() << " RDY_OD: ";
    RDY_OD.print();
    cout << endl;

    //cout << RDY_OT.getCount() << " RDY_OT: ";
    //RDY_OT.print();
    //cout << endl;

    cout << RDY_OV_List.getCount() << " RDY_OV: ";
    RDY_OV_List.print();
    cout << endl << endl;

    cout << "------------- Available scooters IDs -----------------\n";
    cout << Free_Scooters.getCount() << " Scooters: ";
    Free_Scooters.print();
    cout << endl << endl;

    cout << "------------- Available tables [ID, capacity, free seats] -----------------\n";
    cout << Free_Tables.getCount() << " tables: ";
    Free_Tables.print();
    cout << endl << endl;

    cout << "------------- In-Service orders [order ID, scooter/Table ID] -----------------\n";
    cout << InServ_Orders.getCount() << " Orders: ";
    InServ_Orders.print();
    cout << endl << endl;

    cout << "------------- In-maintainance scooters IDs -----------------\n";
    cout << Maint_Scooters.getCount() << " scooters: ";
    Maint_Scooters.print();
    cout << endl << endl;

    cout << "------------- Scooters Back to Restaurant IDs -----------------\n";
    cout << Back_Scooters.getCount() << " scooters: ";
    Back_Scooters.print();
    cout << endl << endl;

    cout << "------------- Cancelled Orders IDs -----------------\n";
    cout << Cancelled_orders.getCount() << " cancelled: ";
    Cancelled_orders.print2();
    cout << endl << endl;

    cout << "------------- Finished orders IDs -----------------\n";
    cout << Finished_orders.getCount() << " Orders: ";
    Finished_orders.printStack();
    cout << endl;

    cout << "PRESS ANY KEY TO MOVE TO NEXT STEP!" << endl;
}

Restaurant::~Restaurant()
{
}
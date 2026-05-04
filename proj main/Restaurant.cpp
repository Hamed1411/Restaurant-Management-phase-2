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
int Restaurant::CalculateDeliveryServiceTime(order* pOrd, scooter* pScooter)
{
    if (pOrd == nullptr || pScooter == nullptr)
        return 1;

    int distance = pOrd->getDistance();
    int speed = pScooter->getSpeed();

    if (speed <= 0)
        return 1;

    int time = distance / speed;

    if (distance % speed != 0)
        time++;

    return time;
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

    if (!inFile)
    {
        cout << "Error: input file not found." << endl;
        return;
    }

    int numCN, numCS;
    int speedCN, speedCS;
    int scooterCount, scooterSpeed;
    int mainOrds, mainDur;
    int totalTables;
    int tableCount, capacity;
    int TH;
    int M;

    inFile >> numCN >> numCS;
    inFile >> speedCN >> speedCS;
    inFile >> scooterCount >> scooterSpeed;
    inFile >> mainOrds >> mainDur;
    inFile >> totalTables;

    int tableID = 1;
    int loadedTables = 0;

    while (loadedTables < totalTables)
    {
        inFile >> tableCount >> capacity;

        for (int i = 0; i < tableCount; i++)
        {
            table* t = new table(tableID, capacity);
            Free_Tables.enqueue(t, 100 - capacity);
            tableID++;
            loadedTables++;
        }
    }

    inFile >> TH;
    inFile >> M;

    for (int i = 1; i <= numCN; i++)
    {
        chef* c = new chef(100 + i, CN, speedCN);
        Free_CN.enqueue(c);
    }

    for (int i = 1; i <= numCS; i++)
    {
        chef* c = new chef(200 + i, CS, speedCS);
        Free_CS.enqueue(c);
    }

    for (int i = 1; i <= scooterCount; i++)
    {
        scooter* s = new scooter(i, scooterSpeed, mainOrds);
        Free_Scooters.enqueue(s, 100 - i);
    }

    for (int i = 0; i < M; i++)
    {
        char actionType;
        inFile >> actionType;

        if (actionType == 'Q')
        {
            string typeText;
            int TQ, ID, size, price;

            inFile >> typeText >> TQ >> ID >> size >> price;

            ORDER_TYPE type = OT;

            if (typeText == "ODG")
                type = ODG;
            else if (typeText == "ODN")
                type = ODN;
            else if (typeText == "OT")
                type = OT;
            else if (typeText == "OVC")
                type = OVC;
            else if (typeText == "OVG")
                type = OVG;
            else if (typeText == "OVN")
                type = OVN;

            order* pOrd = new order(ID, type, TQ, size, price);

            if (pOrd->isDineIn())
            {
                int seats, duration;
                char canShareChar;

                inFile >> seats >> duration >> canShareChar;

                pOrd->setSeats(seats);
                pOrd->setDuration(duration);
                pOrd->setCanShare(canShareChar == 'Y' || canShareChar == 'y');
            }
            else if (pOrd->isDelivery())
            {
                int distance;
                inFile >> distance;
                pOrd->setDistance(distance);
            }

            Action* pAct = new RequestAction(TQ, pOrd, this);

            // Important:
            // If your RequestAction cannot store pOrd extra fields yet,
            // then later you need to update RequestAction to pass seats/distance.
            ACTIONS_LIST.enqueue(pAct);
        }
        else if (actionType == 'X')
        {
            int Tcancel, ID;
            inFile >> Tcancel >> ID;

            Action* pAct = new CancelAction(Tcancel, ID, this);
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
        return;
    }

    if (RDY_OV_List.cancelOrderByID(id, pRemoved))
    {
        if (pRemoved != nullptr && pRemoved->getType() == OVC)
        {
            Cancelled_orders.enqueue(pRemoved);
            return;
        }

        if (pRemoved != nullptr)
            RDY_OV_List.enqueue(pRemoved);
    }

    if (Cooking_Orders.cancelOrderByID(id, pRemoved))
    {
        if (pRemoved != nullptr && pRemoved->getType() == OVC)
        {
            chef* pChef = pRemoved->getChef();

            if (pChef != nullptr)
            {
                if (pChef->getType() == CS)
                    Free_CS.enqueue(pChef);
                else
                    Free_CN.enqueue(pChef);
            }

            Cancelled_orders.enqueue(pRemoved);
            return;
        }

        if (pRemoved != nullptr)
            Cooking_Orders.enqueue(pRemoved, pRemoved->getTR());
    }

    priQueue<order*> tempInServ;
    order* pOrd = nullptr;
    int pri = 0;
    bool found = false;

    while (InServ_Orders.dequeue(pOrd, pri))
    {
        if (pOrd != nullptr && pOrd->getID() == id && pOrd->getType() == OVC)
        {
            Cancelled_orders.enqueue(pOrd);
            found = true;
        }
        else
        {
            tempInServ.enqueue(pOrd, pri);
        }
    }

    while (tempInServ.dequeue(pOrd, pri))
    {
        InServ_Orders.enqueue(pOrd, pri);
    }

    if (found)
        return;
}
bool Restaurant::SimulationFinished() const
{
    return ACTIONS_LIST.isEmpty()
        && PEND_ODG.isEmpty()
        && PEND_ODN.isEmpty()
        && PEND_OT.isEmpty()
        && PEND_OVC.isEmpty()
        && PEND_OVN.isEmpty()
        && PEND_OVG.isEmpty()
        && Cooking_Orders.isEmpty()
        && RDY_OD.isEmpty()
        && RDY_OT.isEmpty()
        && RDY_OV_List.isEmpty()
        && InServ_Orders.isEmpty();
}


void Restaurant::Simulate()
{
    while (!SimulationFinished())
    {
        ExecuteCurrentActions();

        HandleBackScooters();
        HandleMaintenanceScooters();

        MoveCookingToReady();
        MoveInServiceToFinish();

        MoveReadyToService();
        MovePendingToCooking();

        OutputStatusBar();

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
    chef* pChef = nullptr;
    order* pOrd = nullptr;

    // Assign OD orders
   
    // ODG -> CS only
    while (!PEND_ODG.isEmpty() && Free_CS.peek(pChef)) {
        Free_CS.dequeue(pChef);
        PEND_ODG.dequeue(pOrd);
        BindOrderToChef(pOrd, pChef);
    }


    // ODN -> CN, then CS
    while (!PEND_ODN.isEmpty()) {
        if (Free_CN.dequeue(pChef) || Free_CS.dequeue(pChef)) {
            PEND_ODN.dequeue(pOrd);
            BindOrderToChef(pOrd, pChef);
        }
        else break;
    }

    //Assign OT orders -> CN only

    while (!PEND_OT.isEmpty() && Free_CN.peek(pChef)) {
        Free_CN.dequeue(pChef);
        PEND_OT.dequeue(pOrd);
        BindOrderToChef(pOrd, pChef);
    }

    // Assign OV orders
     
    // OVG -> CS only 
    while (!PEND_OVG.isEmpty() && Free_CS.peek(pChef)) {
        int pri;
        Free_CS.dequeue(pChef);
        PEND_OVG.dequeue(pOrd, pri);
        BindOrderToChef(pOrd, pChef);
    }

    // OVC -> CN, then CS
    while (!PEND_OVC.isEmpty()) {
        if (Free_CN.dequeue(pChef) || Free_CS.dequeue(pChef)) {
            PEND_OVC.dequeue(pOrd);
            BindOrderToChef(pOrd, pChef);
        }
        else break;
    }

    // OVN -> CN only
    while (!PEND_OVN.isEmpty() && Free_CN.peek(pChef)) {
        Free_CN.dequeue(pChef);
        PEND_OVN.dequeue(pOrd);
        BindOrderToChef(pOrd, pChef);
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
                pOrd->setTR(currentTime); 
                RDY_OT.enqueue(pOrd); 
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

        if (RDY_OT.dequeue(pOrd))
        {
            if (pOrd != nullptr)
            {
                pOrd->setTS(currentTime);
                pOrd->setTF(currentTime + 1);
                Finished_orders.push(pOrd);
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
                    int serviceTime = CalculateDeliveryServiceTime(pOrd, pScooter);

                    pOrd->setTS(currentTime);
                    pOrd->setTF(currentTime + serviceTime);

                    pOrd->setScooter(pScooter);

                    InServ_Orders.enqueue(pOrd, 100 - pOrd->getTF());


                    Back_Scooters.enqueue(pScooter, 100 - pOrd->getTF());
                }
                else
                {

                    RDY_OV_List.enqueue(pOrd);
                }
            }
            continue;
        }

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
                    pOrd->setTable(pTable);
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
    priQueue<order*> tempQueue;

    order* pOrd = nullptr;
    int pri = 0;

    while (InServ_Orders.dequeue(pOrd, pri))
    {
        if (pOrd == nullptr)
            continue;

        if (pOrd->getTF() <= currentTime)
        {
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
        else
        {
            tempQueue.enqueue(pOrd, pri);
        }
    }

    while (tempQueue.dequeue(pOrd, pri))
    {
        InServ_Orders.enqueue(pOrd, pri);
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
            pScooter->incrementTrips();

            if (pScooter->needsMaintenance())
            {
                pScooter->resetTrips();
                Maint_Scooters.enqueue(pScooter);
            }
            else
            {
                Free_Scooters.enqueue(pScooter, 100 - pScooter->getID());
            }
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

// new //
void Restaurant::BindOrderToChef(order* pOrd, chef* pChef)
{
    pChef->setBusy(true);
    pOrd->setChef(pChef);

    // Set Assigned Time (TA) and calculate Wait Time (TW)
    pOrd->setTA(currentTime);
    pOrd->setTW(currentTime - pOrd->getTQ());

    // Calculate Cook Time
    // If a Normal chef takes a Grilled order, speed is halved
    int speed = pChef->getSpeed();
    if (pChef->getType() == CN && pOrd->isGrilled()) {
        speed /= 2; 
    }

    // Ensure speed is at least 1 to avoid division by zero or negative cook times
    if (speed < 1) speed = 1;

    int cookTime = pOrd->getDuration() / speed;

    // CookTime should be at least 1 timestep
    if (cookTime < 1) cookTime = 1;

    // Set Ready Time (TR) 
    pOrd->setTR(currentTime + cookTime);

   
    Cooking_Orders.enqueue(pOrd, pOrd->getTR());
}

void Restaurant::OutputStatusBar()
{
    cout << "Current Timestep:" << currentTime << endl;

    cout << "================ Actions List ================\n";
    cout << "For reQuest action: print [Order Type, TQ, order ID], For cancel print (X, Tcancel, order ID)\n";
    cout << "Phase 2 input-file simulation mode\n";
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
    if (PEND_OVG.isEmpty())
        cout << "The list is empty.";
    else
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

    if (Cooking_Orders.isEmpty())
    {
        cout << "The list is empty.";
    }
    else
    {
        PriQueueWithCancel tempQueue;
        order* pOrd = nullptr;
        int pri = 0;
        bool firstCooking = true;

        while (Cooking_Orders.dequeue(pOrd, pri))
        {
            if (pOrd != nullptr)
            {
                if (!firstCooking)
                    cout << ", ";

                if (pOrd->getChef() != nullptr)
                    cout << "[" << pOrd->getID() << ", " << pOrd->getChef()->getID() << "]";
                else
                    cout << "[" << pOrd->getID() << ", NoChef]";

                firstCooking = false;
                tempQueue.enqueue(pOrd, pri);
            }
        }

        while (tempQueue.dequeue(pOrd, pri))
        {
            Cooking_Orders.enqueue(pOrd, pri);
        }
    }

    cout << endl << endl;

    cout << "------------- Ready Orders IDs -----------------\n";
    cout << "For each Ready list print\n";
    cout << "List count, order type, IDs of all orders in the list\n";

    cout << RDY_OD.getCount() << " RDY_OD: ";
    RDY_OD.print();
    cout << endl;

    cout << RDY_OT.getCount() << " RDY_OT: ";
    RDY_OT.print();
    cout << endl;

    cout << RDY_OV_List.getCount() << " RDY_OV: ";
    RDY_OV_List.print();
    cout << endl << endl;

    cout << "------------- Available scooters IDs -----------------\n";
    cout << Free_Scooters.getCount() << " Scooters: ";
    if (Free_Scooters.isEmpty())
        cout << "The list is empty.";
    else
        Free_Scooters.print();
    cout << endl << endl;

    cout << "------------- Available tables [ID, capacity, free seats] -----------------\n";
    cout << Free_Tables.getCount() << " tables: ";
    if (Free_Tables.isEmpty())
        cout << "The list is empty.";
    else
        Free_Tables.print();
    cout << endl << endl;

    cout << "------------- In-Service orders [order ID, scooter/Table ID] -----------------\n";
    cout << InServ_Orders.getCount() << " Orders: ";

    if (InServ_Orders.isEmpty())
    {
        cout << "The list is empty.";
    }
    else
    {
        priQueue<order*> tempInServ;
        order* pServOrd = nullptr;
        int servPri = 0;
        bool first = true;

        while (InServ_Orders.dequeue(pServOrd, servPri))
        {
            if (pServOrd != nullptr)
            {
                if (!first)
                    cout << ", ";

                cout << "[" << pServOrd->getID();

                if (pServOrd->isDelivery() && pServOrd->getScooter())
                    cout << ", S" << pServOrd->getScooter()->getID();
                else if (pServOrd->isDineIn() && pServOrd->getTable())
                    cout << ", T" << pServOrd->getTable()->getID();

                cout << "]";

                first = false;
                tempInServ.enqueue(pServOrd, servPri);
            }
        }

        while (tempInServ.dequeue(pServOrd, servPri))
        {
            InServ_Orders.enqueue(pServOrd, servPri);
        }
    }

    cout << endl << endl;

    cout << "------------- In-maintainance scooters IDs -----------------\n";
    cout << Maint_Scooters.getCount() << " scooters: ";
    if (Maint_Scooters.isEmpty())
        cout << "The list is empty.";
    else
        Maint_Scooters.print();
    cout << endl << endl;

    cout << "------------- Scooters Back to Restaurant IDs -----------------\n";
    cout << Back_Scooters.getCount() << " scooters: ";
    if (Back_Scooters.isEmpty())
        cout << "The list is empty.";
    else
        Back_Scooters.print();
    cout << endl << endl;

    cout << "------------- Cancelled Orders IDs -----------------\n";
    cout << Cancelled_orders.getCount() << " cancelled: ";
    if (Cancelled_orders.isEmpty())
        cout << "The list is empty.";
    else
        Cancelled_orders.print2();
    cout << endl << endl;

    cout << "------------- Finished orders IDs -----------------\n";
    cout << Finished_orders.getCount() << " Orders: ";
    if (Finished_orders.isEmpty())
        cout << "The list is empty.";
    else
        Finished_orders.printStack();
    cout << endl << endl;
}


Restaurant::~Restaurant()
{
}
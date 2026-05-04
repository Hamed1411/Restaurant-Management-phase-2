#include "Restaurant.h"
#include "Action.h"
#include "CancelAction.h"
#include "RequestAction.h"
#include <cstdlib>
Restaurant::Restaurant()
{
    currentTime = 1;
    totalGeneratedOrders = 0;
    totalCN = 0;
    totalCS = 0;
    totalScooters = 0;
    overwaitThreshold = 0;
    comboCount = 0;
    rescueCount = 0;
    totalOverwaitOVG = 0;
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
    totalCN = numCN;
    totalCS = numCS;
    inFile >> speedCN >> speedCS;
    inFile >> scooterCount >> scooterSpeed;
    totalScooters = scooterCount;
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
    overwaitThreshold = TH;
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
            else if (typeText == "OVB")
                type = OVB;

            order* pOrd = new order(ID, type, TQ, size, price);

            if (pOrd->isCombo())
            {
                int dist, nChefs, nScooters;
                inFile >> dist >> nChefs >> nScooters;
                pOrd->setDistance(dist);
                pOrd->setNumChefsRequired(nChefs);
                pOrd->setNumScootersRequired(nScooters);
            }
            else if (pOrd->isDineIn())
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

void Restaurant::GenerateOutputFile(string fileName)
{
    ofstream outFile(fileName);
    if (!outFile)
    {
        cout << "Error: Cannot open output file." << endl;
        return;
    }

    // Move Finished_orders into an array for sorting
    int finishedCount = Finished_orders.getCount();
    order** sortedOrders = new order * [finishedCount];

    order* pOrd;
    int idx = 0;
    while (Finished_orders.pop(pOrd))
    {
        sortedOrders[idx++] = pOrd;
    }

    // Sort descending by TF
    for (int i = 0; i < finishedCount - 1; i++)
    {
        for (int j = i + 1; j < finishedCount; j++)
        {
            if (sortedOrders[i]->getTF() < sortedOrders[j]->getTF())
            {
                order* temp = sortedOrders[i];
                sortedOrders[i] = sortedOrders[j];
                sortedOrders[j] = temp;
            }
        }
    }

    outFile << "TF\tID\tTQ\tTA\tTR\tTS\tTi\tTc\tTw\tTserv\tMission_Info\n";

    int countODG = 0, countODN = 0, countOT = 0, countOVC = 0, countOVG = 0, countOVN = 0, countOVB = 0;
    double sumTi = 0, sumTc = 0, sumTw = 0, sumTserv = 0;
    double sumCookTimeAllChefs = 0;
    double sumScooterServiceTime = 0;
    int overwaitCount = 0;

    for (int i = 0; i < finishedCount; i++)
    {
        pOrd = sortedOrders[i];

        int tf = pOrd->getTF();
        int id = pOrd->getID();
        int tq = pOrd->getTQ();
        int ta = pOrd->getTA();
        int tr = pOrd->getTR();
        int ts = pOrd->getTS();
        int ti = pOrd->getIdleTime();
        int tc = pOrd->getCookPeriod();
        int tw = pOrd->getWaitTime();
        int tserv = pOrd->getServiceDuration();

        outFile << tf << "\t" << id << "\t" << tq << "\t" << ta << "\t"
            << tr << "\t" << ts << "\t" << ti << "\t" << tc << "\t"
            << tw << "\t" << tserv << "\t";

        string info = "";
        if (pOrd->isCombo()) info += "[COMBO] ";
        if (pOrd->getType() == OVG && (pOrd->getTS() - pOrd->getTR() > overwaitThreshold)) info += "[OVERWAIT] ";
        if (pOrd->isRescueMission()) info += "[RESCUED by S" + to_string(pOrd->getRescueScooter()->getID()) + "] ";
        
        if (info == "") info = "Normal";
        outFile << info << "\n";

        // Aggregate statistics
        sumTi += ti;
        sumTc += tc;
        sumTw += tw;
        sumTserv += tserv;

        sumCookTimeAllChefs += tc;
        if (pOrd->isDelivery())
        {
            sumScooterServiceTime += tserv; // TS to TF for scooters
        }

        if (tw > overwaitThreshold)
            overwaitCount++;

        ORDER_TYPE type = pOrd->getType();
        if (type == ODG) countODG++;
        else if (type == ODN) countODN++;
        else if (type == OT) countOT++;
        else if (type == OVC) countOVC++;
        else if (type == OVG) countOVG++;
        else if (type == OVN) countOVN++;
        else if (type == OVB) countOVB++;
    }

    // Cancelled orders breakdown
    int cancelledCount = Cancelled_orders.getCount();
    order* cOrd;
    while (Cancelled_orders.dequeue(cOrd))
    {
        ORDER_TYPE type = cOrd->getType();
        if (type == ODG) countODG++;
        else if (type == ODN) countODN++;
        else if (type == OT) countOT++;
        else if (type == OVC) countOVC++;
        else if (type == OVG) countOVG++;
        else if (type == OVN) countOVN++;
        else if (type == OVB) countOVB++;
        delete cOrd;
    }

    int totalOrders = finishedCount + cancelledCount;

    outFile << "\n------------------------------------------------------------\n";
    outFile << "1- Total Orders: " << totalOrders << " [";
    outFile << "ODG:" << countODG << ", ODN:" << countODN << ", OT:" << countOT
        << ", OVC:" << countOVC << ", OVG:" << countOVG << ", OVN:" << countOVN << ", OVB:" << countOVB << "]\n";

    int totalChefs = totalCN + totalCS;
    outFile << "2- Total Chefs: " << totalChefs << " [CN:" << totalCN << ", CS:" << totalCS << "]\n";
    outFile << "3- Total Scooters: " << totalScooters << " (All one type)\n";

    double pctFinished = totalOrders > 0 ? (double)finishedCount / totalOrders * 100.0 : 0;
    double pctCancelled = totalOrders > 0 ? (double)cancelledCount / totalOrders * 100.0 : 0;
    outFile << "4- Percentage of Finished orders: " << pctFinished << "%, Cancelled orders: " << pctCancelled << "%\n";

    double pctOverwait = finishedCount > 0 ? (double)overwaitCount / finishedCount * 100.0 : 0;
    outFile << "5- Percentage of overwait orders (Tw > TH): " << pctOverwait << "%\n";

    double avgTi = finishedCount > 0 ? sumTi / finishedCount : 0;
    double avgTc = finishedCount > 0 ? sumTc / finishedCount : 0;
    double avgTw = finishedCount > 0 ? sumTw / finishedCount : 0;
    double avgTserv = finishedCount > 0 ? sumTserv / finishedCount : 0;
    outFile << "6- Average for finished orders -> Ti: " << avgTi << ", Tc: " << avgTc
        << ", Tw: " << avgTw << ", Tserv: " << avgTserv << "\n";

    outFile << "7- Total COMBO orders: " << comboCount << "\n";
    outFile << "8- Total Rescue missions: " << rescueCount << "\n";
    outFile << "9- Total Overwait OVG (Ready-to-Service wait > TH): " << totalOverwaitOVG << "\n";

    int finalTime = currentTime;
    if (finalTime <= 0) finalTime = 1;

    double scooterUtil = totalScooters > 0 ? (sumScooterServiceTime / (finalTime * totalScooters)) * 100.0 : 0;
    double chefUtil = totalChefs > 0 ? (sumCookTimeAllChefs / (finalTime * totalChefs)) * 100.0 : 0;

    outFile << "7- Scooters utilization %: " << scooterUtil << "%\n";
    outFile << "8- Chefs utilization %: " << chefUtil << "%\n";

    outFile.close();

    for (int i = 0; i < finishedCount; i++)
        delete sortedOrders[i];
    delete[] sortedOrders;
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

    case OVB:
        PEND_COMBO.enqueue(pOrd, static_cast<int>(pOrd->getPriority()));
        comboCount++;
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


void Restaurant::Simulate(int mode)
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

        if (mode == 1)
        {
            cout << "Press ENTER to move to next step..." << endl;
      cin.ignore(1000, '\n');
        }

        currentTime++;
    }

    cout << "Simulation Finished." << endl;
    cout << "Generating Output File..." << endl;

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
    int pri = 0;

    // Assign COMBO orders (Highest priority in any stage)
    while (PEND_COMBO.peek(pOrd, pri)) {
        // COMBO needs up to 4 chefs, at least one CS
        int needed = pOrd->getNumChefsRequired();
        int currentlyAssigned = pOrd->getChefsCount();
        int remaining = needed - currentlyAssigned;

        
        bool hasCS = false;
        for (int i = 0; i < currentlyAssigned; i++) {
            if (pOrd->getChef(i)->getType() == CS) {
                hasCS = true;
                break;
            }
        }

        while (remaining > 0) {
            if (!hasCS) {
                if (Free_CS.dequeue(pChef)) {
                    BindOrderToChef(pOrd, pChef);
                    hasCS = true;
                    remaining--;
                } else if (remaining > 1) { 
                    
                    if (Free_CN.dequeue(pChef)) {
                        BindOrderToChef(pOrd, pChef);
                        remaining--;
                    } else break;
                } else break; 
            } else {
               
                if (Free_CN.dequeue(pChef) || Free_CS.dequeue(pChef)) {
                    BindOrderToChef(pOrd, pChef);
                    remaining--;
                } else break;
            }
        }

        if (pOrd->getChefsCount() == needed) {
            PEND_COMBO.dequeue(pOrd, pri);
            cout << ">>> COMBO Order " << pOrd->getID() << " assigned to " << needed << " chefs: ";
            for (int j = 0; j < needed; j++) cout << pOrd->getChef(j)->getID() << (j < needed - 1 ? ", " : "");
            cout << endl;
        } else {
            break; // Not enough chefs for this combo order yet
        }
    }

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
            // Release all chefs assigned to this order
            for (int i = 0; i < pOrd->getChefsCount(); i++) {
                chef* c = pOrd->getChef(i);
                c->setBusy(false);
                if (c->getType() == CS)
                    Free_CS.enqueue(c);
                else
                    Free_CN.enqueue(c);
            }

            if (pOrd->isCombo())
            {
                RDY_COMBO.enqueue(pOrd);
            }
            else if (pOrd->isTakeaway())
            {
                pOrd->setTR(currentTime + 1);  // takeaway wait 1 timestep before ready
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
    // Check RDY_OV_List for OVG orders that became overwait
    order* pOvCheck = nullptr;
    QueueWithCancel tempCheck;
    while (RDY_OV_List.dequeue(pOvCheck)) {
        if (pOvCheck->getType() == OVG && (currentTime - pOvCheck->getTR() > overwaitThreshold)) {
            // It's overwait! Move to priority list
            RDY_OVG_Overwait.enqueue(pOvCheck, 1000 - pOvCheck->getTQ());
            totalOverwaitOVG++;
            cout << "!!! OVG Order " << pOvCheck->getID() << " became OVERWAIT (Wait in RDY > TH)" << endl;
        } else {
            tempCheck.enqueue(pOvCheck);
        }
    }
    while (tempCheck.dequeue(pOvCheck)) {
        RDY_OV_List.enqueue(pOvCheck);
    }

    for (int i = 0; i < 20; i++) 
    {
        order* pOrd = nullptr;
        int pri = 0;

        // Priority 1: RDY_COMBO
        if (RDY_COMBO.peek(pOrd)) {
            int needed = pOrd->getNumScootersRequired();
            int available = Free_Scooters.getCount();

            if (available >= needed) {
                RDY_COMBO.dequeue(pOrd);
                pOrd->setTS(currentTime);
                
               
                int maxServiceTime = 0;

                for (int j = 0; j < needed; j++) {
                    scooter* pScoot = nullptr;
                    Free_Scooters.dequeue(pScoot, pri);
                    pOrd->addScooter(pScoot);
                    int sTime = CalculateDeliveryServiceTime(pOrd, pScoot);
                    if (sTime > maxServiceTime) maxServiceTime = sTime;
                }

                pOrd->setTF(currentTime + maxServiceTime);
                InServ_Orders.enqueue(pOrd, 1000 - pOrd->getTF());

                cout << ">>> COMBO Order " << pOrd->getID() << " assigned to " << needed << " scooters: ";
                for (int j = 0; j < needed; j++) cout << pOrd->getScooter(j)->getID() << (j < needed - 1 ? ", " : "");
                cout << endl;

                // Scooters go to back list
                for (int j = 0; j < needed; j++) {
                    Back_Scooters.enqueue(pOrd->getScooter(j), 1000 - pOrd->getTF());
                }
                continue;
            }
        }

        // Priority 2: RDY_OVG_Overwait
        if (RDY_OVG_Overwait.peek(pOrd, pri)) {
            scooter* pScoot = nullptr;
            if (Free_Scooters.dequeue(pScoot, pri)) {
                RDY_OVG_Overwait.dequeue(pOrd, pri);
                int sTime = CalculateDeliveryServiceTime(pOrd, pScoot);
                pOrd->setTS(currentTime);
                pOrd->setTF(currentTime + sTime);
                pOrd->addScooter(pScoot);
                InServ_Orders.enqueue(pOrd, 1000 - pOrd->getTF());
                Back_Scooters.enqueue(pScoot, 1000 - pOrd->getTF());
                continue;
            }
        }

        // Priority 3: OT, OD, then regular OV
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

        if (RDY_OD.dequeue(pOrd))
        {
            if (pOrd != nullptr)
            {
                table* pTable = nullptr;
                int neededSeats = pOrd->getSeats();
                if (neededSeats <= 0) neededSeats = 1;

                if (Free_Tables.getBest(neededSeats, pTable))
                {
                    pOrd->setTable(pTable);
                    pOrd->setTS(currentTime);
                    int duration = pOrd->getDuration();
                    if (duration <= 0) duration = RandomInt(2, 5);
                    pOrd->setTF(currentTime + duration);
                    Busy_No_Share.enqueue(pTable, 100 - pTable->getCapacity());
                    InServ_Orders.enqueue(pOrd, 1000 - pOrd->getTF());
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
                pri = 0;

                if (Free_Scooters.dequeue(pScooter, pri))
                {
                    int serviceTime = CalculateDeliveryServiceTime(pOrd, pScooter);
                    pOrd->setTS(currentTime);
                    pOrd->setTF(currentTime + serviceTime);
                    pOrd->addScooter(pScooter);
                    InServ_Orders.enqueue(pOrd, 1000 - pOrd->getTF());
                    Back_Scooters.enqueue(pScooter, 1000 - pOrd->getTF());
                }
                else
                {
                    RDY_OV_List.enqueue(pOrd);
                }
            }
            continue;
        }

        break; // No more orders can be assigned this step
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
        if (pOrd == nullptr) continue;

        // Scooter failure logic
        if (pOrd->isDelivery() && !pOrd->isFailed() && !pOrd->isRescueMission()) {
            if (RandomInt(1, 100) <= 5) { // 5% probability
                pOrd->setFailed(true);
                rescueCount++;
                cout << "!!! Scooter FAIL for Order " << pOrd->getID() << " at timestep " << currentTime << endl;
                
                // Add all currently assigned scooters to Failed_Scooters list
                for (int j = 0; j < pOrd->getScootersCount(); j++) {
                    Failed_Scooters.enqueue(pOrd->getScooter(j));
                }

                // Try to get a rescue scooter
                scooter* pRescue = nullptr;
                int rPri = 0;
                if (Free_Scooters.dequeue(pRescue, rPri)) {
                    pOrd->setRescue(true);
                    pOrd->setRescueScooter(pRescue);
                    cout << ">>> Rescue Scooter " << pRescue->getID() << " assigned to Order " << pOrd->getID() << endl;
                    
                    int reachTime = pOrd->getDistance() / pRescue->getSpeed();
                    if (reachTime < 1) reachTime = 1;
                    
                    pOrd->setTF(currentTime + reachTime * 2);
                    tempQueue.enqueue(pOrd, 1000 - pOrd->getTF());
                    continue;
                } else {
                    tempQueue.enqueue(pOrd, pri);
                    continue;
                }
            }
        }

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
            else if (pOrd->isRescueMission()) {
                scooter* pRescue = pOrd->getRescueScooter();
                Back_Scooters.enqueue(pRescue, 1000 - currentTime);
                
                // Move failed scooters from Failed_Scooters to Maint_Scooters
                for (int i = 0; i < pOrd->getScootersCount(); i++) {
                    scooter* s = nullptr;
               
                    s = pOrd->getScooter(i);
                    s->resetTrips();
                    Maint_Scooters.enqueue(s);
                    
                    
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
    scooter* pScooter = nullptr;
    int pri = 0;

    // Peek the top scooter to see if it has arrived yet
    while (Back_Scooters.peek(pScooter, pri))
    {
        int tf = 1000 - pri; // Priority was 1000 - TF
        if (tf <= currentTime)
        {
            Back_Scooters.dequeue(pScooter, pri);
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
        else
        {
            break; 
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
    pOrd->addChef(pChef);

    // If it's a COMBO order, we need more chefs
    if (pOrd->isCombo() && pOrd->getChefsCount() < pOrd->getNumChefsRequired()) {
        return; // Wait for more chefs
    }

    // Set Assigned Time (TA) and calculate Wait Time (TW)
    pOrd->setTA(currentTime);
    pOrd->setTW(currentTime - pOrd->getTQ());

    // Calculate Cook Time
    // Let's use sum of speeds as it's common for multi-chef tasks.
    int totalSpeed = 0;
    for (int i = 0; i < pOrd->getChefsCount(); i++) {
        chef* c = pOrd->getChef(i);
        int s = c->getSpeed();
        if (c->getType() == CN && pOrd->isGrilled()) {
            s /= 2;
        }
        totalSpeed += s;
    }

    if (totalSpeed < 1) totalSpeed = 1;

    int cookTime = pOrd->getDuration() / totalSpeed;
    if (pOrd->getDuration() % totalSpeed != 0) cookTime++;

    if (cookTime < 1) cookTime = 1;

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
        cout << "The list is empty." << endl;
    else
        PEND_OVG.print();
    cout << endl;

    cout << PEND_COMBO.getCount() << " COMBO (Pending Multi-Chef): ";
    if (PEND_COMBO.isEmpty())
        cout << "The list is empty.";
    else
        PEND_COMBO.print();
    cout << endl << endl;

    cout << "------------- Available chefs IDs -----------------\n";

    cout << Free_CS.getCount() << " CS (Special): ";
    Free_CS.print();
    cout << endl;

    cout << Free_CN.getCount() << " CN (Normal): ";
    Free_CN.print();
    cout << endl << endl;

    cout << "------------- Cooking orders [Orders ID, chef IDs] -----------------\n";
    cout << Cooking_Orders.getCount() << " cooking orders: ";

    if (Cooking_Orders.isEmpty())
    {
        cout << "Empty";
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

                cout << "[" << pOrd->getID() << ", Chefs: ";
                for (int j = 0; j < pOrd->getChefsCount(); j++) {
                    cout << pOrd->getChef(j)->getID() << (j < pOrd->getChefsCount() - 1 ? "+" : "");
                }
                cout << "]";

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
    cout << "List count, order type, IDs of all orders in the list\n";

    cout << RDY_OD.getCount() << " RDY_OD: ";
    RDY_OD.print();
    cout << endl;

    cout << RDY_OT.getCount() << " RDY_OT: ";
    RDY_OT.print();
    cout << endl;

    cout << RDY_OV_List.getCount() << " RDY_OV: ";
    RDY_OV_List.print();
    cout << endl;

    cout << RDY_COMBO.getCount() << " RDY_COMBO (Ready Multi-Scooter): ";
    RDY_COMBO.print();
    cout << endl;

    cout << RDY_OVG_Overwait.getCount() << " RDY_OVG_Overwait (High Priority): ";
    if (RDY_OVG_Overwait.isEmpty()) cout << "Empty";
    else RDY_OVG_Overwait.print();
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
        cout << "Empty";
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
                {
                    if (pServOrd->isFailed() && !pServOrd->isRescueMission())
                        cout << ", FAILED-Waiting Rescue";
                    else if (pServOrd->isRescueMission())
                        cout << ", RESCUE-S" << pServOrd->getRescueScooter()->getID();
                    else
                        cout << ", S" << pServOrd->getScooter()->getID();
                }
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

    // Add a dedicated line for failed orders if any exist
    {
        priQueue<order*> tempInServ;
        order* pServOrd = nullptr;
        int servPri = 0;
        bool firstFailed = true;
        int failedCount = 0;

        while (InServ_Orders.dequeue(pServOrd, servPri)) {
            if (pServOrd && pServOrd->isFailed() && !pServOrd->isRescueMission()) {
                if (firstFailed) cout << ">>> ORDERS WAITING FOR RESCUE: ";
                else cout << ", ";
                cout << pServOrd->getID();
                firstFailed = false;
                failedCount++;
            }
            tempInServ.enqueue(pServOrd, servPri);
        }
        while (tempInServ.dequeue(pServOrd, servPri)) InServ_Orders.enqueue(pServOrd, servPri);
        if (failedCount > 0) cout << endl << endl;
    }

    cout << "------------- Failed scooters IDs -----------------\n";
    cout << Failed_Scooters.getCount() << " scooters: ";
    if (Failed_Scooters.isEmpty())
        cout << "The list is empty.";
    else
        Failed_Scooters.print();
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


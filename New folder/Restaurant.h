#pragma once
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#include "order.h"
#include "table.h"
#include "ArrayStack.h"
#include "TableFitPriQueue.h"
#include "PriQueueWithCancel.h"
#include "QueueWithCancel.h"
#include "chef.h"
#include "Action.h"
#include "scooter.h"

class Restaurant
{
private:
    int currentTime;
    int totalGeneratedOrders;
    
    int totalCN;
    int totalCS;
    int totalScooters;
    int overwaitThreshold;

    LinkedQueue<Action*>    ACTIONS_LIST;
    LinkedQueue<order*>     PEND_ODG;
    LinkedQueue<order*>     PEND_ODN;
    LinkedQueue<order*>     PEND_OT;
    LinkedQueue<order*>     PEND_OVN;
    QueueWithCancel         PEND_OVC;
    priQueue<order*>        PEND_OVG;
    priQueue<order*>        PEND_COMBO;

    LinkedQueue<chef*>      Free_CS;
    LinkedQueue<chef*>      Free_CN;

    LinkedQueue<order*>     Cancelled_orders;
    ArrayStack<order*>      Finished_orders;
    PriQueueWithCancel      Cooking_Orders;

    LinkedQueue<order*>     RDY_OT;
    QueueWithCancel         RDY_OV_List;
    LinkedQueue<order*>     RDY_OD;
    LinkedQueue<order*>     RDY_COMBO;
    priQueue<order*>        RDY_OVG_Overwait;

    priQueue<order*>        InServ_Orders;

    priQueue<scooter*>      Free_Scooters;
    priQueue<scooter*>      Back_Scooters;
    LinkedQueue<scooter*>   Maint_Scooters;
    LinkedQueue<scooter*>   Failed_Scooters;

    TableFitPriQueue        Free_Tables;
    TableFitPriQueue        Busy_Sharable;
    TableFitPriQueue        Busy_No_Share;

    // Bonus Stats
    int comboCount;
    int rescueCount;
    int totalOverwaitOVG;

    int RandomInt(int minVal, int maxVal) const;

public:
    Restaurant();
    ~Restaurant();

    void AddOrderToPending(order* pOrd);
    void HandleCancelOrder(int orderID);

    void ReadInputFile(string fileName);
    void GenerateOutputFile(string fileName);
    void ExecuteCurrentActions();

    void Simulate(int mode);

    void SimulateOneTimeStep();

    void MovePendingToCooking();
    void MoveCookingToReady();
    void MoveReadyToService();
    void TryCancelPendingOVC();
    void TryCancelReadyOVC();
    void TryCancelCookingOV();
    void MoveInServiceToFinish();
    void HandleBackScooters();
    void HandleMaintenanceScooters();
    bool AllOrdersDone() const;
    int CalculateDeliveryServiceTime(order* pOrd, scooter* pScooter);
    bool SimulationFinished() const;


    void BindOrderToChef(order* pOrd, chef* pChef); // new 

    void OutputStatusBar();
    
    
}; 
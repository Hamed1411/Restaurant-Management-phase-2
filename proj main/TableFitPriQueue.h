#pragma once
#include "priQueue.h"
#include "table.h"

class TableFitPriQueue :
    public priQueue <table*>
{
    public:
        bool getBest(int groupSize, table*& foundTable);
   

};


#pragma once
#include <iostream>
#include <string>
using namespace std;

class UI
{
public:
    void PrintMessage(const string& msg) const;
    void PrintCurrentTimestep(int timestep) const;

    char ReadMode() const;
    string ReadInputFileName() const;
    string ReadOutputFileName() const;

    template <typename T>
    void PrintList(const string& title, T& ds) const
    {
        cout << "------------- " << title << " ----------------" << endl;
        ds.print();
        cout << endl;
    }
};


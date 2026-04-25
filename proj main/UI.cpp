#include "UI.h"
#include <iostream>
#include <string>
using namespace std;

void UI::PrintMessage(const string& msg) const
{
    cout << msg << endl;
}

void UI::PrintCurrentTimestep(int timestep) const
{
    cout << "Current Timestep: " << timestep << endl;
}

char UI::ReadMode() const
{
    char mode;
    cout << "Enter mode (I for Interactive, S for Silent): ";
    cin >> mode;
    return mode;
}

string UI::ReadInputFileName() const
{
    string fileName;
    cout << "Enter input file name: ";
    cin >> fileName;
    return fileName;
}

string UI::ReadOutputFileName() const
{
    string fileName;
    cout << "Enter output file name: ";
    cin >> fileName;
    return fileName;
}
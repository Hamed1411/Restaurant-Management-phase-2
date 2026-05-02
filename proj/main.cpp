#include "Restaurant.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main()
{
    Restaurant r;

    string fileName;
    int mode;

    cout << "Enter input file name: ";
    cin >> fileName;

    if (fileName.find(".txt") == string::npos)
        fileName += ".txt";

    cout << "Choose mode:\n";
    cout << "1 - Interactive mode\n";
    cout << "2 - Silent mode\n";
    cin >> mode;

    string outputFileName = fileName;
    int dotPos = outputFileName.find(".txt");
    outputFileName.erase(dotPos, 4);
    outputFileName += "output.txt";

    r.ReadInputFile(fileName);

    if (mode == 1)
    {
        ofstream outFile(outputFileName);

        streambuf* oldCout = cout.rdbuf();

        class TeeBuf : public streambuf
        {
        public:
            streambuf* s1;
            streambuf* s2;

            TeeBuf(streambuf* buf1, streambuf* buf2)
            {
                s1 = buf1;
                s2 = buf2;
            }

            int overflow(int c)
            {
                if (c == EOF) return !EOF;
                s1->sputc(c);
                s2->sputc(c);
                return c;
            }
        };

        TeeBuf tee(oldCout, outFile.rdbuf());
        cout.rdbuf(&tee);

        r.Simulate();

        cout.rdbuf(oldCout);
        outFile.close();
    }
    else
    {
        cout << "Simulation Starts in Silent mode ...\n";

        ofstream outFile(outputFileName);

        streambuf* oldCout = cout.rdbuf();
        cout.rdbuf(outFile.rdbuf());

        r.Simulate();

        cout.rdbuf(oldCout);
        outFile.close();

        cout << "Simulation ends, Output file created\n";
    }

    return 0;
}
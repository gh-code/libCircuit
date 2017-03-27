#include "circuit.h"
#include "celllibrary.h"
#include "EDAUtils.h"
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    CellLibrary library("NangateOpenCellLibrary_typical_conditional_nldm.lib");
    Circuit circuit("c17.v", library);

    if (circuit.isNull())
    {
        cout << "Circuit is empty\n";
        return 1;
    }

    vector<Cell> levelCells;
    EDAUtils::orderByLevel(circuit, levelCells);

    if (circuit.name() == "c17")
    {
        vector<std::string> patterns{"00000", "10100"};

        for (int i = 0; i < patterns.size(); i++)
        {
            if (!circuit.input(patterns[i]))
            {
                cerr << "fail with input size" << endl;
                return 1;
            }
            cout << circuit.input() << "|";
            for (size_t i = 0; i < levelCells.size(); i++)
            {
                levelCells[i].eval();
                levelCells[i].output(0).eval();
            }
            cout << circuit.output() << endl;
        }
    }

    return 0;
}
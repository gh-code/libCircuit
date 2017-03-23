#include "circuit.h"
#include <iostream>

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

    const Module &module = circuit.topModule();
    for (size_t i = 0; i < module.cellSize(); i++)
    {
        Cell cell = module.cell(i);
        cout << cell.type() << endl;
        for (size_t j = 0; j < cell.inputSize(); j++)
        {
            cout << cell.pinName(j) << ": ";
            cout << cell.inputCapacitance(j) << endl;
        }
    }

    return 0;
}
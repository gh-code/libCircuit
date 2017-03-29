#include "circuit.h"
#include "celllibrary.h"
#include <iostream>

using namespace std;

int main()
{
    CellLibrary library("NangateOpenCellLibrary_typical_conditional_nldm.lib");
    if (!library.isNull())
        std::cout << library.cell("NAND2_X1").delay("A1", "ZN", Signal::Fall, 0.0075, 0.0004) << endl;
    return 0;
}
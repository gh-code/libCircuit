#include "circuit.h"
#include "celllibrary.h"
#include <iostream>
#include <string>

using namespace std;

void usage()
{
    cout << "./circuit <verilog>" << endl;
}

void forward(const Node &node)
{
    static int level = 0;
    string spaces(2 * level, ' ');
    cout << spaces << node.nodeName() << endl;
    for (size_t i = 0; i < node.outputSize(); i++)
    {
        level++;
        forward(node.output(i));
        level--;
    }
}

void backward(const Node &node)
{
    static int level = 0;
    string spaces(2 * level, ' ');
    cout << spaces << node.nodeName() << endl;
    for (size_t i = 0; i < node.inputSize(); i++)
    {
        level++;
        backward(node.input(i));
        level--;
    }
}

void printCircuitInfo(const Circuit &circuit)
{
    cout << "Circuit: " << circuit.name() << endl;
    cout << "#input: " << circuit.inputSize() << endl;
    cout << "#output: " << circuit.outputSize() << endl;
    cout << "Modules: ";
    cout << circuit.moduleSize() << endl;
}

void printCellLibraryInfo(const CellLibrary &library)
{
    cout << "library: " << library.name() << endl;
    cout << "cell_count: " << library.cellCount() << endl;
    cout << "time_unit: " << library.time_unit() << endl;
    cout << "leakage_power_unit: " << library.leakage_power_unit() << endl;
    cout << "voltage_unit: " << library.voltage_unit() << endl;
    cout << "current_unit: " << library.current_unit() << endl;
    cout << "pulling_resistance_unit: " << library.pulling_resistance_unit() << endl;
    cout << "capacitive_load_unit: " << library.capacitive_load_unit() << endl;
    cout << "nom_process: " << library.nom_process() << endl;
    cout << "nom_temperature: " << library.nom_temperature() << endl;
    cout << "nom_voltage: " << library.nom_voltage() << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage();
        return 1;
    }

    CellLibrary library("NangateOpenCellLibrary_typical_conditional_nldm.lib");
    Circuit circuit(argv[1], library);

    cout << std::string(79, '-') << endl;
    printCellLibraryInfo(library);
    cout << std::string(79, '-') << endl;
    printCircuitInfo(circuit);
    cout << std::string(79, '-') << endl;

    if (circuit.isNull())
    {
        cout << "Circuit is empty\n";
        return 1;
    }

    for (size_t i = 0; i < circuit.outputSize(); i++)
        backward(circuit.outputPort(i));
    for (size_t i = 0; i < circuit.inputSize(); i++)
        forward(circuit.inputPort(i));
        
    // for (size_t i = 0; i < circuit.moduleSize(); i++)
    //     cout << circuit.module(i).name() << " ";
    // cout << endl;
    //
    // cout << "Gate count: ";
    // cout << circuit.gateCount() << endl;
    //
    // cout << "Inputs: ";
    // for (size_t i = 0; i < circuit.inputSize(); i++)
    //     cout << circuit.inputPort(i).name() <<  " ";
    // cout << endl;
    //
    // cout << "Outputs: ";
    // for (size_t i = 0; i < circuit.outputSize(); i++)
    //     cout << circuit.outputPort(i).name() <<  " ";
    // cout << endl;

    return 0;
}

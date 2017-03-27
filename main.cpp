#include "circuit.h"
#include "celllibrary.h"
#include "EDAUtils.h"
#include <iostream>
#include <string>

using namespace std;

void usage()
{
    cout << "./circuit <verilog>" << endl;
}

void forward(const Node &node)
{
    if(node.isNull())
        return;

    static int level = 0;
    string spaces(2 * level, ' ');
    cout << spaces << node.nodeName();
    if (node.isGate())
        cout << " (" << type_str(node.toGate().gateType()) << ") {" << node.toGate().level() << ")";
    if (node.isCell())
        cout << " (" << type_str(node.toCell().gateType()) << ") (" << node.toCell().level() << ")";
    cout << endl;
    for (size_t i = 0; i < node.outputSize(); i++)
    {
        level++;
        forward(node.output(i));
        level--;
    }
}

void backward(const Node &node)
{
    if(node.isNull())
        return;

    static int level = 0;
    string spaces(2 * level, ' ');
    cout << spaces << node.nodeName();
    if (node.isGate())
        cout << " (" << type_str(node.toGate().gateType()) << ")";
    if (node.isCell())
        cout << " (" << type_str(node.toCell().gateType()) << ")";
    cout << endl;

    for (size_t i = 0; i < node.inputSize(); i++)
    {
        level++;
        backward(node.input(i));
        level--;
    }
}

void printCircuitInfo(const Circuit &circuit)
{
    cout << " circuit: " << circuit.name() << endl;
    cout << " #inputs: " << circuit.inputSize() << endl;
    cout << "#outputs: " << circuit.outputSize() << endl;
    cout << "  #gates: " << circuit.gateCount() << endl;
    cout << "#modules: " << circuit.moduleSize() << endl;
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

    if (circuit.isNull())
    {
        cout << "Circuit is empty\n";
        return 1;
    }

    cout << std::string(79, '-') << endl;
    printCellLibraryInfo(library);
    cout << std::string(79, '-') << endl;
    printCircuitInfo(circuit);
    cout << std::string(79, '-') << endl;

    vector<Cell> temp;
    EDAUtils::orderByLevel(circuit, temp);
    for(size_t i = 0; i < circuit.topModule().gateSize(); i++)
    {
        Gate c = circuit.topModule().gate(i);
        cout << "Cell: " << c.name() << "  ";
        cout << "inputSize: " << c.inputSize() << endl;
        cout << "outputSize: " << c.outputSize() << endl;
        cout << "(" << c.level() << ")" << endl;
    }
    for(size_t i = 0; i < circuit.topModule().cellSize(); i++)
    {
        Cell c = circuit.topModule().cell(i);
        cout << "Cell: " << c.name() << "  ";
        cout << "inputSize: " << c.inputSize() << endl;
        cout << "outputSize: " << c.outputSize() << endl;
        cout << "(" << c.level() << ")" << endl;
    }
    
    cout << "===============================" << endl;
    cout << "========  Remove All DFF  =====" << endl;
    cout << "===============================" << endl;
    
    EDAUtils::removeAllDFF(circuit);

    printCircuitInfo(circuit);
    for(size_t i = 0; i < circuit.topModule().gateSize(); i++)
    {
        Gate c = circuit.topModule().gate(i);
        cout << "Cell: " << c.name() << "  ";
        cout << "inputSize: " << c.inputSize() << endl;
        cout << "outputSize: " << c.outputSize() << endl;
        cout << "(" << c.level() << ")" << endl;
    }
    for(size_t i = 0; i < circuit.topModule().cellSize(); i++)
    {
        Cell c = circuit.topModule().cell(i);
        cout << "Cell: " << c.name() << "  ";
        cout << "inputSize: " << c.inputSize() << endl;
        cout << "outputSize: " << c.outputSize() << endl;
        cout << "(" << c.level() << ")" << endl;
    }
    cout << "======== backward =========" << endl;
    for (size_t i = 0; i < circuit.outputSize(); i++)
        backward(circuit.outputPort(i));
    cout << "======== forward ==========" << endl;
    for (size_t i = 0; i < circuit.inputSize(); i++)
        forward(circuit.inputPort(i));

    cout << "===============================" << endl;
    cout << "===== Insert MUX to Cell  =====" << endl;
    cout << "===============================" << endl;
    EDAUtils::insertCell2AllCellOutputs(circuit, library, EDAUtils::mux_connect_interal);

    printCircuitInfo(circuit);
    for(size_t i = 0; i < circuit.topModule().gateSize(); i++)
    {
        Gate c = circuit.topModule().gate(i);
        cout << "Cell: " << c.name() << "  ";
        cout << "inputSize: " << c.inputSize() << endl;
        cout << "outputSize: " << c.outputSize() << endl;
        cout << "(" << c.level() << ")" << endl;
    }
    for(size_t i = 0; i < circuit.topModule().cellSize(); i++)
    {
        Cell c = circuit.topModule().cell(i);
        cout << "Cell: " << c.name() << "  ";
        cout << "inputSize: " << c.inputSize() << endl;
        cout << "outputSize: " << c.outputSize() << endl;
        cout << "(" << c.level() << ")" << endl;
    }
    cout << "======== backward =========" << endl;
    for (size_t i = 0; i < circuit.outputSize(); i++)
        backward(circuit.outputPort(i));
    cout << "======== forward ==========" << endl;
    for (size_t i = 0; i < circuit.inputSize(); i++)
        forward(circuit.inputPort(i));
    return 0;
}

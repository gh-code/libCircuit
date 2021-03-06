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
    if (node.isWire())
        cout << " (Wire)" ;
    if (node.isPort())
        cout << " (Port)" ;
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
    if (node.isWire())
        cout << " (Wire)" ;
    if (node.isPort())
        cout << " (Port)" ;
    if (node.isGate())
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
    cout << " #outputs: " << circuit.outputSize() << endl;
    cout << " #PI: " << circuit.PISize() << endl;
    cout << " #PO: " << circuit.POSize() << endl;
    cout << " #PPI: " << circuit.PPISize() << endl;
    cout << " #PPO: " << circuit.PPOSize() << endl;
    cout << "  #gates: " << circuit.gateCount() << endl;
    cout << "#modules: " << circuit.moduleSize() << endl;
    for(size_t i = 0; i < circuit.topModule().PISize(); i++)
    {
        Port w = circuit.topModule().PI(i);
        cout << "Port(PI): " << w.name() << " ";
        cout << "inputSize: " << w.inputSize() << endl;
        cout << "outputSize: " << w.outputSize() << endl;
    }
    for(size_t i = 0; i < circuit.topModule().POSize(); i++)
    {
        Port w = circuit.topModule().PO(i);
        cout << "Port(PO): " << w.name() << " ";
        cout << "inputSize: " << w.inputSize() << endl;
        cout << "outputSize: " << w.outputSize() << endl;
    }
    for(size_t i = 0; i < circuit.topModule().PPISize(); i++)
    {
        Port w = circuit.topModule().PPI(i);
        cout << "Port(PPI): " << w.name() << " ";
        cout << "inputSize: " << w.inputSize() << endl;
        cout << "outputSize: " << w.outputSize() << endl;
    }
    for(size_t i = 0; i < circuit.topModule().PPOSize(); i++)
    {
        Port w = circuit.topModule().PPO(i);
        cout << "Port(PPO): " << w.name() << " ";
        cout << "inputSize: " << w.inputSize() << endl;
        cout << "outputSize: " << w.outputSize() << endl;
    }
    for(size_t i = 0; i < circuit.topModule().wireSize(); i++)
    {
        Wire w = circuit.topModule().wire(i);
        cout << "Wire: " << w.name() << " ";
        cout << "inputSize: " << w.inputSize() << endl;
        cout << "outputSize: " << w.outputSize() << endl;
    }
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
    /* EDAUtils::orderByLevel(circuit, temp); */
    
    /* cout << "===============================" << endl; */
    /* cout << "========  Remove All DFF  =====" << endl; */
    /* cout << "===============================" << endl; */
    
    /* EDAUtils::removeAllDFF(circuit, library); */

    /* printCircuitInfo(circuit); */
    /* cout << "======== backward =========" << endl; */
    /* for (size_t i = 0; i < circuit.outputSize(); i++) */
    /*     backward(circuit.outputPort(i)); */
    /* cout << "======== forward ==========" << endl; */
    /* for (size_t i = 0; i < circuit.inputSize(); i++) */
    /*     forward(circuit.inputPort(i)); */


    /* cout << "===============================" << endl; */
    /* cout << "===== Insert MUX to Cell  =====" << endl; */
    /* cout << "===============================" << endl; */
    /* EDAUtils::insertCell2AllCellOutputs(circuit, library, EDAUtils::mux_connect_interal); */

    /* printCircuitInfo(circuit); */
    /* cout << "======== backward =========" << endl; */
    /* for (size_t i = 0; i < circuit.outputSize(); i++) */
    /*     backward(circuit.outputPort(i)); */
    /* cout << "======== forward ==========" << endl; */
    /* for (size_t i = 0; i < circuit.inputSize(); i++) */
    /*     forward(circuit.inputPort(i)); */

/*     temp.clear(); */
/*     EDAUtils::orderByLevel(circuit, temp); */

/*     printCircuitInfo(circuit); */

/*     cout << "===============================" << endl; */
/*     cout << "========= Simulation ==========" << endl; */
/*     cout << "===============================" << endl; */
/*     circuit.input("00000"); */
/*     for (size_t i = 0; i < circuit.inputSize(); i++) */
/*     { */
/*         Port p = circuit.inputPort(i); */
/*         for(size_t j = 0; j < p.outputSize(); j++) */
/*         { */
/*             Node node = p.output(j); */
/*             if(node.isWire()) */
/*                 node.setValue(p.value()); */
/*         } */
/*     } */
/*     for (size_t i = 0; i < temp.size(); i++) */
/*     { */
/*         Cell cell = temp[i]; */
/*         cell.eval(); */
/*         cell.output(0).eval(); */
/*         cout << "Cell: " << cell.name() << "  Value = " << cell.value() << endl; */
/*         cout << "("; */
/*         for(size_t j = 0; j < cell.inputSize(); j++) */
/*             cout << cell.inputPinName(j) << ":" << cell.input(j).value() << " $ "; */
/*         cout << ")" << endl; */
/*     } */
/*     for (size_t i = 0; i < circuit.outputSize(); i++) */
/*     { */
/*         Port p = circuit.outputPort(i); */
/*         for(size_t j = 0; j < p.inputSize(); j++) */
/*         { */
/*             Node node = p.input(j); */
/*             if(node.isWire()) */
/*                 p.setValue(node.value()); */
/*         } */
/*     } */
/*     cout << "output" << endl; */
/*     cout << circuit.output() << endl; */
/*     cout << endl; */

    cout << "======================================" << endl;
    cout << "======== Timeframe Expansion =========" << endl;
    cout << "======================================" << endl;
    vector<Circuit> maintains;
    EDAUtils::timeFrameExpansion(circuit, library, 2, maintains);

    temp.clear();
    EDAUtils::orderByLevel(circuit, temp);

    printCircuitInfo(circuit);
    cout << "======== backward =========" << endl;
    for (size_t i = 0; i < circuit.outputSize(); i++)
        backward(circuit.outputPort(i));
    cout << "======== forward ==========" << endl;
    for (size_t i = 0; i < circuit.inputSize(); i++)
        forward(circuit.inputPort(i));

    /* cout << "===============================" << endl; */
    /* cout << "===== Insert MUX to Cell  =====" << endl; */
    /* cout << "===============================" << endl; */
    /* EDAUtils::insertCell2AllCellOutputs(circuit, library, EDAUtils::mux_connect_internal); */

    /* temp.clear(); */
    /* EDAUtils::orderByLevel(circuit, temp); */

    /* printCircuitInfo(circuit); */
    /* cout << "======== backward =========" << endl; */
    /* for (size_t i = 0; i < circuit.outputSize(); i++) */
    /*     backward(circuit.outputPort(i)); */
    /* cout << "======== forward ==========" << endl; */
    /* for (size_t i = 0; i < circuit.inputSize(); i++) */
    /*     forward(circuit.inputPort(i)); */
    return 0;
}

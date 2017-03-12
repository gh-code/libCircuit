#include "circuit.h"
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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage();
        return 1;
    }

    Circuit circuit(argv[1]);

    if (circuit.isNull())
    {
        cout << "Circuit is empty\n";
        return 1;
    }

    cout << "Circuit: " << circuit.name() << endl;
    cout << "#input: " << circuit.inputSize() << endl;
    cout << "#output: " << circuit.outputSize() << endl;
    cout << "Modules: ";
    cout << circuit.moduleSize() << endl;

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

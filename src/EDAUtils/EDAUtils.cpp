#include "EDAUtils.h"
#include <algorithm>
#include <list>
#include <map>

using namespace std;

unsigned EDAUtils::levelize(const Circuit &circuit, std::vector<Node*> &nodes)
{
    cout << "Call EDAUtils::levelize()" << endl;

    list<Node*> Queue;
    map<string, unsigned> count_map;
    for(size_t in_idx = 0; in_idx < circuit.inputSize(); in_idx++)
    {  
        Port p = circuit.inputPort(in_idx);
        for(size_t pin_idx = 0; pin_idx < p.outputSize(); pin_idx++)
        {
            Node node = p.output(pin_idx);
            if(node.isCell())
            {
                Cell cell = node.toCell();
                if(count_map.count(cell.name()) == 0)
                    count_map[cell.name()] = 1;
                else
                    count_map[cell.name()] = count_map[cell.name()] + 1;

                if(count_map[cell.name()] == cell.inputSize())
                {
                    cell.setLevel(1);
                    Queue.push_back(new Cell(cell));
                }
            }
            else if(node.isGate())
            {
                Gate gate = node.toGate();
                if(count_map.count(gate.name()) == 0)
                    count_map[gate.name()] = 1;
                else
                    count_map[gate.name()] = count_map[gate.name()] + 1;
                if(count_map[gate.name()] == gate.inputSize())
                {
                    gate.setLevel(1);
                    Queue.push_back(new Gate(gate));
                }
            }
            else if(node.isPort())
                cout << "Input port directly connect to output port" << endl;
            else
                cout << "Illegal levelize Type" << endl;
        }
    }

    int l1, l2;
    while(!Queue.empty())
    {
        Node* node = Queue.front();
        Queue.pop_front();
        if(node->isCell())
            l2 = node->toCell().level();
        else if(node->isGate())
            l2 = node->toGate().level();

        for(size_t j = 0; j < node->outputSize(); j++)
        {
            Node out = node->output(j);
            if(out.isWire())
            {
                Wire w = out.toWire();
                for(size_t k = 0; k < w.outputSize(); k++)
                {
                    Node outw = w.output(k);
                    if(outw.isCell())
                    {
                        Cell outc = outw.toCell();
                        l1 = outc.level();
                        if(l1 <= l2)
                            outc.setLevel(l2+1);
                        count_map[outc.name()] = count_map[outc.name()] + 1;
                        if(count_map[outc.name()] == outc.inputSize())
                            Queue.push_back(new Cell(outc));
                    }
                    else if(outw.isGate())
                    {
                        Gate outg = outw.toGate();
                        l1 = outg.level();
                        if(l1 <= l2)
                            outg.setLevel(l2+1);
                        count_map[outg.name()] = count_map[outg.name()] + 1;
                        if(count_map[outg.name()] == outg.inputSize())
                            Queue.push_back(new Gate(outg));
                    }
                }
            }
            else if(out.isCell())
            {
                Cell outc = out.toCell();
                l1 = outc.level();
                if(l1 <= l2)
                    outc.setLevel(l2+1);
                count_map[outc.name()] = count_map[outc.name()] + 1;
                if(count_map[outc.name()] == outc.inputSize())
                    Queue.push_back(new Cell(outc));
            }
            else if(out.isGate())
            {
                Gate outg = out.toGate();
                l1 = outg.level();
                if(l1 <= l2)
                    outg.setLevel(l2+1);
                count_map[outg.name()] = count_map[outg.name()] + 1;
                if(count_map[outg.name()] == outg.inputSize())
                    Queue.push_back(new Gate(outg));
            }
        }

    }

    
     
    return 0;
}


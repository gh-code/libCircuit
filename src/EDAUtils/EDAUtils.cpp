#include "EDAUtils.h"
#include <queue>
#include <map>

using namespace std;

unsigned EDAUtils::levelizeCell(const Circuit &circuit, std::vector<Cell*> &cells)
{
    cout << "Call EDAUtils::levelizeCell()" << endl;

    queue<Cell*> Queue;
    map<Cell*, unsigned> count_map;
    for(size_t in_idx = 0; in_idx < circuit.inputSize(); in_idx++)
    {  
        Port p = circuit.inputPort(in_idx);
        for(size_t pin_idx = 0; pin_idx < p.outputSize(); pin_idx++)
        {
            Node node = p.output(pin_idx);
            if(node.isCell())
            {
                Queue.push_back(node.toCell());
            }
            else
            {
                cout << "Illegal Node Type" << endl;
            }
        }
    }



    return 0;
}

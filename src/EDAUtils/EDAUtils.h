#ifndef _EDAUTILS_H_
#define _EDAUTILS_H_

#include <vector>
#include "circuit.h"

class EDAUtils
{
    public:
        static void levelize(const Circuit &);
        static void orderGateByLevel(const Circuit &, std::vector<Gate> &);
        static void orderCellByLevel(const Circuit &, std::vector<Cell> &);
};

#endif

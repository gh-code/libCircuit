#ifndef _EDAUTILS_H_
#define _EDAUTILS_H_

#include <vector>
#include "circuit.h"

class EDAUtils
{
    public:
        static unsigned levelizeCell(const Circuit &, std::vector<Cell*> &);
        static unsigned levelizeGate(const Circuit &, std::vector<Cell*> &);
};

#endif

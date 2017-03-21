#ifndef _EDAUTILS_H_
#define _EDAUTILS_H_

#include <vector>
#include "circuit.h"

class EDAUtils
{
    public:
        static unsigned levelize(const Circuit &, std::vector<Node*> &);
};

#endif

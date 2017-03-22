#ifndef _EDAUTILS_H_
#define _EDAUTILS_H_

#include <vector>
#include "circuit.h"

class EDAUtils
{
    public:
        /**
         * Levelize the all gate/cell in Circuit(sequential/combinational),
         * consider using orderCellByLevel() or orderGateByLevel()
         * @param Circuit & : circuit object
         *
         * Example:
         *  Circuit c17("c17.v");
         *  EDAUtils::levelize(c17);
         */
        static void levelize(const Circuit &);

        /**
         * Levelize Circuit(sequential/combinational) and feedback the levelized Gates
         * @param Circuit & : circuit object
         * @param std::vector<Gate> & : an Gate container
         *
         * Example:
         *  Circuit c17("c17.v");
         *  std::vector<Gate> gateList;
         *  EDAUtils::orderGateByLevel(c17, gateList);
         */
        static void orderByLevel(const Circuit &, std::vector<Gate> &);

        /**
         * Levelize Circuit(sequential/combinational) and feedback the levelized Cell
         * @param Circuit & : circuit object
         * @param std::vector<Cell> & : an Cell container
         *
         * Example:
         *  Circuit c17("c17.v");
         *  std::vector<Cell> cellList;
         *  EDAUtils::orderCellByLevel(c17, cellList);
         */
        static void orderByLevel(const Circuit &, std::vector<Cell> &);

        static void removeDFF(Circuit &);
        static void timeFrameExpansion(Circuit &, unsigned);
};

#endif

#ifndef _EDAUTILS_H_
#define _EDAUTILS_H_

#include <vector>
#include "circuit.h"
#include "celllibrary.h"

class EDAUtils
{
    private:
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
        static void levelize(const Circuit &circuit);

        /**
         * Levelize Circuit(sequential/combinational) and feedback the levelized Gates/Cells
         * @param Circuit & : circuit object
         * @param std::vector<Gate> & : an Gate/Cell container
         *
         * Example:
         *  Circuit c17("c17.v");
         *  std::vector<Gate> gateList;
         *  EDAUtils::orderGateByLevel(c17, gateList);
         */
        static void orderByLevel(const Circuit &circuit, std::vector<Gate> &orderedList);

        static void removeAllDFF(Circuit &circuit);
       
        static bool mux_connect_interal(Circuit &circuit, Cell &target, CellLibrary &library, std::vector<Cell> &newCells);
        static bool insertCell2AllCellOutputs(Circuit &circuit, CellLibrary &library, bool (*CALLBACK)(Circuit &circuit, Cell &target, CellLibrary &library, std::vector<Cell> &newCells));

        static void timeFrameExpansion(Circuit &, unsigned);
};

#endif

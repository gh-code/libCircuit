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

        /**
         * Levelize Circuit(sequential/combinational) and feedback the levelized Cells
         * @param Circuit & : circuit object
         * @param std::vector<Cell> & : an Cell container
         *
         * Example:
         *  Circuit c17("c17.v");
         *  std::vector<Cell> cellList;
         *  EDAUtils::orderGateByLevel(c17, cellList);
         */
        static void orderByLevel(const Circuit &circuit, std::vector<Cell> &orderedList);
       
        /**
         * Remove All the cells which its type contains "FF" and has input "CK".
         * For example, DFF_X1.
         * @param Circuit & : circuit object
         *
         * Example:
         *  Circuit circuit("seq_17.v");
         *  EDAUtils::removeAllDFF(circuit);
         */
        static void removeAllDFF(Circuit &circuit, CellLibrary &library);

        /**
         * Generate a Port name with EDAUtils.
         * @param baseCell : the base cell which the port connect
         * @param pin : the pin of base cell which the port connect
         * 
         * The name will be PORT:[bascell.name()]:EDAUTILS:[pin]
         */
        static std::string genPortName(const Cell &baseCell, const std::string &pin);

        /**
         * Generate a Wire name with EDAUtils.
         * @param baseCell : the base cell which the wire connect
         * @param pin : the pin of base cell which the wire connect
         * 
         * The name will be Wire:[bascell.name()]:EDAUTILS:[pin]
         */
        static std::string genWireName(const Cell &baseCell, const std::string &pin);
        
        /**
         * Generate a Cell name with EDAUtils.
         * @param circuit : the circuit object
         *
         * The name will be [circuit.topModule().name]:EDAUTILS:CELL:<int index>
         */
        static std::string genCellName(const Circuit &circuit);

        /**
         * Default CALLBACK function for insert MUX2_X1 to all cell output
         * MUX2_X1 has 3 inputs (A, B, S) and 1 output (Z)
         * The all outputs of target will be inserted an MUX2_X1
         * The output of the target cell will connect to pin A of MUX2_X1 
         * The pin S and pin B of MUX2_X1 are Wire and internal.
         * 
         * Example:
         *  CellLibrary library("library.lib")
         *  Circuit circuit("c17.v")
         *  EDAUtils::insertCell2AllCellOutput(circuit, library, EDAUtils::mux_connect_internal);
         */
        static bool mux_connect_internal(Circuit &circuit, Cell &target, CellLibrary &library, std::vector<Cell> &newCells);

        /**
         * Insert an specific Cell to all Cell output with CALLBACK function connectivity.
         * Need write own CALLBACK function.
         * @param circuit : target circuit object
         * @param library : cell library object
         * @param (*CALLBACK)
         *    @param circuit : target circuit object
         *    @param target : the target cell preparing to be inserted.
         *    @param library : cell library object
         *    @param newCells : the vector container to put the newed cell which is inserted to circuit.
         * 
         * Example:
         *  Insert a NAND2_X1 into all Cell Output
         *  ``````````````````````````````````````````````````````````                   
         *                   =======================
         *   ----------      |        new_nand     |
         *   |        |      |  ta   ---------     |
         *   | target |----- |  -- A1|       |     |    outputNode (Wire or Port)
         *   | (cell) |      |  tb   | NAND2 |ZN --| -------------
         *   ----------      | 1-- A2|       |     |
         *                   |       ---------     |
         *                   =======================              
         *
         *  ```````````````````````````````````````````````````````````
         *
         *  bool nand_call_back(Circuit &circuit, Cell &target, CellLibrary &library, std::vector<Cell> &newCells) {
         *      if (library.hasCell("NAND2_X1")) {
         *          Node outputNode = target.output(0); // only at the first output
         *          if (outputNode.isWire() || outputNode.isPort()) { 
         *              Cell new_nand = library.cell("NAND2_X1");
         *              new_nand.setName(EDAUtils::genCellName(circuit));
         *              target.breakOutputConnection(target.outputPinName(0));
         *              Wire ta = circuit.topModule().createWire(EDAUtils::genWireName(new_nand, "A1"));
         *              Wire tb = circuit.topModule().createWire(EDAUtils::genWireName(new_nand, "A2"));
         *              tb.setInternal(true);  tb.setValue(1);
         *              new_nand.connect("A1", ta);
         *              new_nand.connect("A2", tb);
         *              new_nand.connect("ZN", outputNode);
         *              target.connect(target.outputPinName(0), ta);
         *              newCells.push_back(new_nand);
         *          }
         *          return true;
         *      } else
         *          return false;
         *  }
         *
         *  CellLibrary library("library.lib");
         *  Circuit circuit("c17.v");
         *  EDAUtils::insertCell2AllCellOutputs(circuit, library, nand_call_back);
         */
        static bool insertCell2AllCellOutputs(Circuit &circuit, CellLibrary &library, bool (*CALLBACK)(Circuit &circuit, Cell &target, CellLibrary &library, std::vector<Cell> &newCells));

        /**
         * [Alpha version]
         * Time frame expansion for a circuit with specified cycles (<100).
         * @param circuit : target circuit object
         * @param library : cell library object
         * @param cycles : time cycle specified to extend
         * @param maintains : the metadata for saving each extended circuit information
         *
         * The target circuit will contain all information, including cell, port .... Just use it to levelize..., etc.
         * !! Alpha version !! 
         * This function will removeAllDFF automatically, and concat to target circuit object.
         *
         * Example:
         *  CellLibrary library("library.lib");
         *  Circuit circuit("seq_17.v");
         *  vector<Circuit> maintains;
         *  EDAUtils::timeFrameExpansion(circuit, library, 10, maintains);
         */
        static void timeFrameExpansion(Circuit &, CellLibrary &library, const unsigned cycles, std::vector<Circuit> &maintains);
};

#endif

#include "EDAUtils.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <list>
#include <map>

using namespace std;

/* const static string EDAUTILS_PPO_PREFIX = ""; */

const std::string EDAUTILS_PPO_PREFIX = "PPO:";
const std::string EDAUTILS_PPI_PREFIX = "PPI:";
const std::string EDAUTILS_SCOPE_SIGN = ":";
const std::string EDAUTILS_SIGN = "EDAUTILS";
const std::string EDAUTILS_PORT_SIGN = "PORT";
const std::string EDAUTILS_WIRE_SIGN = "WIRE";
const std::string EDAUTILS_CELL_SIGN = "CELL";

void EDAUtils::levelize(const Circuit &circuit)
{
    cout << "Call EDAUtils::levelize()" << endl;

    list<Node*> Queue;
    map<string, unsigned> count_map;
  
    // Set Internal Wire ready
    for(size_t i = 0; i < circuit.topModule().wireSize(); i++)
    {
        Wire wire = circuit.topModule().wire(i);
        if(wire.isInternal())
        {
            for(size_t j = 0; j < wire.outputSize(); j++)
            {
                Node node = wire.output(j);
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
            }
        }
    }

    // If circuit is sequential, search FF and set the cell ready
    for(size_t c_idx = 0; c_idx < circuit.topModule().cellSize(); c_idx++)
    {
        Cell cell = circuit.topModule().cell(c_idx);
        for(size_t p_idx = 0; p_idx < cell.inputSize(); p_idx++)
        {
            Node node = cell.input(p_idx);
            if(node.isWire())
            {
                Wire w = node.toWire();
                for(size_t w_idx = 0; w_idx < w.inputSize(); w_idx++) // one input
                {
                    Node nodew = w.input(w_idx);
                    if(nodew.isWire())
                    {
                        cout << "levelize Exception: Wire to Wire" << endl;
                    }
                    else if(nodew.isCell())
                    {
                        Cell nodec = nodew.toCell();
                        if(nodec.type().find("FF") && nodec.hasInput("CK"))
                        {
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
                    }
                }
            }
            else if(node.isCell())
            {
                Cell nodec = node.toCell();
                if(nodec.type().find("FF") && nodec.hasInput("CK"))
                {
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
            }
        }
    }

    // Set all input port (PI PPI) ready
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
            else if(node.isWire())
            {
                Wire wire = node.toWire();
                for(size_t wo = 0; wo < wire.outputSize(); wo++)
                {
                    Node node = wire.output(wo);
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
                    else
                    {
                        cout << "Illegal levelize Type" << endl;
                    }
                }
            }
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
            else if(out.isPort())
            {
                Port p = out.toPort();
                for(size_t k = 0; k < p.outputSize(); k++)
                {
                    Node outp = p.output(k);
                    if(outp.isCell())
                    {
                        Cell outc = outp.toCell();
                        l1 = outc.level();
                        if(l1 <= l2)
                            outc.setLevel(l2+1);
                    }
                    else if(outp.isGate())
                    {
                        Gate outg = outp.toGate();
                        l1 = outg.level();
                        if(l1 <= l2)
                            outg.setLevel(l2+1);
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
}

static 
bool _compare_(const Gate &gate1, const Gate &gate2)
{
    return gate1.level() < gate2.level();
}
void EDAUtils::orderByLevel(const Circuit &circuit, std::vector<Gate> &gates)
{
    levelize(circuit);
    for(size_t idx = 0; idx < circuit.topModule().gateSize(); idx++)
        gates.push_back(circuit.topModule().gate(idx));
    std::sort(gates.begin(), gates.end(), _compare_);
   
    for(size_t idx = 0; idx < circuit.topModule().cellSize(); idx++)
        gates.push_back(circuit.topModule().cell(idx).toGate());
    std::sort(gates.begin(), gates.end(), _compare_);
}

void EDAUtils::removeAllDFF(Circuit &circuit)
{
    for(size_t m_idx = 0; m_idx < circuit.moduleSize(); m_idx++)
    {
        Module module = circuit.module(m_idx);
        for(size_t c_idx = 0; c_idx < module.cellSize(); c_idx++)
        {
            Cell cell = module.cell(c_idx);
            if(cell.type().find("FF") && cell.hasInput("CK"))
            {
                module.removeNode(cell);

                for(size_t pin_i = 0; pin_i < cell.inputSize(); pin_i++)
                {
                    if(cell.inputPinName(pin_i) == "CK") continue;

                    Node nodei = cell.input(pin_i); // Port or Wire
                    if(!nodei.isNull())
                    {
                        Port ppo = module.createPort(EDAUTILS_PPO_PREFIX + nodei.name(), Port::PortType::PPO);
                        nodei.connect(Node::dir2str(Node::Direct::left), ppo);
                    }
                }
                for(size_t pin_o = 0; pin_o < cell.outputSize(); pin_o++)
                {
                    Node nodeo = cell.output(pin_o); // Wire
                    if(!nodeo.isNull())
                    {
                        Port ppi = module.createPort(EDAUTILS_PPI_PREFIX + nodeo.name(), Port::PortType::PPI);
                        nodeo.connect(Node::dir2str(Node::Direct::right), ppi);
                    }
                }
            }
        }
    }
}

static std::string _genWireName(const Cell &baseCell, const std::string &pin)
{
    std::ostringstream oss;
    oss << EDAUTILS_WIRE_SIGN << EDAUTILS_SCOPE_SIGN;
    oss << baseCell.name() << EDAUTILS_SCOPE_SIGN << pin;
    return oss.str();
}

static std::string _genPortName(const Cell &baseCell, const std::string &pin)
{
    std::ostringstream oss;
    oss << EDAUTILS_PORT_SIGN << EDAUTILS_SCOPE_SIGN;
    oss << baseCell.name() << EDAUTILS_SCOPE_SIGN << pin;
    return oss.str();
}

static std::string _genCellName(Circuit &circuit)
{
    static unsigned cellIdx = 0;
    std::ostringstream oss;
    oss << circuit.topModule().name() << EDAUTILS_SCOPE_SIGN;
    oss << EDAUTILS_SIGN << EDAUTILS_SCOPE_SIGN << EDAUTILS_CELL_SIGN << EDAUTILS_SCOPE_SIGN << cellIdx;
    cellIdx++;
    return oss.str();
}

bool EDAUtils::mux_connect_interal(Circuit &circuit, Cell &target, CellLibrary &library, vector<Cell> &newCells)
{
    if(library.hasCell("MUX2_X1"))
    {
        for(size_t out = 0; out < target.outputSize(); out++)
        {
            Node outNode = target.output(out);
            if(outNode.isWire() || outNode.isPort())
            {
                Cell newCell = library.cell("MUX2_X1");
                newCell.setName(_genCellName(circuit));
                
                target.breakOutputConnection(target.outputPinName(out));
                Wire ta = circuit.topModule().createWire(_genWireName(newCell, "A"));
                Wire tb = circuit.topModule().createWire(_genWireName(newCell, "B"));
                tb.setInternal(true);
                tb.setValue(0);
                Wire sel = circuit.topModule().createWire(_genWireName(newCell, "S"));
                sel.setInternal(true);
                sel.setValue(0);
                /* Port tb = circuit.topModule().createPort(_genPortName(newCell, "B"), Port::PortType::Input); */
                /* Port sel = circuit.topModule().createPort(_genPortName(newCell, "S"), Port::PortType::Input); */

                newCell.connect("A", ta);
                newCell.connect("B", tb);
                newCell.connect("S", sel);

                newCell.connect("Z", outNode);
                target.connect(target.outputPinName(out), ta);

                newCells.push_back(newCell);
            }
        }
        return true;
    }
    else
        return false;
}

bool EDAUtils::insertCell2AllCellOutputs(Circuit &circuit, CellLibrary &library, bool (*CALLBACK)(Circuit &circuit, Cell &target, CellLibrary &library, vector<Cell> &newCells))
{
    vector<Cell> allCells;
    for(size_t idx = 0; idx < circuit.topModule().cellSize(); idx++)
    {
        Cell cell = circuit.topModule().cell(idx);
        if(!CALLBACK(circuit, cell, library, allCells))
            return false;
    }

    for(vector<Cell>::iterator it = allCells.begin(); it != allCells.end(); it++)
        circuit.topModule().addCell(*it);

    return true;
}

void EDAUtils::timeFrameExpansion(Circuit &circuit, unsigned cycles)
{
    
}

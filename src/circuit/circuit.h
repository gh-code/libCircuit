#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <iterator>

class CellLibrary;

class NodePrivate;
class PortPrivate;
class WirePrivate;
class GatePrivate;
class CellPrivate;
class ModulePrivate;
class CircuitPrivate;

class Node;
class Port;
class Wire;
class Gate;
class Cell;
class Module;
class Circuit;

typedef std::string Pattern;
class Signal
{
public:
    enum SignalType {
        F = 0,  // False
        T = 1,  // True
        Z = 2,  // High Impedance
        X = 3,  // Unknown
        Last
    };
    Signal();
    Signal(unsigned s);
    Signal operator& (const Signal&);
    Signal operator&= (const Signal&);
    Signal operator| (const Signal&);
    Signal operator|= (const Signal&);
    Signal operator~ ();
    Signal operator! ();
    friend std::ostream& operator<< (std::ostream&, const Signal&);

private:
    unsigned value;
};

class Node
{
public:
    enum NodeType {
        PortNode    = 1,
        WireNode    = 2,
        GateNode    = 3,
        CellNode    = 4,
        ModuleNode  = 5,
        CircuitNode = 6,
        BaseNode    = 11
    };

    enum Direct {
        left        = 0,
        right       = 1,
        UNKNOW      = 2
    };
    inline static std::string dir2string(Node::Direct dir) { return dir == Node::left ? "left" : "right"; }

    Node();
    Node(const Node&);
    Node& operator= (const Node&);
    bool operator== (const Node&) const;
    bool operator!= (const Node&) const;
    ~Node();

    size_t inputSize() const;
    size_t outputSize() const;
    bool hasInput(const std::string &name) const;
    bool hasOutput(const std::string &name) const;
    Node input(const std::string &name) const;
    Node output(const std::string &name) const;
    Node input(size_t i) const;
    Node output(size_t i) const;
    void addInput(Node &node);
    void addOutput(Node &node);
    void addInputPinName(const std::string &pinName);
    void addOutputPinName(const std::string &pinName);
    void connect(const std::string&, Node node);
    void connectInput(size_t, Node node);
    void connectOutput(size_t, Node node);

    virtual void eval();

    Node insertAfter(const Node &node);
    Node cloneNode(bool deep = true) const;

    inline std::string name() const { return nodeName(); }
    std::string nodeName() const;
    NodeType nodeType() const;

    bool isPort() const;
    bool isWire() const;
    bool isGate() const;
    bool isCell() const;
    bool isModule() const;
    bool isCircuit() const;

    bool isNull() const;
    void clear();

    Port toPort() const;
    Wire toWire() const;
    Gate toGate() const;
    Cell toCell() const;
    Module toModule() const;
    Circuit toCircuit() const;

    Signal value() const;
    void setValue(Signal value);

protected:
    NodePrivate* impl;
    Node(NodePrivate*);
};

class Port : public Node
{
public:
    enum PortType {
        Input   = 1,
        Output  = 2,
        PPI     = 3,
        PPO     = 4,
        BasePort = 11,
    };
    Port();
    Port(const std::string&, PortType);
    Port(const Port&);
    Port& operator= (const Port&);

    PortType type() const;

    // Overridden from Node
    inline Node::NodeType nodeType() const { return PortNode; }

private:
    Port(PortPrivate*);

    friend class Node;
    friend class Module;
};

class Wire : public Node
{
public:
    Wire();
    Wire(const Wire&);
    Wire& operator= (const Wire&);

    // Overridden from Node
    inline Node::NodeType nodeType() const { return WireNode; }

private:
    Wire(WirePrivate*);

    friend class Node;
    friend class Module;
};

class Gate : public Node
{
public:
    enum GateType {
        INV     = 1,
        BUF     = 2,
        NAND    = 3,
        AND     = 4,
        NOR     = 5,
        OR      = 6,
        XNOR    = 7,
        XOR     = 8,
        BaseGate = 21,
        CustomGate = 22
    };

    Gate();
    Gate(const std::string &name, GateType type);
    Gate(const Gate&);
    Gate& operator= (const Gate&);

    void setName(const std::string &name);
    int level() const;
    void setLevel(int level);
    Gate::GateType gateType() const;

    std::string outputWireName() const;

    // Overridden from Node
    void eval();
    inline Node::NodeType nodeType() const { return GateNode; }

private:
    Gate(GatePrivate*);

    friend class Node;
    friend class Cell;
    friend class Module;
};

class Cell : public Gate
{
public:
    Cell();
    explicit Cell(const std::string &type);
    Cell(const std::string &name, const std::string &type);
    Cell(const Cell&);
    Cell& operator= (const Cell&);

    //Gate::GateType gateType() const;
    std::string type() const;

    double area() const;
    //double delay() const;
    //double slew() const;
    double inputCapacitance(size_t index) const;
    double inputCapacitance(const std::string &pinName) const;
    double inputCapacitanceRise(size_t index) const;
    double inputCapacitanceRise(const std::string &pinName) const;
    double inputCapacitanceFall(size_t index) const;
    double inputCapacitanceFall(const std::string &pinName) const;

    std::string inputPinName(size_t i);
    std::string outputPinName(size_t i);
    Port::PortType pinType(size_t) const;

    void setArea(double);
    void setInputCapacitance(const std::string&, double cap);
    void setInputCapacitanceRise(const std::string&, double cap);
    void setInputCapacitanceFall(const std::string&, double cap);

    void addInputPinName(const std::string&);
    void addOutputPinName(const std::string&);

    // Overridden from Node
    inline Node::NodeType nodeType() const { return CellNode; }

private:
    Cell(CellPrivate*);

    friend class Node;
    friend class Module;
};

class Module : public Node
{
public:
    Module();
    Module(const Module&);
    Module& operator= (const Module&);

    // Read
    Pattern input() const;
    Pattern output() const;
    // Write
    bool input(Pattern &pattern);
    bool output(Pattern &pattern);

    Port PI(size_t i) const;  // exclude PPIs
    Port PO(size_t i) const;  // exclude PPOs
    Port PPI(size_t i) const;
    Port PPO(size_t i) const;
    Port inputPort(size_t i) const;  // include PPIs
    Port outputPort(size_t i) const; // include PPOs

    size_t PISize() const;
    size_t POSize() const;
    size_t PPISize() const;
    size_t PPOSize() const;
    // size_t inputSize() const;  // inherits Node
    // size_t outputSize() const; // inherits Node
    size_t portSize() const;
    size_t wireSize() const;
    size_t gateSize() const; // see gateCount()
    size_t cellSize() const; // see gateCount()

    bool hasPort(const std::string&) const;
    bool hasWire(const std::string&) const;
    bool hasGate(const std::string&) const;
    bool hasCell(const std::string&) const;

    Port port(size_t i) const;
    Wire wire(size_t i) const;
    Gate gate(size_t i) const;
    Cell cell(size_t i) const;
    Port port(const std::string&) const;
    Wire wire(const std::string&) const;
    Gate gate(const std::string&) const;
    Cell cell(const std::string&) const;

    size_t gateCount() const; // return gateSize() or cellSize() if present
    inline size_t size() const { return gateCount(); }

    void addCell(Cell &cell);

    Port createPort(const std::string&, Port::PortType);
    Wire createWire(const std::string&);
    Gate createGate(const std::string&, Gate::GateType);
    Cell createCell(const std::string&, const std::string&);
    bool removeNode(Node &node);

private:
    Node input(const std::string &name) const;
    Node output(const std::string &name) const;
    Module(ModulePrivate*);

    friend class Node;
    friend class Circuit;
};

class Circuit : public Node
{
public:
    Circuit();
    explicit Circuit(const std::string &path);
    Circuit(const std::string &path, CellLibrary &lib);
    Circuit(const Circuit&);
    Circuit& operator= (const Circuit&);

    // Reimplemented from Node
    size_t inputSize() const        { return topModule().inputSize(); }
    size_t outputSize() const       { return topModule().outputSize(); }
    Port inputPort(size_t i) const  { return topModule().inputPort(i); }
    Port outputPort(size_t i) const { return topModule().outputPort(i); }

    // Read
    Pattern input() const;
    Pattern output() const;
    // Write
    bool input(const Pattern &pattern);
    bool output(const Pattern &pattern);

    inline Port PI(size_t i) const  { return topModule().PI(i); }
    inline Port PO(size_t i) const  { return topModule().PO(i); }
    inline Port PPI(size_t i) const { return topModule().PPI(i); }
    inline Port PPO(size_t i) const { return topModule().PPO(i); }

    inline size_t PISize() const    { return topModule().PISize(); }
    inline size_t POSize() const    { return topModule().POSize(); }
    inline size_t PPISize() const   { return topModule().PPISize(); }
    inline size_t PPOSize() const   { return topModule().PPOSize(); }

    size_t gateCount() const;
    inline size_t size() const { return gateCount(); }

    size_t moduleSize() const;
    Module module(size_t i) const;
    Module module(const std::string &name) const;

    Module topModule() const;
    void setTopModule(Module &module);

    Module createModule(const std::string &name);

    void load(std::fstream&, const std::string&);
    void load(std::fstream&, const std::string&, CellLibrary &lib);

    inline Node::NodeType nodeType() const { return CircuitNode; }

    class iterator
    {
    public:
        std::list<Node*>::iterator it;
        typedef std::forward_iterator_tag iterator_category;
        typedef int difference_type;
        typedef Node value_type;
        typedef Node *pointer;
        typedef Node &reference;

        inline iterator(const std::list<Node*>::iterator i) : it(i) {}
        inline Node &operator*() const { return *(*it); }
        inline Node *operator->() const { return (*it); }
        inline bool operator==(const iterator &o) const { return it == o.it; }
        inline bool operator!=(const iterator &o) const { return it != o.it; }
        inline iterator &operator++() { ++it; return *this; }
        inline iterator operator++(int) { std::list<Node*>::iterator i = it; ++it; return i; }
    };

    class const_iterator
    {
    public:
    };

    // iterator begin() const { return iterator(nodes.begin()); }
    // iterator end() const { return iterator(nodes.end()); }
    // const_iterator begin() const {}
    // const_iterator end() const {}
    // const_iterator cbegin() const {}
    // const_iterator cend() const {}

private:
    Circuit(CircuitPrivate*);

    friend class Node;
};

inline const char * type_str(Node::NodeType type)
{
    switch (type)
    {
        case Node::PortNode:    return "PortNode";
        case Node::WireNode:    return "WireNode";
        case Node::GateNode:    return "GateNode";
        case Node::CellNode:    return "CellNode";
        case Node::ModuleNode:  return "ModuleNode";
        case Node::CircuitNode: return "CircuitNode";
        case Node::BaseNode:    return "BaseNode";
    };
    return 0;
}

inline const char * type_str(Port::PortType type)
{
    switch (type)
    {
        case Port::Input:       return "Input";
        case Port::Output:      return "Output";
        case Port::PPI:         return "PPI";
        case Port::PPO:         return "PPO";
        case Port::BasePort:    return "BasePort";
    }
    return 0;
}

inline const char * type_str(Gate::GateType type)
{
    switch (type)
    {
        case Gate::INV:         return "INV";
        case Gate::BUF:         return "BUF";
        case Gate::NAND:        return "NAND";
        case Gate::AND:         return "AND";
        case Gate::NOR:         return "NOR";
        case Gate::OR:          return "OR";
        case Gate::XNOR:        return "XNOR";
        case Gate::XOR:         return "XOR";
        case Gate::BaseGate:    return "BaseGate";
        case Gate::CustomGate:  return "CustomGate";
    }
    return 0;
}

#endif // CIRCUIT_H

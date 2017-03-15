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
typedef unsigned Signal;

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

    Node insertAfter(const Node &node);
    Node cloneNode(bool deep = true) const;

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
        BasePort = 11,
    };
    Port();
    Port(const std::string&, PortType);
    Port(const Port&);
    Port& operator= (const Port&);

    std::string name() const;
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

    std::string name() const;

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

    std::string name() const;
    Gate setName(const std::string &name);
    Gate::GateType gateType() const;

    // Overridden from Node
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

    std::string name() const;
    Cell setName(const std::string &name);
    std::string type() const;
    double area() const;
    double delay() const;
    double slew() const;

    void addInputPinName(const std::string&);
    void addOutputPinName(const std::string&);
    Port::PortType pinType(size_t) const;

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

    inline std::string name() const { return nodeName(); }

    // Read
    Pattern input() const;
    Pattern output() const;
    // Write
    bool input(Pattern &pattern);
    bool output(Pattern &pattern);

    Port inputPort(size_t i) const;
    Port outputPort(size_t i) const;

    size_t portSize() const;
    size_t wireSize() const;
    size_t gateSize() const;
    size_t cellSize() const;

    Port port(const std::string&) const;
    Wire wire(const std::string&) const;
    Gate gate(const std::string&) const;
    Cell cell(const std::string&) const;

    size_t gateCount() const;
    inline size_t size() const { return gateCount(); }

    void addCell(Cell &cell);

    Port createPort(const std::string&, Port::PortType);
    Wire createWire(const std::string&);
    Gate createGate(const std::string&, Gate::GateType);
    Cell createCell(const std::string&, const std::string&);

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
    inline std::string name() const { return nodeName(); }
    size_t inputSize() const        { return topModule().inputSize(); }
    size_t outputSize() const       { return topModule().outputSize(); }
    Port inputPort(size_t i) const  { return topModule().inputPort(i); }
    Port outputPort(size_t i) const { return topModule().outputPort(i); }

    // Read
    Pattern input() const;
    Pattern output() const;
    // Write
    bool input(Pattern &pattern);
    bool output(Pattern &pattern);

    size_t gateCount() const;
    inline size_t size() const { return gateCount(); }

    size_t moduleSize() const;
    Module module(size_t i) const;
    Module module(const std::string &name) const;

    Module topModule() const;
    void setTopModule(Module &module);

    inline Port createPort(const std::string &portName, Port::PortType type)    { return topModule().createPort(portName, type); }
    inline Wire createWire(const std::string &wireName)                         { return topModule().createWire(wireName); }
    inline Gate createGate(const std::string &gateName, Gate::GateType type)    { return topModule().createGate(gateName,type); }
    inline Cell createCell(const std::string &cellName, const std::string &type){ return topModule().createCell(cellName,type); }
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

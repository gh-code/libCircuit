#include "circuit.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>
#include <qatomic.h>
#include "../parser/verilog/driver.h"
#include "../parser/verilog/expression.h"

//#define DEBUG 1

/**************************************************************
 *
 * Private class declerations
 *
 **************************************************************/
class NodePrivate
{
public:
    NodePrivate(CircuitPrivate*, NodePrivate* parent = 0);
    NodePrivate(NodePrivate* n, bool deep);
    virtual ~NodePrivate();

    std::string nodeName() const { return name; }
    Signal nodeValue() const { return value; }
    void setNodeValue(Signal &v) { value = v; }

    CircuitPrivate* ownerCircuit();
    void setOwnerCircuit(CircuitPrivate* c);

    size_t inputSize() const;
    size_t outputSize() const;
    bool hasInput(const std::string &name) const;
    bool hasOutput(const std::string &name) const;
    NodePrivate* input(const std::string &name) const;
    NodePrivate* output(const std::string &name) const;
    NodePrivate* input(size_t i) const;
    NodePrivate* output(size_t i) const;
    void addInput(NodePrivate *node);
    void addOutput(NodePrivate *node);
    void addInputPinName(const std::string &pinName);
    void addOutputPinName(const std::string &pinName);
    void connect(const std::string &pin, NodePrivate *targ);
    void connectInput(size_t pin, NodePrivate *targ);
    void connectOutput(size_t pin, NodePrivate *targ);

    virtual NodePrivate* cloneNode(bool deep = true);
    void clear();

    inline NodePrivate* parent() const { return hasParent ? ownerNode : 0; }
    inline void setParent(NodePrivate *p) { ownerNode = p; hasParent = true; }

    bool isPort() const     { return nodeType() == Node::PortNode; }
    bool isWire() const     { return nodeType() == Node::WireNode; }
    bool isGate() const     { return nodeType() == Node::GateNode; }
    bool isCell() const     { return nodeType() == Node::CellNode; }
    bool isModule() const   { return nodeType() == Node::ModuleNode; }
    bool isCircuit() const  { return nodeType() == Node::CircuitNode; }

    virtual Node::NodeType nodeType() const { return Node::BaseNode; }

    QAtomicInt ref;
    NodePrivate* ownerNode;
    std::map<std::string,NodePrivate*> inputs;
    std::map<std::string,NodePrivate*> outputs;
    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
    bool hasParent : 1;

    std::string name;
    Signal value;
};

class PortPrivate : public NodePrivate
{
public:
    PortPrivate(CircuitPrivate*, NodePrivate* parent, const std::string &name, Port::PortType type);
    PortPrivate(PortPrivate* n, bool deep);
    ~PortPrivate();

    Port::PortType type;
};

class WirePrivate : public NodePrivate
{
public:
    WirePrivate(CircuitPrivate*, NodePrivate* parent, const std::string &name);
    WirePrivate(WirePrivate* n, bool deep);
    ~WirePrivate();

    // Reimplemented from NodePrivate
    NodePrivate* cloneNode(bool deep = true);
    Node::NodeType nodeType() const { return Node::GateNode; }
};

class GatePrivate : public NodePrivate
{
public:
    GatePrivate(GatePrivate* n, bool deep);
    GatePrivate(CircuitPrivate*, NodePrivate* parent, const std::string &name, Gate::GateType type);
    ~GatePrivate();

    Gate::GateType gateType() const { return type; }

    // Reimplemented from NodePrivate
    NodePrivate* cloneNode(bool deep = true);
    Node::NodeType nodeType() const { return Node::GateNode; }

    Gate::GateType type;
};

class CellPrivate : public GatePrivate
{
public:
    CellPrivate(CellPrivate* n, bool deep);
    CellPrivate(CircuitPrivate*, NodePrivate* parent, const std::string &name, const std::string &type);
    ~CellPrivate();

    std::string cellType() const { return type; }

    void setName(const std::string &name);

    // Reimplemented from NodePrivate
    NodePrivate* cloneNode(bool deep = true);
    Node::NodeType nodeType() const { return Node::CellNode; }

    double area;
    std::string type;
};

class ModulePrivate : public NodePrivate
{
public:
    ModulePrivate(CircuitPrivate*, NodePrivate* parent, const std::string &name);
    ModulePrivate(ModulePrivate* n, bool deep);
    ~ModulePrivate();

    size_t gateCount() const;

    void addCell(CellPrivate *);

    bool hasPort(const std::string &portName);
    bool hasWire(const std::string &wireName);
    bool hasGate(const std::string &gateName);
    bool hasCell(const std::string &cellName);

    PortPrivate* port(const std::string &portName);
    WirePrivate* wire(const std::string &wireName);
    GatePrivate* gate(const std::string &gateName);
    CellPrivate* cell(const std::string &cellName);

    PortPrivate* createPort(const std::string &portName, Port::PortType);
    WirePrivate* createWire(const std::string &wireName);
    GatePrivate* createGate(const std::string &gateName, Gate::GateType);
    CellPrivate* createCell(const std::string &cellName, const std::string&);

    std::map<std::string,PortPrivate*> ports;
    std::map<std::string,WirePrivate*> wires;
    std::map<std::string,CellPrivate*> cells;
    std::map<std::string,GatePrivate*> gates;
};

class CircuitPrivate : public NodePrivate
{
public:
    CircuitPrivate();
    CircuitPrivate(const std::string &name);
    CircuitPrivate(CircuitPrivate* n, bool deep);
    ~CircuitPrivate();

    size_t gateCount() const;

    bool hasModule(const std::string &moduleName);
    ModulePrivate* module(const std::string &moduleName);
    ModulePrivate* createModule(const std::string &moduleName);

    void setTopModule(ModulePrivate *module);

    ModulePrivate *topModule;
    std::map<std::string,ModulePrivate*> modules;
    std::vector<std::string> moduleNames;
};

/**************************************************************
 *
 * NodePrivate
 *
 **************************************************************/

inline void NodePrivate::setOwnerCircuit(CircuitPrivate *c)
{
    ownerNode = c;
    hasParent = false;
}

NodePrivate::NodePrivate(CircuitPrivate *c, NodePrivate *parent) : ref(1)
{
    if (parent)
        setParent(parent);
    else
        setOwnerCircuit(c);
}

NodePrivate::NodePrivate(NodePrivate* n, bool deep) : ref(1)
{
    setOwnerCircuit(n->ownerCircuit());
    name = n->name;
    inputNames = n->inputNames;
    outputNames = n->outputNames;
    inputs = n->inputs;
    outputs = n->outputs;
    std::map<std::string,NodePrivate*>::iterator it;
    for (it = inputs.begin(); it != inputs.end(); ++it)
        it->second = 0;
    for (it = outputs.begin(); it != outputs.end(); ++it)
        it->second = 0;

    if (!deep)
        return;
}

NodePrivate::~NodePrivate()
{
    // std::map<std::string,NodePrivate*>::iterator it;
    // for (it = outputs.begin(); it != outputs.end(); ++it)
    //     if (!it->second->ref.deref())
    //         delete it->second;
    // for (it = inputs.begin(); it != inputs.end(); ++it)
    //     if (!it->second->ref.deref())
    //         delete it->second;
}

CircuitPrivate* NodePrivate::ownerCircuit()
{
    NodePrivate *p = this;
    while (p && !p->isCircuit())
    {
        if (!p->hasParent)
            return (CircuitPrivate*)p->ownerNode;
        p = p->parent();
    }
    return static_cast<CircuitPrivate*>(p);
}

NodePrivate* NodePrivate::cloneNode(bool deep)
{
    NodePrivate *p = new NodePrivate(this, deep);
    // We are not interested in this node
    p->ref.deref();
    return p;
}

size_t NodePrivate::inputSize() const
{
    return inputs.size();
}

size_t NodePrivate::outputSize() const
{
    return outputs.size();
}

bool NodePrivate::hasInput(const std::string &name) const
{
    return (inputs.find(name) != inputs.end());
}

bool NodePrivate::hasOutput(const std::string &name) const
{
    return (outputs.find(name) != outputs.end());
}

NodePrivate* NodePrivate::input(const std::string &name) const
{
    return inputs.at(name);
}

NodePrivate* NodePrivate::output(const std::string &name) const
{
    return outputs.at(name);
}

NodePrivate* NodePrivate::input(size_t i) const
{
    std::string name = inputNames[i];
    return inputs.at(name);
}

NodePrivate* NodePrivate::output(size_t i) const
{
    std::string name = outputNames[i];
    return outputs.at(name);
}

void NodePrivate::addInput(NodePrivate *node)
{
    inputs[node->name] = node;
    inputNames.push_back(node->name);
}

void NodePrivate::addOutput(NodePrivate *node)
{
    outputs[node->name] = node;
    outputNames.push_back(node->name);
}

void NodePrivate::addInputPinName(const std::string &pinName)
{
    inputs[pinName] = 0;
    inputNames.push_back(pinName);
}

void NodePrivate::addOutputPinName(const std::string &pinName)
{
    outputs[pinName] = 0;
    outputNames.push_back(pinName);
}

void NodePrivate::connect(const std::string &pin, NodePrivate *targ)
{
    if (hasOutput(pin))
    {
        targ->ref.ref();
        outputs[pin] = targ;
        //targ->ref.ref();
        std::ostringstream oss;
        oss << targ->inputNames.size();
        targ->inputNames.push_back(oss.str());
        targ->inputs[oss.str()] = this;
    }
    else if (hasInput(pin))
    {
        targ->ref.ref();
        inputs[pin] = targ;
        //this->ref.ref();
        std::ostringstream oss;
        oss << targ->outputNames.size();
        targ->outputNames.push_back(oss.str());
        targ->outputs[oss.str()] = this;
    }
    else
    {
        std::cerr << "No such pin: " << pin << std::endl;
    }
}

void NodePrivate::connectInput(size_t pin, NodePrivate *targ)
{
    std::string pinName;
    if (nodeType() == Node::CellNode)
    {
        if (pin >= inputs.size())
        {
            std::cerr << "Invalid output pin number: " << pin << std::endl;
            return;
        }
        pinName = inputNames[pin];
    }
    else
    {
        std::ostringstream oss;
        if (pin >= inputs.size())
        {
            oss << inputNames.size();
            inputNames.push_back(oss.str());
        }
        else
            oss << pin;
        pinName = oss.str();
    }

    // TODO handle reconnect

    std::ostringstream oss;
    oss << targ->outputNames.size();
    targ->outputNames.push_back(oss.str());
    targ->outputs[oss.str()] = this;
    //this->ref.ref();

    inputs[pinName] = targ;
    targ->ref.ref();
}

void NodePrivate::connectOutput(size_t pin, NodePrivate *targ)
{
    std::string pinName;
    if (nodeType() == Node::CellNode)
    {
        if (pin >= outputs.size())
        {
            std::cerr << "Invalid output pin number: " << pin << std::endl;
            return;
        }
        pinName = outputNames[pin];
    }
    else
    {
        std::ostringstream oss;
        if (pin >= outputs.size())
        {
            oss << outputNames.size();
            outputNames.push_back(oss.str());
        }
        else
            oss << pin;
        pinName = oss.str();
    }

    // TODO handle reconnect

    std::ostringstream oss;
    oss << targ->inputNames.size();
    targ->inputNames.push_back(oss.str());
    targ->inputs[oss.str()] = this;
    //this->ref.ref();

    outputs[pinName] = targ;
    targ->ref.ref();
}


/**************************************************************
 *
 * Node
 *
 **************************************************************/

#define IMPL ((NodePrivate*)impl)

Node::Node()
{
    impl = 0;
}

Node::Node(const Node& n)
{
    impl = n.impl;
    if (impl)
        impl->ref.ref();
}

Node::Node(NodePrivate *n)
{
    impl = n;
    if (impl)
        impl->ref.ref();
}

Node& Node::operator=(const Node &n)
{
    if (n.impl)
        n.impl->ref.ref();
    if (impl && !impl->ref.deref())
        delete impl;
    impl = n.impl;
    return *this;
}

bool Node::operator==(const Node &n) const
{
    return (impl == n.impl);
}

bool Node::operator!=(const Node &n) const
{
    return (impl != n.impl);
}

Node::~Node()
{
    if (impl && !impl->ref.deref())
        delete impl;
}

size_t Node::inputSize() const
{
    if (!impl)
        return 0;
    return IMPL->inputSize();
}

size_t Node::outputSize() const
{
    if (!impl)
        return 0;
    return IMPL->outputSize();
}

bool Node::hasInput(const std::string &name) const
{
    if (!impl)
        return false;
    return IMPL->hasInput(name);
}

bool Node::hasOutput(const std::string &name) const
{
    if (!impl)
        return false;
    return IMPL->hasOutput(name);
}

Node Node::input(const std::string &name) const
{
    if (!impl)
        return Node();
    return Node(IMPL->input(name));
}

Node Node::output(const std::string &name) const
{
    if (!impl)
        return Node();
    return Node(IMPL->output(name));
}

Node Node::input(size_t i) const
{
    if (!impl)
        return Node();
    return Node(IMPL->input(i));
}

Node Node::output(size_t i) const
{
    if (!impl)
        return Node();
    return Node(IMPL->output(i));
}

void Node::addInput(Node &node)
{
    if (!impl)
        return;
    return IMPL->addInput(node.impl);
}

void Node::addOutput(Node &node)
{
    if (!impl)
        return;
    return IMPL->addOutput(node.impl);
}

void Node::addInputPinName(const std::string &pinName)
{
    if (!impl)
        return;
    return IMPL->addInputPinName(pinName);
}

void Node::addOutputPinName(const std::string &pinName)
{
    if (!impl)
        return;
    return IMPL->addOutputPinName(pinName);
}

void Node::connect(const std::string &pinName, Node targ)
{
    if (!impl)
        return;
    IMPL->connect(pinName, targ.impl);
}

void Node::connectInput(size_t pin, Node targ)
{
    if (!impl)
        return;
    IMPL->connectInput(pin, targ.impl);
}

void Node::connectOutput(size_t pin, Node targ)
{
    if (!impl)
        return;
    IMPL->connectOutput(pin, targ.impl);
}

// void Node::connect(const std::string &pin, Node &targ, const std::string &targPin)
// {
//     if (!impl)
//         return;
//     IMPL->connect(pin, targ.impl, targPin);
// }

Node Node::cloneNode(bool deep) const
{
    if (!impl)
        return Node();
    return Node(IMPL->cloneNode(deep));
}

std::string Node::nodeName() const
{
    if (!impl)
        return std::string();
    return IMPL->name;
}

Node::NodeType Node::nodeType() const
{
    if (!impl)
        return Node::BaseNode;
    return IMPL->nodeType();
}

bool Node::isNull() const
{
    return (impl == 0);
}

void Node::clear()
{
    if (impl && !impl->ref.deref())
        delete impl;
    impl = 0;
}

bool Node::isPort() const
{
    if (impl)
        return impl->isPort();
    return false;
}

bool Node::isWire() const
{
    if (impl)
        return impl->isWire();
    return false;
}

bool Node::isGate() const
{
    if (impl)
        return impl->isGate();
    return false;
}

bool Node::isCell() const
{
    if (impl)
        return impl->isCell();
    return false;
}

bool Node::isModule() const
{
    if (impl)
        return impl->isModule();
    return false;
}

bool Node::isCircuit() const
{
    if (impl)
        return impl->isCircuit();
    return false;
}

#undef IMPL

/**************************************************************
 *
 * PortPrivate
 *
 **************************************************************/

PortPrivate::PortPrivate(CircuitPrivate *c, NodePrivate* p, const std::string &name_, Port::PortType type_)
    : NodePrivate(c, p)
{
    name = name_;
    type = type_;
}

PortPrivate::~PortPrivate()
{
}

/**************************************************************
 *
 * Port
 *
 **************************************************************/

#define IMPL ((PortPrivate*)impl)

Port::Port()
    : Node()
{
}

Port::Port(const Port &x)
    : Node(x)
{
}

Port::Port(PortPrivate *x)
    : Node(x)
{
}

Port::Port(const std::string &name, Port::PortType type)
{
    impl = new PortPrivate(0, 0, name, type);
}

Port& Port::operator=(const Port& x)
{
    return (Port&) Node::operator=(x);
}

std::string Port::name() const
{
    if (!impl)
        return std::string();
    return IMPL->nodeName();
}

Port::PortType Port::type() const
{
    if (!impl)
        return Port::BasePort;
    return IMPL->type;
}

#undef IMPL

/**************************************************************
 *
 * WirePrivate
 *
 **************************************************************/

WirePrivate::WirePrivate(CircuitPrivate *c, NodePrivate* p, const std::string &name_)
: NodePrivate(c, p)
{
    name = name_;
}

WirePrivate::WirePrivate(WirePrivate* n, bool deep)
    : NodePrivate(n, deep)
{
}

WirePrivate::~WirePrivate()
{
}

NodePrivate* WirePrivate::cloneNode(bool deep)
{
    NodePrivate *p = new WirePrivate(this, deep);
    // We are not interested in this node
    p->ref.deref();
    return p;
}

/**************************************************************
 *
 * Wire
 *
 **************************************************************/

Wire::Wire()
    : Node()
{
}

Wire::Wire(const Wire &x)
    : Node(x)
{
}

Wire::Wire(WirePrivate *x)
    : Node(x)
{
}

Wire& Wire::operator=(const Wire &x)
{
    return (Wire&) Node::operator=(x);
}


/**************************************************************
 *
 * GatePrivate
 *
 **************************************************************/

GatePrivate::GatePrivate(GatePrivate* n, bool deep)
    : NodePrivate(n, deep)
{
    name = n->name;
    type = n->type;
}

GatePrivate::GatePrivate(CircuitPrivate *c, NodePrivate* p, const std::string &name_, Gate::GateType type_)
    : NodePrivate(c, p)
{
    name = name_;
    type = type_;
}

GatePrivate::~GatePrivate()
{
}

NodePrivate* GatePrivate::cloneNode(bool deep)
{
    NodePrivate *p = new GatePrivate(this, deep);
    // We are not interested in this node
    p->ref.deref();
    return p;
}

/**************************************************************
 *
 * Gate
 *
 **************************************************************/

#define IMPL ((GatePrivate*)impl)

Gate::Gate()
{
    impl = 0;
}

Gate::Gate(const std::string &name, Gate::GateType type)
{
    impl = new GatePrivate(0, 0, name, type);
}

Gate::Gate(const Gate &x)
    : Node(x)
{
}

Gate::Gate(GatePrivate *x)
    : Node(x)
{
}

Gate& Gate::operator=(const Gate &x)
{
    return (Gate&) Node::operator=(x);
}

Gate::GateType Gate::gateType() const
{
    if (!impl)
        return Gate::BaseGate;
    return IMPL->gateType();
}

#undef IMPL

/**************************************************************
 *
 * CellPrivate
 *
 **************************************************************/

CellPrivate::CellPrivate(CellPrivate* n, bool deep)
     : GatePrivate(n, deep)
 {
     name = n->name;
     type = n->type;
 }

CellPrivate::CellPrivate(CircuitPrivate *c, NodePrivate* p, const std::string &name_, const std::string &type_)
    : GatePrivate(c, p, name, Gate::CustomGate)
{
    name = name_;
    type = type_;
}

CellPrivate::~CellPrivate()
{
}

NodePrivate* CellPrivate::cloneNode(bool deep)
{
    NodePrivate *p = new CellPrivate(this, deep);
    // We are not interested in this node
    p->ref.deref();
    return p;
}

void CellPrivate::setName(const std::string &name_)
{
    name = name_;
}

/**************************************************************
 *
 * Cell
 *
 **************************************************************/

#define IMPL ((CellPrivate*)impl)

/*!
    Contructs an empty cell.
*/
Cell::Cell()
{
    impl = 0;
}

Cell::Cell(const std::string &type)
{
    impl = new CellPrivate(0, 0, "library", type);
}

Cell::Cell(const std::string &name, const std::string &type)
{
    impl = new CellPrivate(0, 0, name, type);
}

Cell::Cell(const Cell& x)
    : Gate(x)
{
}

Cell::Cell(CellPrivate *x)
    : Gate(x)
{
}

Cell& Cell::operator=(const Cell &x)
{
    return (Cell&) Gate::operator=(x);
}

std::string Cell::name() const
{
    if (!impl)
        return std::string();
    return IMPL->nodeName();
}

Cell Cell::setName(const std::string &name)
{
    if (!impl)
        return Cell();
    IMPL->setName(name);
    return Cell(IMPL);
}

std::string Cell::type() const
{
    if (!impl)
        return std::string();
    return IMPL->cellType();
}

#undef IMPL

/**************************************************************
 *
 * ModulePrivate
 *
 **************************************************************/

ModulePrivate::ModulePrivate(CircuitPrivate* c, NodePrivate* p, const std::string &name_)
    : NodePrivate(c, p)
{
    name = name_;
}

ModulePrivate::ModulePrivate(ModulePrivate* n, bool deep)
    : NodePrivate(n, deep)
{
    // Bad implementation
    // move to NodePrivate is quite correct on performance.
    wires = n->wires;
}

template <class T>
static void deleteNameMap(std::map<std::string,T*> &nm)
{
    typename std::map<std::string,T*>::iterator it;
    for (it = nm.begin(); it != nm.end(); ++it)
    {
        it->second->ref.deref();
        delete it->second;
    }
}

ModulePrivate::~ModulePrivate()
{
    //deleteNameMap(ports);
    //deleteNameMap(wires);
    deleteNameMap(cells);
    deleteNameMap(gates);
}

size_t ModulePrivate::gateCount() const
{
    if (!gates.empty() && !cells.empty())
    {
        std::cerr << "Bad circuit" << std::endl;
        return 0;
    }
    else if (!gates.empty())
        return gates.size();
    else if (!cells.empty())
        return cells.size();
    else
        return 0;
}

PortPrivate* ModulePrivate::port(const std::string &portName)
{
    return ports[portName];
}

WirePrivate* ModulePrivate::wire(const std::string &wireName)
{
    return wires[wireName];
}

GatePrivate* ModulePrivate::gate(const std::string &gateName)
{
    return gates[gateName];
}

CellPrivate* ModulePrivate::cell(const std::string &cellName)
{
    return cells[cellName];
}

void ModulePrivate::addCell(CellPrivate *cell)
{
    if (cells.find(cell->name) != cells.end())
        std::cerr << "WARNING: Duplicate add the same cell instance" << std::endl;
    cell->setOwnerCircuit(this->ownerCircuit());
    cell->setParent(this);
    cell->ref.ref();
    cells[cell->name] = cell;
}

WirePrivate* ModulePrivate::createWire(const std::string &wireName)
{
    // Duplicate creating wire
    if (wires.find(wireName) != wires.end())
        std::cerr << "WARNING: Duplicate create wire: " << wireName << std::endl;
    WirePrivate *w = new WirePrivate((CircuitPrivate*)this->ownerNode, this, wireName);
    // w->ref.deref();
    wires[wireName] = w;
    return w;
}

PortPrivate* ModulePrivate::createPort(const std::string &portName, Port::PortType type)
{
    if (ports.find(portName) != ports.end())
        std::cerr << "WARNING: Duplicate create port: " << portName << std::endl;
    PortPrivate *p = new PortPrivate((CircuitPrivate*)this->ownerNode, this, portName, type);
    switch (type)
    {
        case Port::Input:  addInput(p);  break;
        case Port::Output: addOutput(p); break;
        default:
            return 0;
    }
    ports[portName] = p;
    return p;
}

GatePrivate* ModulePrivate::createGate(const std::string &gateName, Gate::GateType type)
{
    // Duplicate creating wire
    if (gates.find(gateName) != gates.end())
        std::cerr << "WARNING: Duplicate create wire: " << gateName << std::endl;
    GatePrivate *w = new GatePrivate((CircuitPrivate*)this->ownerNode, this, gateName, type);
    // w->ref.deref();
    gates[gateName] = w;
    return w;
}

/**************************************************************
 *
 * Module
 *
 **************************************************************/

#define IMPL ((ModulePrivate*)impl)

Module::Module()
{
}

Module::Module(const Module &x)
    : Node(x)
{
}

Module::Module(ModulePrivate *x)
    : Node(x)
{
}

Port Module::inputPort(size_t i) const
{
    if (!impl)
        return Port();
    return Port((PortPrivate*)impl->input(i));
}

Port Module::outputPort(size_t i) const
{
    if (!impl)
        return Port();
    return Port((PortPrivate*)impl->output(i));
}

size_t Module::gateCount() const
{
    if (!impl)
        return 0;
    return IMPL->gateCount();
}

size_t Module::wireSize() const
{
    if (!impl)
        return 0;
    return IMPL->wires.size();
}

size_t Module::gateSize() const
{
    if (!impl)
        return 0;
    return IMPL->gates.size();
}

size_t Module::cellSize() const
{
    if (!impl)
        return 0;
    return IMPL->cells.size();
}

Port Module::port(const std::string &portName) const
{
    if (!impl)
        return Port();
    return Port(IMPL->port(portName));
}

Wire Module::wire(const std::string &wireName) const
{
    if (!impl)
        return Wire();
    return Wire(IMPL->wire(wireName));
}

Gate Module::gate(const std::string &gateName) const
{
    if (!impl)
        return Gate();
    return Gate(IMPL->gate(gateName));
}

Cell Module::cell(const std::string &cellName) const
{
    if (!impl)
        return Cell();
    return Cell(IMPL->cell(cellName));
}

void Module::addCell(Cell &cell)
{
    if (!impl)
        return;
    IMPL->addCell((CellPrivate*)cell.impl);
}

Port Module::createPort(const std::string &portName, Port::PortType type)
{
    if (!impl)
        return Port();
    return Port(IMPL->createPort(portName, type));
}

Wire Module::createWire(const std::string &wireName)
{
    if (!impl)
        return Wire();
    return Wire(IMPL->createWire(wireName));
}

Gate Module::createGate(const std::string &gateName, Gate::GateType type)
{
    if (!impl)
        return Gate();
    return Gate(IMPL->createGate(gateName, type));
}

#undef IMPL

/**************************************************************
 *
 * CircuitPrivate
 *
 **************************************************************/

CircuitPrivate::CircuitPrivate()
    : NodePrivate(0)
{
    name = "#circuit";
}

CircuitPrivate::CircuitPrivate(const std::string &name_)
    : NodePrivate(0)
{
    name = name_;
}

CircuitPrivate::CircuitPrivate(CircuitPrivate* n, bool deep)
    : NodePrivate(n, deep)
{
    topModule = n->topModule;
    modules = n->modules;
    moduleNames = n->moduleNames;
}

CircuitPrivate::~CircuitPrivate()
{
    deleteNameMap(modules);
}

size_t CircuitPrivate::gateCount() const
{
    size_t total = 0;
    std::map<std::string,ModulePrivate*>::const_iterator it;
    for (it = modules.begin(); it != modules.end(); ++it)
    {
        ModulePrivate *module = it->second;
        total += module->gateCount();
    }
    return total;
}

ModulePrivate* CircuitPrivate::createModule(const std::string &name) {
    ModulePrivate *m = new ModulePrivate(this, this, name);
    if (modules.find(name) != modules.end())
        return 0;
    moduleNames.push_back(name);
    modules[name] = m;
    //m->ref.deref();
    return m;
}

void CircuitPrivate::setTopModule(ModulePrivate *module)
{
    topModule = module;
}

/**************************************************************
 *
 * Circuit
 *
 **************************************************************/

#define IMPL ((CircuitPrivate*)impl)

Circuit::Circuit()
{
    impl = 0;
}

CellLibrary defaultLibrary;

Circuit::Circuit(const std::string &path, CellLibrary &lib)
{
    // Open a Verilog file
    std::fstream infile(path.c_str());
    if (!infile.good())
    {
        std::cerr << "Could not open file: " << path << std::endl;
        impl = 0;
        return;
    }
    load(infile, path, lib);
}

typedef struct
{
    int id;
    Port::PortType type;
    std::vector<std::vector<Port> > *list;
}
PortHandlerData;

static void handlePortRangeCallback(const std::string &name, void *d)
{
    PortHandlerData *data = (PortHandlerData *)d;
    Port port(name, data->type);
    int id = data->id;
    (*(data->list))[id].push_back(port);
}

static void handleWireRangeCallback(const std::string &name, void *d)
{
    Module *module = (Module *)d;
    Wire wire = module->createWire(name);
}

static void handlePortWireRange(const std::string &name, VNRange *range, void (*CALLBACK)(const std::string&, void *), void *data)
{
    if (!range)
    {
        CALLBACK(name, data);
    }
    else if (range->right() < range->left())
    {
        for (int k = range->left(); k >= range->right(); k--)
        {
            std::ostringstream oss;
            oss << name << '[' << k << ']';
            CALLBACK(oss.str(), data);
        }
    }
    else // if (portRange->left() < portRange->right())
    {
        for (int k = range->left(); k <= range->right(); k++)
        {
            std::ostringstream oss;
            oss << name << '[' << k << ']';
            CALLBACK(oss.str(), data);
        }
    }
}

void Circuit::load(std::fstream &infile, const std::string &path, CellLibrary &lib)
{
    if (impl && impl->ref.deref())
        delete impl;

    VerilogContext verilog;
    Verilog::Driver driver(verilog);
    //driver.trace_scanning = true;
    //driver.trace_parsing = true;

    // Start parsing the Verilog file
    verilog.clearExpressions();
    bool result = driver.parse_stream(infile, path);
    if (!result || verilog.expressions.size() == 0)
    {
        impl = 0;
        return;
    }

    // Use top module name as the circuit name
    VNModule *topModule = ((VNModule*)(verilog.expressions[0]));
    impl = new CircuitPrivate(topModule->name());

    // TODO implement it
    if (verilog.expressions.size() > 1)
    {
        std::cerr << "Error: Multiple module feature is not supported yet" << std::endl;
    }

    // Start building circuit
    for (unsigned int ei = 0; ei < verilog.expressions.size(); ++ei)
    {
        VNModule *vmodule = ((VNModule*)verilog.expressions[ei]);
        Module module = createModule(vmodule->name());
        if (ei == 0)
            setTopModule(module);

        // Create port list of current module
        std::map<std::string,int> portListDef;
        std::vector<std::vector<Port> > portList;
        if (vmodule->portSize() > 0)
        {
            for (size_t i = 0; i < vmodule->portSize(); i++) {
                portListDef[vmodule->port(i)->name()] = i;
                std::vector<Port> tmp;
                portList.push_back(tmp);
            }
        }
        // Duplicate listing port names error
        if (vmodule->portSize() != portListDef.size())
        {
            std::cerr << "Duplicate listing port names." << std::endl;
            delete impl;
            impl = 0;
            return;
        }

        // Inputs
        for (size_t i = 0; i < vmodule->inputSize(); i++)
        {
            VNInput *input = vmodule->input(i);
            VNRange *range = input->range();

            for (size_t j = 0; j < input->varSize(); j++)
            {
                std::string inputName = input->var(j)->name();

                std::map<std::string,int>::iterator found = portListDef.find(inputName);
                if (found == portListDef.end())
                {
                    std::cerr << "Port is not in the list: " << inputName << std::endl;
                    delete impl;
                    impl = 0;
                    return;
                }
                int id = found->second;
                portListDef.erase(found);

                PortHandlerData data;
                data.id = id;
                data.type = Port::Input;
                data.list = &portList;
                handlePortWireRange(inputName, range, handlePortRangeCallback, &data);
            }
        }

        // Outputs
        for (size_t i = 0; i < vmodule->outputSize(); i++)
        {
            VNOutput *output = vmodule->output(i);
            VNRange *range = output->range();

            for (size_t j = 0; j < output->varSize(); j++)
            {
                std::string outputName = output->var(j)->name();

                std::map<std::string,int>::iterator found = portListDef.find(outputName);
                if (found == portListDef.end())
                {
                    std::cerr << "Port is not in the list: " << outputName << std::endl;
                    delete impl;
                    impl = 0;
                    return;
                }
                int id = found->second;
                portListDef.erase(found);

                PortHandlerData data;
                data.id = id;
                data.type = Port::Output;
                data.list = &portList;
                handlePortWireRange(outputName, range, handlePortRangeCallback, &data);
            }
        }

        // Nets
        for (size_t i = 0; i < vmodule->netSize(); i++)
        {
            VNNet *net = vmodule->net(i);
            VNRange *range = net->range();
            if (net->type() != "wire")
            {
                std::cerr << "Unsupported net type: " << net->type() << std::endl;
                delete impl;
                impl = 0;
                return;
            }
            for (size_t j = 0; j < net->varSize(); j++)
            {
                std::string netName = net->var(j)->name();
                handlePortWireRange(netName, range, handleWireRangeCallback, &module);
            }
        }

        // Setup ports for instances to connect to
        for (size_t i = 0; i < portList.size(); i++)
        {
            for (size_t j = 0; j < portList[i].size(); j++)
            {
                Port &port = portList[i][j];
                module.createPort(port.name(), port.type());
            }
        }

        // Gate instance in the module
        // TODO
        for (size_t i = 0; i < vmodule->gateInstSize(); i++)
        {
            VNGateInst *gInst = vmodule->gateInst(i);
            Gate::GateType type;
            if (gInst->type() == "and")         { type = Gate::AND; }
            else if (gInst->type() == "nand")   { type = Gate::NAND; }
            else if (gInst->type() == "or")     { type = Gate::OR; }
            else if (gInst->type() == "nor")    { type = Gate::NOR; }
            else if (gInst->type() == "xor")    { type = Gate::XOR; }
            else if (gInst->type() == "xnor")   { type = Gate::XNOR; }
            else if (gInst->type() == "buf")    { type = Gate::BUF; }
            else if (gInst->type() == "not")    { type = Gate::AND; }
            else                                { type = Gate::BaseGate; }

            for (size_t j = 0; j < gInst->instSize(); j++)
            {
                VNInstance *inst = gInst->inst(j);
                Gate gate = module.createGate(inst->name(), type);
#ifdef DEBUG
                std::cout << inst->connSize() << std::endl;
                std::cout << inst->name() << "(" << gInst->type() << ")" << std::endl;
#endif
                for (size_t k = 0; k < inst->connSize(); k++)
                {
                    //std::string to = inst->conn(k)->to();
                    std::string from = inst->conn(k)->from();
#ifdef DEBUG
                    std::string spaces(2, ' ');
                    if (k == 0)
                        std::cout << spaces << "Output is connected to '" << from << "'" << std::endl;
                    else
                        std::cout << spaces << "Input " << k - 1 << " is connected to '" << from << "'" << std::endl;
#endif
                    if (k == 0)
                    {
                        // output
                        Wire w = module.wire(from);
                        if (w.isNull())
                        {
                            Port p = module.port(from);
                            if (!p.isNull())
                                gate.connectOutput(k, p);
                            else
                            {
                                // TODO Handle literal like 1'b0, 1'b1
                                std::cerr << "No port/wire can be connected: " << from << std::endl;
                            }
                        }
                        else { gate.connectOutput(k, w); }
                    }
                    else
                    {
                        Wire w = module.wire(from);
                        if (w.isNull())
                        {
                            Port p = module.port(from);
                            if (!p.isNull())
                                gate.connectInput(k-1, p);
                            else
                            {
                                // TODO Handle literal like 1'b0, 1'b1
                                std::cerr << "No port/wire can be connected: " << from << std::endl;
                            }
                        }
                        else { gate.connectInput(k-1, w); }
                    }
                }
            }
        }

        // Module instances in the module
        // TODO
        for (size_t i = 0; i < vmodule->moduleInstSize(); i++)
        {
            VNModuleInst *mInst = vmodule->moduleInst(i);
            for (size_t j = 0; j < mInst->instSize(); j++)
            {
                VNInstance *inst = mInst->inst(j);
                Cell cell = lib.cell(mInst->name());

                if (cell.isNull())
                {
                    std::cerr << "WARNING: Unknown module: " << mInst->name() << std::endl;
                    // Connect modules after modules are all created.
                    // TODO somewhere to store module connectivity
                    continue;
                }

                cell.setName(mInst->inst(0)->name());

                for (size_t k = 0; k < inst->connSize(); k++)
                {
                    std::string to = inst->conn(k)->to();
                    std::string from = inst->conn(k)->from();
                    if (to == "")
                    {
                        std::cout << cell.name() << "(" << cell.type() << ")" << std::endl;
                        std::string spaces(2, ' ');
                        //TODO
                        //
                        if (!cell.input(k).isNull())
                        {
                            Wire w = module.wire(from);
                            if (w.isNull())
                            {
                                Port p = module.port(from);
                                if (!p.isNull())
                                    cell.connectInput(k, p);
                                else
                                {
                                    // TODO Handle literal like 1'b0, 1'b1
                                    std::cerr << "No port/wire can be connected: " << from << std::endl;
                                }
                            }
                            else { cell.connectInput(k, w); }
                            //std::cout << spaces << "Input '" << k << "' is connected to '" << from << "'" << std::endl;
                        }
                        else if (!cell.output(k).isNull())
                        {
                            Wire w = module.wire(from);
                            if (w.isNull())
                            {
                                Port p = module.port(from);
                                if (!p.isNull())
                                    cell.connectOutput(k, p);
                                else
                                {
                                    // TODO Handle literal like 1'b0, 1'b1
                                    std::cerr << "No port/wire can be connected: " << from << std::endl;
                                }
                            }
                            else { cell.connectInput(k, w); }
                            //std::cout << spaces << "Output '" << k << "' is connected to '" << from << "'" << std::endl;
                        }
                        else
                        {
                            std::cout << spaces << "Don't known how to connect '" << k << "' and '" << from << "'" << std::endl;
                        }
                    }
                    else
                    {
                        Wire w = module.wire(from);
                        if (w.isNull())
                        {
                            Port p = module.port(from);
                            if (!p.isNull())
                                cell.connect(to, p);
                            else
                            {
                                // TODO Handle literal like 1'b0, 1'b1
                                std::cerr << "No port/wire can be connected" << std::endl;
                            }
                        }
                        else { cell.connect(to, w); }
                    }
                }
                module.addCell(cell);
            }
        }

        if (!portListDef.empty())
        {
            std::cerr << "Port is not defined: ";
            std::string comma = "";
            for (std::map<std::string,int>::iterator it = portListDef.begin();
                    it != portListDef.end(); ++it)
            {
                std::cerr << comma << (it->first);
                comma = ", ";
            }
            std::cout << std::endl;
            delete impl;
            impl = 0;
            return;
        }
    }
}

Circuit::Circuit(const Circuit &x)
    : Node(x)
{
}

Circuit::Circuit(CircuitPrivate *x)
    : Node(x)
{
}

Circuit& Circuit::operator= (const Circuit &x)
{
    return (Circuit&) Node::operator=(x);
}

size_t Circuit::gateCount() const
{
    if (!impl)
        return 0;
    return IMPL->gateCount();
}

Module Circuit::createModule(const std::string &name)
{
    if (!impl)
        impl = new CircuitPrivate("circuit");
    return IMPL->createModule(name);
}

size_t Circuit::moduleSize() const
{
    if (!impl)
        return 0;
    return IMPL->modules.size();
}

Module Circuit::module(size_t i) const
{
    if (!impl)
        return Module();
    std::string name = IMPL->moduleNames[i];
    return Module(IMPL->modules[name]);
}

Module Circuit::module(const std::string &name) const
{
    if (!impl)
        return Module();
    return Module(IMPL->modules[name]);
}

Module Circuit::topModule() const
{
    if (!impl)
        return Module();
    return Module(IMPL->topModule);
}

void Circuit::setTopModule(Module &module)
{
    if (impl)
        IMPL->setTopModule((ModulePrivate*) module.impl);
}

#undef IMPL

/**************************************************************
 *
 * Node casting functions
 *
 **************************************************************/

Port Node::toPort() const
{
    if (impl && impl->isPort())
        return Port(((PortPrivate*)impl));
    return Port();
}

Wire Node::toWire() const
{
    if (impl && impl->isWire())
        return Wire(((WirePrivate*)impl));
    return Wire();
}

Gate Node::toGate() const
{
    if (impl && impl->isGate())
        return Gate(((GatePrivate*)impl));
    return Gate();
}

Cell Node::toCell() const
{
    if (impl && impl->isCell())
        return Cell(((CellPrivate*)impl));
    return Cell();
}

Module Node::toModule() const
{
    if (impl && impl->isModule())
        return Module(((ModulePrivate*)impl));
    return Module();
}

Circuit Node::toCircuit() const
{
    if (impl && impl->isCircuit())
        return Circuit(((CircuitPrivate*)impl));
    return Circuit();
}

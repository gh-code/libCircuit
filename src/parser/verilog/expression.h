// $Id$
/** \file expression.h Implements an Verilog calculator class group. */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <map>
#include <vector>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <cmath>

class VNPort;
class VNVariable;
class VNInstance;
class VNConnection;
class VNInput;
class VNOutput;
class VNGateInst;
class VNModuleInst;
class VNNet;

class VerilogNode
{
public:
    enum NodeType {
        InputNode   = 1,
        OutputNode  = 2,
        NetNode     = 3,
        GateInstNode = 4,
        ModuleInstNode = 5,
        BaseNode = 11
    };
    virtual ~VerilogNode() {}
    bool isInput() const        { return nodeType() == InputNode; }
    bool isOutput() const       { return nodeType() == OutputNode; }
    bool isNet() const          { return nodeType() == NetNode; }
    bool isGateInst() const     { return nodeType() == GateInstNode; }
    bool isModuleInst() const   { return nodeType() == ModuleInstNode; }
    virtual NodeType nodeType() const { return BaseNode; }
};

class VerilogList
{
public:
    VerilogList() {}
    ~VerilogList()
    {
        for (size_t i = 0; i < nodes.size(); i++)
            if (nodes[i] != NULL)
                delete nodes[i];
        nodes.clear();
    }
    std::vector<VerilogNode*> nodes;
};

class VNModule : public VerilogNode
{
public:
    VNModule(const std::string &name, VerilogList *ports, VerilogList *items)
        : m_name(name), m_ports(ports), m_items(items)
    {
        m_inputs = new VerilogList;
        m_outputs = new VerilogList;
        m_nets = new VerilogList;
        m_gateInsts = new VerilogList;
        m_moduleInsts = new VerilogList;

        for (size_t i = 0; i < items->nodes.size(); i++)
        {
            VerilogNode *item = items->nodes[i];

            switch (item->nodeType())
            {
                case VerilogNode::InputNode:
                    m_inputs->nodes.push_back(item);
                    break;
                case VerilogNode::OutputNode:
                    m_outputs->nodes.push_back(item);
                    break;
                case VerilogNode::NetNode:
                    m_nets->nodes.push_back(item);
                    break;
                case VerilogNode::GateInstNode:
                    m_gateInsts->nodes.push_back(item);
                    break;
                case VerilogNode::ModuleInstNode:
                    m_moduleInsts->nodes.push_back(item);
                    break;
                default:
                    ; // Do nothing
            }
        }
    }
    ~VNModule() {
        delete m_inputs;
        delete m_outputs;
        delete m_nets;
        delete m_gateInsts;
        delete m_moduleInsts;
    }

    std::string name() const { return m_name; }
    size_t portSize() const             { return m_ports->nodes.size(); }
    size_t itemSize() const             { return m_items->nodes.size(); }
    VNPort* port(size_t i) const        { return ((VNPort*)m_ports->nodes[i]); }
    VerilogNode* item(size_t i) const   { return m_items->nodes[i]; }

    size_t inputSize() const            { return m_inputs->nodes.size(); }
    size_t outputSize() const           { return m_outputs->nodes.size(); }
    size_t netSize() const              { return m_nets->nodes.size(); }
    size_t gateInstSize() const         { return m_gateInsts->nodes.size(); }
    size_t moduleInstSize() const       { return m_moduleInsts->nodes.size(); }

    VNInput* input(size_t i) const      { return (VNInput*)(m_inputs->nodes[i]); }
    VNOutput* output(size_t i) const    { return (VNOutput*)(m_outputs->nodes[i]); }
    VNNet* net(size_t i) const          { return (VNNet*)(m_nets->nodes[i]); }
    VNGateInst* gateInst(size_t i) const { return (VNGateInst*)(m_gateInsts->nodes[i]); }
    VNModuleInst* moduleInst(size_t i) const { return (VNModuleInst*)(m_moduleInsts->nodes[i]); }

private:
    std::string m_name;
    VerilogList *m_ports;
    VerilogList *m_items;

    VerilogList *m_inputs;
    VerilogList *m_outputs;
    VerilogList *m_nets;
    VerilogList *m_moduleInsts;
    VerilogList *m_gateInsts;
};

class VNRange : public VerilogNode
{
public:
    VNRange(int left, int right) : m_left(left), m_right(right) {}

    int left() const { return m_left; }
    int right() const { return m_right; }

private:
    int m_left;
    int m_right;
};

class VNPort : public VerilogNode
{
public:
    VNPort(const std::string &name) : m_name(name) {}

    std::string name() const { return m_name; }

private:
    std::string m_name;
};

class VNInput : public VerilogNode
{
public:
    VNInput(VerilogNode *range, VerilogList *vars)
        : m_range(range), m_vars(vars) {}

    VNRange* range() const { return ((VNRange*)m_range); }
    size_t varSize() const { return m_vars->nodes.size(); }
    VNVariable* var(size_t i) const { return ((VNVariable*)(m_vars->nodes[i])); }
    VerilogNode::NodeType nodeType() const { return VerilogNode::InputNode; }

private:
    VerilogNode *m_range;
    VerilogList *m_vars;
};

class VNOutput : public VerilogNode
{
public:
    VNOutput(VerilogNode *range, VerilogList *vars)
        : m_range(range), m_vars(vars) {}

    VNRange* range() const { return ((VNRange*)m_range); }
    size_t varSize() const { return m_vars->nodes.size(); }
    VNVariable* var(size_t i) const { return ((VNVariable*)(m_vars->nodes[i])); }
    VerilogNode::NodeType nodeType() const { return VerilogNode::OutputNode; }

private:
    VerilogNode *m_range;
    VerilogList *m_vars;
};

class VNNet : public VerilogNode
{
public:
    VNNet(const std::string &type, VerilogNode *range, VerilogList *vars)
        : m_type(type), m_range(range), m_vars(vars) {}

    std::string type() const { return m_type; }
    VNRange* range() const { return ((VNRange*)m_range); }
    size_t varSize() const { return m_vars->nodes.size(); }
    VNVariable* var(size_t i) const { return ((VNVariable*)(m_vars->nodes[i])); }
    VerilogNode::NodeType nodeType() const { return VerilogNode::NetNode; }

private:
    std::string m_type;
    VerilogNode *m_range;
    VerilogList *m_vars;
};

class VNVariable : public VerilogNode
{
public:
    VNVariable(const std::string &name) : m_name(name) {}

    std::string name() const { return m_name; }

private:
    std::string m_name;
};

class VNGateInst : public VerilogNode
{
public:
    VNGateInst(const std::string &type, VerilogList *insts)
        : m_type(type), m_insts(insts) {}

    std::string type() const { return m_type; }
    size_t instSize() const { return m_insts->nodes.size(); }
    VNInstance* inst(size_t i) const { return ((VNInstance*)(m_insts->nodes[i])); }
    VerilogNode::NodeType nodeType() const { return VerilogNode::GateInstNode; }

private:
    std::string m_type;
    VerilogList *m_insts;
};

class VNModuleInst : public VerilogNode
{
public:
    VNModuleInst(const std::string &name, VerilogList *insts)
        : m_name(name), m_insts(insts) {}

    std::string name() const { return m_name; }
    size_t instSize() const { return m_insts->nodes.size(); }
    VNInstance* inst(size_t i) const { return ((VNInstance*)(m_insts->nodes[i])); }
    VerilogNode::NodeType nodeType() const { return VerilogNode::ModuleInstNode; }

private:
    std::string m_name;
    VerilogList *m_insts;
};

class VNInstance : public VerilogNode
{
public:
    VNInstance(const std::string &name, VerilogList *conns)
        : m_name(name), m_conns(conns) {}

    std::string name() const { return m_name; }
    size_t connSize() const { return m_conns->nodes.size(); }
    VNConnection* conn(size_t i) const { return ((VNConnection*)(m_conns->nodes[i])); }

private:
    std::string m_name;
    VerilogList *m_conns;
};

class VNInstanceName : public VerilogNode
{
public:
    VNInstanceName(const std::string &name) : m_name(name) {}

    std::string name() const { return m_name; }

private:
    std::string m_name;
};

class VNConnection : public VerilogNode
{
public:
    VNConnection(const std::string &from, const std::string &to) : m_from(from), m_to(to) {}

    std::string from() const { return m_from; }
    std::string to() const { return m_to; }

private:
    std::string m_from;
    std::string m_to;
};

class VerilogContext
{
public:
    std::vector<VerilogNode*> expressions;

    ~VerilogContext()
    {
        clearExpressions();
    }

    void clearExpressions()
    {
	for(unsigned int i = 0; i < expressions.size(); ++i)
	    delete expressions[i];
	expressions.clear();
    }
};

#endif // EXPRESSION_H

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

class LibertyNode;
class LibertyNodeList;
class LNDefine;
class LNValue;
class LNString;
class LNInteger;
class LNDouble;
class LibertyContext;

class LibertyNode
{
public:
    enum NodeType {
        StatementNode   = 1,
        ValueNode       = 2,
        DefineNode      = 3,
        StringNode      = 4,
        IntegerNode     = 5,
        DoubleNode      = 6,
        RangeNode       = 7,
        TimingTableNode = 8,
        TimingNode      = 9,
        PinNode         = 10,
        BaseNode        = 21
    };
    virtual ~LibertyNode() {};
    virtual LibertyNode::NodeType nodeType() const { return LibertyNode::BaseNode; }
};

class LibertyNodeList
{
public:
    ~LibertyNodeList()
    {
        for (size_t i = 0; i < nodes.size(); i++)
            if (nodes[i])
                delete nodes[i];
        nodes.clear();
    }
    std::vector<LibertyNode*> nodes;
};

class LNWireLoad : public LibertyNode
{
public:
    double capacitance;
    double resistance;
    double area;
    double slope;
    std::map<int,double> fanout_length;
};

class LNLuTableTemplate : public LibertyNode
{
public:
    std::string variable_1;
    std::string variable_2;
    std::string index_1;
    std::string index_2;
};

class LNRange : public LibertyNode
{
public:
    LNRange() {}
    LNRange(const std::string &name, double left, double right)
        : m_name(name), m_left(left), m_right(right) { }

    std::string name() const { return m_name; }
    double left() const { return m_left; }
    double right() const { return m_right; }
    LibertyNode::NodeType nodeType() const { return LibertyNode::RangeNode; }

private:
    std::string m_name;
    double m_left;
    double m_right;
};

class LNStatement : public LibertyNode
{
public:
    LNStatement(const std::string &name, LibertyNodeList*, LibertyNodeList*);
    LNStatement(LibertyNode *name, LibertyNodeList*, LibertyNodeList*);
    virtual ~LNStatement()
    {
        if (m_parameterList)
            delete m_parameterList;
        if (m_statementList)
            delete m_statementList;
    }
    std::string name() const { return m_name; }
    LibertyNode::NodeType nodeType() const { return LibertyNode::StatementNode; }
    LNValue* toValue() const { return ((LNValue *)this); }
    LNDefine* toDefine() const { return ((LNDefine *)this); }
    const std::vector<LibertyNode*>& parameters() const { return m_parameterList->nodes; }
    const std::vector<LibertyNode*>& statements() const { return m_statementList->nodes; }
    size_t parameterSize() const { return m_parameterList->nodes.size(); }
    size_t statementSize() const { return m_statementList->nodes.size(); }
    LibertyNode* parameter(size_t i) { return m_parameterList->nodes[i]; }
    LibertyNode* statement(size_t i) { return m_statementList->nodes[i]; }

private:
    std::string m_name;
    LibertyNodeList *m_parameterList;
    LibertyNodeList *m_statementList;
};

class LNDefine : public LNStatement
{
public:
    LNDefine(LibertyNode *a, LibertyNode *b, LibertyNode *c)
        : LNStatement(a, 0, 0), m_b(b), m_c(c)
    {
    }
    ~LNDefine()
    {
        delete m_b;
        delete m_c;
    }
    LibertyNode::NodeType nodeType() const { return LibertyNode::DefineNode; }

private:
    LibertyNode *m_b;
    LibertyNode *m_c;
};

class LNString : public LibertyNode
{
public:
    LNString(const std::string &d) : data(d) {}
    std::string value() const { return data; }
    LibertyNode::NodeType nodeType() const { return LibertyNode::StringNode; }

private:
    std::string data;
};

class LNInteger : public LibertyNode
{
public:
    LNInteger(int d) : data(d) {}
    int value() const { return data; }
    LibertyNode::NodeType nodeType() const { return LibertyNode::IntegerNode; }

private:
    int data;
};

class LNDouble : public LibertyNode
{
public:
    LNDouble(double d) : data(d) {}
    double value() const { return data; }
    LibertyNode::NodeType nodeType() const { return LibertyNode::DoubleNode; }

private:
    double data;
};

class LNValue : public LNStatement
{
public:
    LNValue(const std::string &name, const std::string &value) : LNStatement(name, 0, 0), m_value(new LNString(value)) { }
    LNValue(LibertyNode *name, const std::string &value) : LNStatement(name, 0, 0), m_value(new LNString(value)) { }
    LNValue(const std::string &name, int value)         : LNStatement(name, 0, 0), m_value(new LNInteger(value)) { }
    LNValue(LibertyNode *name, int value)               : LNStatement(name, 0, 0), m_value(new LNInteger(value)) { }
    LNValue(const std::string &name, double value)      : LNStatement(name, 0, 0), m_value(new LNDouble(value)) { }
    LNValue(LibertyNode *name, double value)            : LNStatement(name, 0, 0), m_value(new LNDouble(value)) { }
    LNValue(const std::string &name, LibertyNode *value) : LNStatement(name, 0, 0), m_value(value) { }
    LNValue(LibertyNode *name, LibertyNode *value)      : LNStatement(name, 0, 0), m_value(value) { }
    ~LNValue()
    {
        delete m_value;
    }

    LibertyNode::NodeType nodeType() const { return LibertyNode::ValueNode; }
    LibertyNode* value() const { return m_value; }

private:
    LibertyNode *m_value;
};

class LNTimingTable : public LibertyNode
{
public:
    LNTimingTable(const std::string &name_, const std::string &template_, LibertyNodeList *values)
        : name(name_), lu_table_template(template_)
    {
        const std::vector<LibertyNode*> &nodes = values->nodes;
        for (size_t i = 0; i < nodes.size(); i++)
        {
            if (nodes[i]->nodeType() == LibertyNode::StringNode)
            {
                LNString *str = (LNString*) nodes[i];
                table.push_back(str->value());
            }
            else
            {
                // TODO
                std::cerr << "WARNING: Invalid data in value(...)" << std::endl;
            }
        }
    }
    LibertyNode::NodeType nodeType() const { return LibertyNode::TimingTableNode; }
    std::string name;   // cell_fall, cell_rise, ...
    std::string lu_table_template;
    std::vector<std::string> table;
};

class LNTiming : public LibertyNode
{
public:
    LibertyNode::NodeType nodeType() const { return LibertyNode::TimingNode; }
    std::string related_pin;
    std::string timing_sense;
    std::string timing_type;
    LNTimingTable *cell_fall;
    LNTimingTable *cell_rise;
    LNTimingTable *fall_transition;
    LNTimingTable *rise_transition;
    LNTimingTable *fall_constraint;
    LNTimingTable *rise_constraint;
    std::string when;
    std::string sdf_cond;
};

class LNPin : public LibertyNode
{
public:
    LibertyNode::NodeType nodeType() const { return LibertyNode::PinNode; }
    std::string name;
    std::string direction;
    double capacitance;
    double fall_capacitance;
    double rise_capacitance;
    LNRange fall_capacitance_range;
    LNRange rise_capacitance_range;
    double max_capacitance;
    double max_transition;
    std::string function;
    std::vector<LNTiming*> timings;
};

class LNLeakagePower : public LibertyNode
{
public:
    std::string when;
    float value;
};

class LNCell : public LibertyNode
{
public:
    std::string name;
    double area;
    double cell_leakage_power;
    LNLeakagePower *leakage_power;
    std::vector<LNPin*> pins;
};

class LibertyContext
{
public:
    std::string name;   // NangateOpenCellLibrary
    std::string time_unit;
    std::string leakage_power_unit;
    std::string voltage_unit;
    std::string current_unit;
    std::string pulling_resistance_unit;
    std::string capacitive_load_unit;
    double nom_process;
    double nom_temperature;
    double nom_voltage;
    std::string default_wire_load;
    std::map<std::string,LNWireLoad> wire_loads;
    std::map<std::string,LNLuTableTemplate> lu_table_templates;
    std::map<std::string,LNCell> cells;
    std::vector<LibertyNode*> expressions;

    ~LibertyContext()
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

inline LNStatement::LNStatement(const std::string &name, LibertyNodeList *plist, LibertyNodeList *slist)
    : m_name(name), m_parameterList(plist), m_statementList(slist)
{
}

inline LNStatement::LNStatement(LibertyNode *name, LibertyNodeList *plist, LibertyNodeList *slist)
    : m_name(), m_parameterList(plist), m_statementList(slist)
{
    m_name = ((LNString *)name)->value();
    delete name;
}

#endif // EXPRESSION_H
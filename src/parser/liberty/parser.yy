/* $Id$ -*- mode: c++ -*- */
/** \file parser.yy Contains the Liberty Bison parser source */
/**
 * Bison for Liberty
 *
 * Author: Gary Huang<gh.nctu+code@gmail.com>
 *
 *   2016
 *      First release
 */

%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "expression.h"

inline std::string trim_quote(const std::string&);

%}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "start" */
%start source_text

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="Liberty"

/* set the parser's class identifier */
%define "parser_class_name" "Parser"

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class Driver& driver }

/* verbose error messages */
%error-verbose

 /*** BEGIN EXAMPLE - Change the Liberty grammar's tokens below ***/

%union {
    int                         ival;
    double                      fval;
    std::string*                sval;
    class LibertyNode*          node;
    class LibertyNodeList*      list;
}

%token END 0 "end of file"
%token LIBRARY
%token DEFINE
%token TIME_UNIT
%token LEAKAGE_POWER_UNIT
%token VOLTAGE_UNIT
%token CURRENT_UNIT
%token PULLING_RESISTANCE_UNIT
%token CAPACITIVE_LOAD_UNIT
%token NOM_PROCESS
%token NOM_TEMPERATURE
%token NOM_VOLTAGE
%token LU_TABLE_TEMPLATE
%token VARIABLE_1
%token VARIABLE_2
%token INDEX_1
%token INDEX_2
%token CELL
%token AREA
%token CELL_LEAKAGE_POWER
%token LEAKAGE_POWER
%token PIN
%token DIRECTION
%token FUNCTION
%token MAX_CAPACITANCE
%token MAX_TRANSITION
%token TIMING
%token RELATED_PIN
%token TIMING_SENSE
%token TIMING_TYPE
%token CELL_FALL
%token CELL_RISE
%token FALL_TRANSITION
%token RISE_TRANSITION
%token FALL_CONSTRAINT
%token RISE_CONSTRAINT
%token WHEN
%token SDF_COND
%token CAPACITANCE
%token FALL_CAPACITANCE
%token RISE_CAPACITANCE
%token FALL_CAPACITANCE_RANGE
%token RISE_CAPACITANCE_RANGE
%token VALUES

%token<sval> VERSION
%token<sval> IDENTIFIER
%token<sval> STRING
%token<sval> INTEGER
%token<sval> FLOAT

%type<sval> string
%type<list> top_statement_list
%type<node> top_statement
%type<list> lu_table_template_statement_list
%type<node> lu_table_template_statement
%type<list> statement_list
%type<node> statement
%type<list> cell_statement_list
%type<node> cell_statement
%type<list> pin_statement_list
%type<node> pin_statement
%type<list> timing_statement_list
%type<node> timing_statement
%type<list> parameter_list
%type<node> parameter
%type<node> keyword
%type<node> value
%type<node> identifier


// %destructor { delete $$; } STRING IDENTIFIER
// %destructor { delete $$; } name_of_module

 /*** END EXAMPLE - Change the Liberty grammar's tokens above ***/

%{

#include "driver.h"
#include "scanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN EXAMPLE - Change the Liberty grammar rules below ***/

source_text
        : LIBRARY '(' identifier ')' '{' top_statement_list '}'
        {
            driver.liberty.name = ((LNString*)$3)->value(); delete $3;
            for (size_t i =  0; i < $6->nodes.size(); i++)
            {
                LibertyNode *node = $6->nodes[i];
                driver.liberty.expressions.push_back(node);
            }
            $6->nodes.clear();
            delete $6;
        }
        ;

top_statement_list
        : top_statement                         { $$ = new LibertyNodeList; if ($1) $$->nodes.push_back($1); }
        | top_statement_list top_statement      { if ($2) $1->nodes.push_back($2); }
        ;

statement_list
        : statement                             { $$ = new LibertyNodeList; if ($1) $$->nodes.push_back($1); }
        | statement_list statement              { if ($2) $1->nodes.push_back($2); }
        ;

top_statement
        : TIME_UNIT ':' string ';'              { driver.liberty.time_unit = *$3;           $$ = 0; delete $3; }
        | LEAKAGE_POWER_UNIT ':' string ';'     { driver.liberty.leakage_power_unit = *$3;  $$ = 0; delete $3; }
        | VOLTAGE_UNIT ':' string ';'           { driver.liberty.voltage_unit = *$3;        $$ = 0; delete $3; }
        | CURRENT_UNIT ':' string ';'           { driver.liberty.current_unit = *$3;        $$ = 0; delete $3; }
        | PULLING_RESISTANCE_UNIT ':' string ';' { driver.liberty.pulling_resistance_unit = *$3; $$ = 0; delete $3; }
        | CAPACITIVE_LOAD_UNIT '(' INTEGER ',' identifier ')' ';'
        {
            driver.liberty.capacitive_load_unit = (*$3) + ((LNString*)$5)->value();
            $$ = 0;
            delete $3;
            delete $5;
        }
        | NOM_PROCESS ':' FLOAT ';'             { driver.liberty.nom_process     = atof($3->c_str()); $$ = 0; delete $3; }
        | NOM_TEMPERATURE ':' FLOAT ';'         { driver.liberty.nom_temperature = atof($3->c_str()); $$ = 0; delete $3; }
        | NOM_VOLTAGE ':' FLOAT ';'             { driver.liberty.nom_voltage     = atof($3->c_str()); $$ = 0; delete $3; }
        | LU_TABLE_TEMPLATE '(' identifier ')' '{' lu_table_template_statement_list '}'
        {
            const std::vector<LibertyNode*>& nodes = $6->nodes;
            LNLuTableTemplate lu_table_template;
            for (size_t i = 0; i < nodes.size(); i++)
            {
                LNValue *val = (LNValue*) nodes[i];
                if (val->name() == "variable_1") {
                    LNString *str = (LNString*)(val->value());
                    lu_table_template.variable_1 = str->value();
                } else if (val->name() == "variable_2") {
                    LNString *str = (LNString*)(val->value());
                    lu_table_template.variable_2 = str->value();
                } else if (val->name() == "index_1") {
                    LNString *str = (LNString*)(val->value());
                    lu_table_template.index_1 = str->value();
                } else if (val->name() == "index_2") {
                    LNString *str = (LNString*)(val->value());
                    lu_table_template.index_2 = str->value();
                } else {
                    std::cerr << "What the hell?" << std::endl;
                }
            }
            std::string name = ((LNString*)$3)->value();
            driver.liberty.lu_table_templates[name] = lu_table_template;
            $$ = 0;
            delete $3;
            delete $6;
        }
        | CELL '(' identifier ')' '{' cell_statement_list '}'
        {
            LNCell cell;
            std::vector<LibertyNode*>::iterator it = $6->nodes.begin();
            while (it != $6->nodes.end())
            {
                LibertyNode *node = *it;
                switch (node->nodeType())
                {
                    case LibertyNode::ValueNode:
                    {
                        LNValue *val = (LNValue*) node;
                        if (val->name() == "area") {
                            LNDouble *f = (LNDouble*)(val->value());
                            cell.area = f->value();
                            delete node;
                            it = $6->nodes.erase(it);
                        } else if (val->name() == "cell_leakage_power") {
                            LNDouble *f = (LNDouble*)(val->value());
                            cell.cell_leakage_power = f->value();
                            delete node;
                            it = $6->nodes.erase(it);
                        } else {
                            ++it;
                        }
                    }
                    break;
                    case LibertyNode::StatementNode:
                    {
                        LNStatement *s = (LNStatement*) node;
                        for (size_t i = 0; i < s->statementSize(); i++)
                        {
                            if (s->statement(i)->nodeType() == LibertyNode::PinNode)
                            {
                                LNPin *pin = (LNPin*) s->statement(i);
                                cell.pins.push_back(pin);
                            }
                        }
                        it = $6->nodes.erase(it);
                    }
                    break;
                    default:
                        ++it;
                }
            }
            std::string name = ((LNString*)$3)->value();
            cell.name = name;
            driver.liberty.cells[name] = cell;

            LibertyNodeList *paramList = new LibertyNodeList;
            paramList->nodes.push_back($3);
            $$ = new LNStatement("cell", paramList, $6);
        }
        | statement                             { $$ = $1; }
        ;

lu_table_template_statement_list
        : lu_table_template_statement           { $$ = new LibertyNodeList; if ($1) $$->nodes.push_back($1); }
        | lu_table_template_statement_list lu_table_template_statement { if ($2) $1->nodes.push_back($2); }
        ;

lu_table_template_statement
        : VARIABLE_1 ':' identifier ';'         { $$ = new LNValue("variable_1", $3); }
        | VARIABLE_2 ':' identifier ';'         { $$ = new LNValue("variable_2", $3); }
        | INDEX_1 '(' string ')'                { $$ = new LNValue("index_1", new LNString(*$3)); delete $3; }
        | INDEX_2 '(' string ')'                { $$ = new LNValue("index_2", new LNString(*$3)); delete $3; }
        ;

cell_statement_list
        : cell_statement                        { $$ = new LibertyNodeList; if ($1) $$->nodes.push_back($1); }
        | cell_statement_list cell_statement    { if ($2) $1->nodes.push_back($2); }
        ;

cell_statement
        : AREA ':' FLOAT ';'                    { $$ = new LNValue("area", new LNDouble(atof($3->c_str()))); delete $3; }
        | CELL_LEAKAGE_POWER ':' FLOAT ';'      { $$ = new LNValue("cell_leakage_power", new LNDouble(atof($3->c_str()))); delete $3; }
        | LEAKAGE_POWER '(' ')' '{' statement_list '}'  { $$ = 0; delete $5; }
        | PIN '(' identifier ')' '{' pin_statement_list '}'
        {
            LNPin *pin = new LNPin;
            pin->name = ((LNString*)$3)->value();
            std::vector<LibertyNode*>::iterator it = $6->nodes.begin();
            while (it != $6->nodes.end())
            {
                LibertyNode *node = *it;
                switch (node->nodeType())
                {
                    case LibertyNode::ValueNode:
                    {
                        LNValue *val = (LNValue*) node;
                        if (val->name() == "direction") {
                            LNString *str = (LNString*)(val->value());
                            pin->direction = str->value();
                            delete node;
                            it = $6->nodes.erase(it);
                        } else if (val->name() == "capacitance") {
                            LNDouble *f = (LNDouble*)(val->value());
                            pin->capacitance = f->value();
                            delete node;
                            it = $6->nodes.erase(it);
                        } else if (val->name() == "fall_capacitance") {
                            LNDouble *f = (LNDouble*)(val->value());
                            pin->fall_capacitance = f->value();
                            delete node;
                            it = $6->nodes.erase(it);
                        } else if (val->name() == "rise_capacitance") {
                            LNDouble *f = (LNDouble*)(val->value());
                            pin->rise_capacitance = f->value();
                            delete node;
                            it = $6->nodes.erase(it);
                        } else if (val->name() == "max_capacitance") {
                            LNDouble *f = (LNDouble*)(val->value());
                            pin->max_capacitance = f->value();
                            delete node;
                            it = $6->nodes.erase(it);
                        } else if (val->name() == "max_transition") {
                            LNDouble *f = (LNDouble*)(val->value());
                            pin->max_transition = f->value();
                            delete node;
                            it = $6->nodes.erase(it);
                        } else if (val->name() == "function") {
                            LNString *str = (LNString*)(val->value());
                            pin->function = str->value();
                            delete node;
                            it = $6->nodes.erase(it);
                        } else {
                            ++it;
                        }
                    }
                    break;
                    case LibertyNode::RangeNode:
                    {
                        LNRange *range = (LNRange*)node;
                        if (range->name() == "fall_capacitance_range") {
                            pin->fall_capacitance_range = (*range);
                            it = $6->nodes.erase(it);
                            delete node;
                        } else if (range->name() == "rise_capacitance_range") {
                            pin->rise_capacitance_range = (*range);
                            it = $6->nodes.erase(it);
                            delete node;
                        } else {
                            ++it;
                        }
                    }
                    break;
                    case LibertyNode::StatementNode:
                    {
                        LNStatement *s = (LNStatement*) node;
                        for (size_t i = 0; i < s->statementSize(); i++)
                        {
                            if (s->statement(i)->nodeType() == LibertyNode::TimingNode)
                            {
                                // don't delete node
                                LNTiming *timing = (LNTiming*) s->statement(i);
                                pin->timings.push_back(timing);
                            }
                        }
                        it = $6->nodes.erase(it);
                    }
                    break;
                    default:
                        ++it;
                }
            }
            $6->nodes.push_back(pin);
            LibertyNodeList *paramList = new LibertyNodeList;
            paramList->nodes.push_back($3);
            $$ = new LNStatement("pin", paramList, $6);
        }
        | statement
        {
            $$ = $1;
            /* clock_gating_integrated_cell, statetable, ... */
        }
        ;

pin_statement_list
        : pin_statement                         { $$ = new LibertyNodeList; if ($1) $$->nodes.push_back($1); }
        | pin_statement_list pin_statement      { if ($2) $1->nodes.push_back($2); }
        ;

pin_statement
        : DIRECTION ':' identifier ';'          { $$ = new LNValue("direction", $3); }
        | FUNCTION ':' string ';'               { $$ = new LNValue("function", *$3); delete $3; }
        | CAPACITANCE ':' FLOAT ';'             { $$ = new LNValue("capacitance", atof($3->c_str())); delete $3; }
        | FALL_CAPACITANCE ':' FLOAT ';'        { $$ = new LNValue("fall_capacitance", atof($3->c_str())); delete $3; }
        | RISE_CAPACITANCE ':' FLOAT ';'        { $$ = new LNValue("rise_capacitance", atof($3->c_str())); delete $3; }
        | FALL_CAPACITANCE_RANGE '(' FLOAT ',' FLOAT ')' ';' { $$ = new LNRange("fall_capacitance_range", atof($3->c_str()), atof($5->c_str())); delete $3; delete $5; }
        | RISE_CAPACITANCE_RANGE '(' FLOAT ',' FLOAT ')' ';' { $$ = new LNRange("rise_capacitance_range", atof($3->c_str()), atof($5->c_str())); delete $3; delete $5; }
        | MAX_CAPACITANCE ':' FLOAT ';'         { $$ = new LNValue("max_capacitance", atof($3->c_str())); delete $3; }
        | MAX_TRANSITION ':' FLOAT ';'          { $$ = new LNValue("max_transition", atof($3->c_str())); delete $3; }
        | TIMING '(' ')' '{' timing_statement_list '}'
        {
            std::vector<LibertyNode*>::iterator it = $5->nodes.begin();
            LNTiming *timing = new LNTiming;
            timing->cell_fall = 0;
            timing->cell_rise = 0;
            timing->fall_transition = 0;
            timing->rise_transition = 0;
            timing->fall_constraint = 0;
            timing->rise_constraint = 0;
            while (it != $5->nodes.end())
            {
                LibertyNode *node = *it;
                switch (node->nodeType())
                {
                    case LibertyNode::TimingTableNode:
                    {
                        LNTimingTable *table = (LNTimingTable*) node;
                        if (table->name == "cell_fall") {
                            timing->cell_fall = table;
                        } else if (table->name == "cell_rise") {
                            timing->cell_rise = table;
                        } else if (table->name == "fall_transition") {
                            timing->fall_transition = table;
                        } else if (table->name == "rise_transition") {
                            timing->rise_transition = table;
                        } else if (table->name == "fall_constraint") {
                            timing->fall_constraint = table;
                        } else if (table->name == "rise_constraint") {
                            timing->rise_constraint = table;
                        } else {
                            std::cerr << "Weird table" << std::endl;
                        }
                        it = $5->nodes.erase(it);
                    }
                    break;
                    case LibertyNode::ValueNode:
                    {
                        LNValue *val = (LNValue*) node;
                        if (val->name() == "related_pin") {
                            LNString *s = (LNString *)(val->value());
                            timing->related_pin = s->value();
                            it = $5->nodes.erase(it);
                        } else if (val->name() == "timing_sense") {
                            LNString *s = (LNString *)(val->value());
                            timing->timing_sense = s->value();
                            it = $5->nodes.erase(it);
                        } else if (val->name() == "timing_type") {
                            LNString *s = (LNString *)(val->value());
                            timing->timing_type = s->value();
                            it = $5->nodes.erase(it);
                        } else if (val->name() == "when") {
                            LNString *s = (LNString *)(val->value());
                            timing->when = s->value();
                            it = $5->nodes.erase(it);
                        } else if (val->name() == "sdf_cond") {
                            LNString *s = (LNString *)(val->value());
                            timing->sdf_cond = s->value();
                            it = $5->nodes.erase(it);
                        } else {
                            ++it;
                        }
                    }
                    break;
                    default:
                        ++it;
                }
            }
            $5->nodes.push_back(timing);
            $$ = new LNStatement("timing", 0, $5);
        }
        | statement
        {
            $$ = $1;
            /* ignore clock_gate_test_pin, internal_power, ... */
        }
        ;

timing_statement_list
        : timing_statement                      { $$ = new LibertyNodeList; if ($1) $$->nodes.push_back($1); }
        | timing_statement_list timing_statement { if ($2) $1->nodes.push_back($2); }
        ;

timing_statement
        : RELATED_PIN ':' string ';'            { $$ = new LNValue("related_pin", *$3); delete $3; }
        | TIMING_SENSE ':' identifier ';'       { $$ = new LNValue("timing_sense", $3); }
        | TIMING_TYPE ':' identifier ';'       { $$ = new LNValue("timing_type", $3); }
        | CELL_FALL '(' identifier ')' '{' VALUES '(' parameter_list ')' ';' '}' { $$ = new LNTimingTable("cell_fall", ((LNString*)$3)->value(), $8); }
        | CELL_RISE '(' identifier ')' '{' VALUES '(' parameter_list ')' ';' '}' { $$ = new LNTimingTable("cell_rise", ((LNString*)$3)->value(), $8); }
        | FALL_TRANSITION '(' identifier ')' '{' VALUES '(' parameter_list ')' ';' '}' { $$ = new LNTimingTable("fall_transition", ((LNString*)$3)->value(), $8); }
        | RISE_TRANSITION '(' identifier ')' '{' VALUES '(' parameter_list ')' ';' '}' { $$ = new LNTimingTable("rise_transition", ((LNString*)$3)->value(), $8); }
        | FALL_CONSTRAINT '(' identifier ')' '{' VALUES '(' parameter_list ')' ';' '}' { $$ = new LNTimingTable("fall_constraint", ((LNString*)$3)->value(), $8); }
        | RISE_CONSTRAINT '(' identifier ')' '{' VALUES '(' parameter_list ')' ';' '}' { $$ = new LNTimingTable("rise_constraint", ((LNString*)$3)->value(), $8); }
        | WHEN ':' string ';'                   { $$ = new LNValue("when", *$3); delete $3; }
        | SDF_COND ':' string ';'               { $$ = new LNValue("sdf_cond", *$3); delete $3; }
        | statement
        {
            $$ = $1;
            /* ignore CLKGATETST timing definition */
        }
        ;

statement
        : identifier ':' value ';'              { $$ = new LNValue($1, $3); }
        | keyword ':' value ';'                 { $$ = new LNValue($1, $3); }
        | identifier '(' parameter_list ')'     { $$ = new LNStatement($1, $3, 0); }
        | keyword '(' parameter_list ')'        { $$ = new LNStatement($1, $3, 0); }
        | identifier '(' parameter_list ')' ';' { $$ = new LNStatement($1, $3, 0); }
        | keyword '(' parameter_list ')' ';'    { $$ = new LNStatement($1, $3, 0); }
        | identifier '(' ')' '{' statement_list '}'                 { $$ = new LNStatement($1, 0, $5); }
        | identifier '(' parameter_list ')' '{' statement_list '}'  { $$ = new LNStatement($1, $3, $6); }
        | DEFINE '(' identifier ',' identifier ',' identifier ')' ';' { $$ = new LNDefine($3, $5, $7); }
        | DEFINE '(' identifier ',' keyword ',' identifier ')' ';'  { $$ = new LNDefine($3, $5, $7); }
        ;

parameter_list
        : parameter                     { $$ = new LibertyNodeList; if ($1) $$->nodes.push_back($1); }
        | parameter_list ',' parameter  { if ($3) $1->nodes.push_back($3); }
        ;

parameter
        : value                         { $$ = (LNValue*)$1; }
        ;

value
        : identifier                    { $$ = $1; }
        | INTEGER                       { $$ = new LNInteger(atoi($1->c_str())); delete $1; }
        | FLOAT                         { $$ = new LNDouble(atof($1->c_str())); delete $1; }
        ;

string
        : STRING                        { (*$1) = trim_quote(*$1); $$ = $1; }
        ;

identifier
        : IDENTIFIER                    { $$ = new LNString(*$1); delete $1; }
        | string                        { $$ = new LNString(*$1); delete $1; }
        | VERSION                       { $$ = new LNString(*$1); delete $1; }
        ;

keyword
        : VARIABLE_1                    { $$ = new LNString("variable_1"); }
        | VARIABLE_2                    { $$ = new LNString("variable_2"); }
        | INDEX_1                       { $$ = new LNString("index_1"); }
        | INDEX_2                       { $$ = new LNString("index_2"); }
        | CELL                          { $$ = new LNString("cell"); }
        | AREA                          { $$ = new LNString("area"); }
        | WHEN                          { $$ = new LNString("when"); }
        | RELATED_PIN                   { $$ = new LNString("related_pin"); }
        | CAPACITANCE                   { $$ = new LNString("capacitance"); }
        | VALUES                        { $$ = new LNString("values"); }
        ;

 /*** END EXAMPLE - Change the Liberty grammar rules above ***/

%% /*** Additional Code ***/

void Liberty::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}

inline std::string trim_quote(const std::string &s)
{
    size_t i = 0;
    size_t j = s.size() - 1;
    while (s[i] == ' ' || s[i] == '"') { i++; }
    while (s[j] == ' ' || s[j] == '"') { j--; }
    return s.substr(i, j + 1 - i);
}
/* $Id$ -*- mode: c++ -*- */
/** \file parser.yy Contains the Verilog Bison parser source */
/**
 * Bison for Verilog
 *
 * Author: Gary Huang<gh.nctu+code@gmail.com>
 *
 * ChangeLog:
 *   2017/02/22
 *      Reduce support only synthesized circuit
 *   2016
 *      First release
 */

%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>

#include "expression.h"

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
%name-prefix="Verilog"

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

 /*** BEGIN EXAMPLE - Change the Verilog grammar's tokens below ***/

%union {
    int                         ival;
    double                      fval;
    std::string*                sval;
    class VerilogNode*          vnode;
    class VerilogList*          vlist;
}

%token END 0 "end of file"
%token INPUT OUTPUT INOUT
%token ASSIGN
%token SPECIFY
%token ENDSPECIFY
%token PARAMETER
%token INTEGER REAL
%token IF ELSE
%token VBEGIN VEND
%token VINITIAL
%token ALWAYS
%token REPEAT
%token POSEDGE NEGEDGE
%token MODULE ENDMODULE
%token TASK ENDTASK
%token FUNCTION ENDFUNCTION
%token SCALARED
%token VECTORED
%token NON_BLOCK_ASSIGN
%token SPECIFY_ASSIGN
%token<sval> GATETYPE
%token<sval> UNSIGNED_NUMBER
%token<sval> FLOAT_NUMBER
%token<sval> IDENTIFIER
%token<sval> STRING
%token<sval> NETTYPE
%token<sval> REG
%token<sval> SYSTEM_IDENTIFIER
%token<sval> BASE
%token<sval> STRENGTH0 STRENGTH1

%type<vlist> module_item_list
%type<vlist> gate_instance_list
%type<vlist> module_instance_list
%type<vlist> terminal_list
%type<vlist> name_of_variable_list
%type<vnode> gate_declaration
%type<vnode> net_declaration
%type<vlist> list_of_ports
%type<vlist> port_list
%type<vnode> module
%type<vnode> module_item
%type<vnode> port
%type<sval> identifier
%type<vnode> name_of_variable
%type<sval> name_of_module
%type<vnode> input_declaration
%type<vnode> output_declaration
%type<vnode> port_reference
%type<vnode> port_expression
%type<vlist> list_of_variables
%type<sval> DECIMAL_NUMBER
%type<sval> name_of_gate_instance
%type<vnode> gate_instance
%type<vnode> module_instance
%type<vnode> terminal
%type<sval> name_of_instance
%type<sval> number
%type<vnode> range
%type<sval> primary
%type<vlist> expression_list
%type<sval> expression
%type<vnode> module_port_connection
%type<vlist> module_port_connection_list
%type<vnode> named_port_connection
%type<vlist> named_port_connection_list
%type<vlist> list_of_module_connections
%type<vnode> module_instantiation

%destructor { delete $$; } STRING IDENTIFIER
%destructor { delete $$; } name_of_module

 /*** END EXAMPLE - Change the Verilog grammar's tokens above ***/

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

 /*** BEGIN EXAMPLE - Change the Verilog grammar rules below ***/

//
// 1. Source Text
//
source_text
        :                               { /* an empty document */ }
        | description_list              { /* no implementation */ }
        ;

description
        : module                        { driver.verilog.expressions.push_back($1);  }
        ;

description_list
        : description
        | description_list description
        ;

module
        : MODULE name_of_module ';' ENDMODULE                   { $$ = new VNModule(*$2, 0, 0); delete($2); }
        | MODULE name_of_module ';' module_item_list ENDMODULE  { $$ = new VNModule(*$2, 0, $4); delete($2); }
        | MODULE name_of_module list_of_ports ';' module_item_list ENDMODULE { $$ = new VNModule(*$2, $3, $5); delete $2; }
        ;

port
        :                               { $$ = 0; }
        | port_expression               { $$ = new VNPort(((VNVariable*)$1)->name()); delete $1; }
        /*
        | '.' name_of_port '(' ')'      { $$ = NULL; }
        | '.' name_of_port '(' port_expression ')' { $$ = NULL; }
        */
        ;

port_list
        : port                          { $$ = new VerilogList; if ($1) $$->nodes.push_back($1); }
        | port_list ',' port            { if ($3) $1->nodes.push_back($3); }
        ;

list_of_ports
        : '(' port_list ')'             { $$ = $2; }
        ;

port_expression
        : port_reference                { $$ = $1; }
        | '{' port_reference_list '}'   { error(@$, std::string("Unimplemented rule for port_reference_list")); YYERROR; }
        ;

port_reference
        : name_of_variable              { $$ = $1; }
        | name_of_variable '[' expression ']' { error(@$, std::string("Unimplemented rule for port_reference")); YYERROR; }
        | name_of_variable '[' expression ':' expression ']' { error(@$, std::string("Unimplemented rule for port_reference")); YYERROR; }
        ;

port_reference_list
        : port_reference                { /* no implementation */ }
        | port_reference_list ',' port_reference { /* no implementation */  }
        ;

/*
name_of_port
        : IDENTIFIER                    { $$ = $1; }
        ;
*/

module_item
        : input_declaration             { $$ = $1; }
        | output_declaration            { $$ = $1; }
        /*
        | inout_declaration             { $$ = $1; }
        */
        | net_declaration               { $$ = $1; }
        | gate_declaration              { $$ = $1; }
        | module_instantiation          { $$ = $1; }
        ;

module_item_list
        : module_item                   { $$ = new VerilogList; $$->nodes.push_back($1); }
        | module_item_list module_item  { $1->nodes.push_back($2); }
        ;

//
// 2. Declarations
//

input_declaration
        : INPUT list_of_variables ';'   { $$ = new VNInput(0, $2); }
        | INPUT range list_of_variables ';' { $$ = new VNInput($2, $3); }
        ;

output_declaration
        : OUTPUT list_of_variables ';'  { $$ = new VNOutput(0, $2); }
        | OUTPUT range list_of_variables ';' { $$ = new VNOutput($2, $3); }
        ;

/*
inout_declaration
        : INOUT list_of_variables ';'
        | INOUT range list_of_variables ';'
        ;
*/

net_declaration
        : NETTYPE list_of_variables ';' { $$ = new VNNet(*$1, 0, $2); delete $1; }
        | NETTYPE range list_of_variables ';' { $$ = new VNNet(*$1, $2, $3); delete $1; }
        ;

list_of_variables
        : name_of_variable_list         { $$ = $1; }
        ;

name_of_variable
        : IDENTIFIER                    { $$ = new VNVariable(*$1); delete $1; }
        ;

name_of_variable_list
        : name_of_variable              { $$ = new VerilogList; $$->nodes.push_back($1); }
        | name_of_variable_list ',' name_of_variable { $$->nodes.push_back($3); }
        ;

range
        : '[' expression ':' expression ']'
        {
            $$ = new VNRange(atoi($2->c_str()), atoi($4->c_str()));
            delete $2; delete $4;
        }
        ;

//
// 3. Primitive Instances
//
gate_declaration
        : GATETYPE gate_instance_list ';' { $$ = new VNGateInst(*$1, $2); delete $1; }
        ;

gate_instance
        : '(' terminal_list ')'         { $$ = new VNInstance("", $2); }
        | name_of_gate_instance '(' terminal_list ')' { $$ = new VNInstance(*$1, $3); delete $1; }
        ;

gate_instance_list
        : gate_instance                 { $$ = new VerilogList; $$->nodes.push_back($1); }
        | gate_instance_list ',' gate_instance { $1->nodes.push_back($3); }
        ;

name_of_gate_instance
        : IDENTIFIER                    { $$ = $1; }
        | IDENTIFIER range              { error(@$, std::string("Unsupported range with gate instance")); YYERROR; delete $1; delete $2; }
        ;

terminal
        : expression                    { $$ = new VNConnection(*$1, ""); delete $1; }
        ;

terminal_list
        : terminal                      { $$ = new VerilogList; $$->nodes.push_back($1); }
        | terminal_list ',' terminal    { $1->nodes.push_back($3); }
        ;

//
// 4. Module Instantiations
//
module_instantiation
        : name_of_module module_instance_list ';' { $$ = new VNModuleInst(*$1, $2); delete $1; }
        | name_of_module parameter_value_assignment module_instance_list ';'
        {
            error(@$, std::string("Unimplemented rule")); YYERROR;
            delete $1;
        }
        ;

name_of_module
        : IDENTIFIER                    { $$ = $1; }
        ;

parameter_value_assignment
        : '#' '(' expression_list ')'   { /* no implementation */ }

module_instance
        : name_of_instance '(' ')'      { $$ = new VNInstance(*$1, 0); delete $1; }
        | name_of_instance '(' list_of_module_connections ')' { $$ = new VNInstance(*$1, $3); delete $1; }
        ;

module_instance_list
        : module_instance               { $$ = new VerilogList; $$->nodes.push_back($1); }
        | module_instance_list ',' module_instance { $1->nodes.push_back($3); }
        ;

name_of_instance
        : IDENTIFIER                    { $$ = $1; }
        | IDENTIFIER range              { error(@$, std::string("Range in instance name is unsupported")); YYERROR; delete $1; delete $2; }
        ;

list_of_module_connections
        : module_port_connection_list   { $$ = $1; }
        | named_port_connection_list    { $$ = $1; }
        ;

module_port_connection
        :                               { $$ = new VNConnection("", ""); }
        | expression                    { $$ = new VNConnection(*$1, ""); delete $1; }
        ;

named_port_connection
        : '.' IDENTIFIER '(' identifier ')' { $$ = new VNConnection(*$4, *$2); delete $2; delete $4; }
        | '.' IDENTIFIER '(' expression ')' { $$ = new VNConnection(*$4, *$2); delete $2; delete $4; }
        ;

module_port_connection_list
        : module_port_connection        { $$ = new VerilogList; if ($1) $$->nodes.push_back($1); }
        | module_port_connection_list ',' module_port_connection { if ($3) $1->nodes.push_back($3); }
        ;

named_port_connection_list
        : named_port_connection         { $$ = new VerilogList; $$->nodes.push_back($1); }
        | named_port_connection_list ',' named_port_connection { $1->nodes.push_back($3); }
        ;

//
// 5. Behavioral Statements
//

//
// 7. Expressions
//

mintypmax_expression
        : expression                    { /* no implementation */ }
        | expression ':' expression ':' expression { /* no implementation */ }
        ;

expression
        : primary                       { $$ = $1; }
        | STRING                        { $$ = $1; }
        ;

expression_list
        : expression                    { $$ = new VerilogList; $$->nodes.push_back(new VNVariable(*$1)); delete $1; }
        | expression_list ',' expression { $1->nodes.push_back(new VNVariable(*$3)); delete $3; }
        ;

primary
        : number                        { $$ = $1; }
        | identifier                    { $$ = $1; }
        | identifier '[' expression ']' { $$ = $1; *$$ += "["+(*$3)+"]"; delete $3; }
        | identifier '[' expression ':' expression ']' { error(@$, std::string("Unimplemented rule")); YYERROR; }
        | concatenation                 { error(@$, std::string("Unimplemented rule")); YYERROR; }
        | multiple_concatenation        { error(@$, std::string("Unimplemented rule")); YYERROR; }
        | '(' mintypmax_expression ')'  { error(@$, std::string("Unimplemented rule")); YYERROR; }
        ;

number
        : DECIMAL_NUMBER                { $$ = $1;  }
        | BASE UNSIGNED_NUMBER          { $$ = $1; *$$ += *$2; }
        | UNSIGNED_NUMBER BASE UNSIGNED_NUMBER { $$ = $1; *$$ += *$2 + *$3; }
        | FLOAT_NUMBER                  { $$ = $1; }
        ;

DECIMAL_NUMBER
        : UNSIGNED_NUMBER               { $$ = $1; }
        | '+' UNSIGNED_NUMBER           { $$ = $2; }
        | '-' UNSIGNED_NUMBER           { $$ = $2; *$$ = "-" + *$2; }
        ;

concatenation
        : '{' expression_list '}'       { /* no implementation */ }
        ;

multiple_concatenation
        : '{' expression '{' expression_list '}' '}' { /* no implementation */ }
        ;

//
// 8. General
//

identifier
        : IDENTIFIER                    { $$ = $1; }
        | identifier '.' IDENTIFIER     { *$$ += "." + *$3; }
        ;

 /*** END EXAMPLE - Change the Verilog grammar rules above ***/

%% /*** Additional Code ***/

void Verilog::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}

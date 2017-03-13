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
}

%token END 0 "end of file"
%token LIBRARY
%token DEFINE
%token<sval> VERSION
%token<sval> IDENTIFIER
%token<sval> STRING
%token<sval> INTEGER
%token<sval> FLOAT

%type<node> parameter
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
        : LIBRARY '(' identifier ')' '{' statement_list '}'  { /*driver.liberty.expression.push_back($1);*/ }
        ;

statement_list
        : statement                     { }
        | statement_list statement      { }
        ;

statement
        : identifier ':' value ';'              { }
        | identifier '(' parameter_list ')'     { }
        | identifier '(' parameter_list ')' ';' { }
        | identifier '(' ')' '{' statement_list '}'                 { }
        | identifier '(' parameter_list ')' '{' statement_list '}'  { }
        | DEFINE '(' identifier ',' identifier ',' identifier ')' ';' { }
        ;

parameter_list
        : parameter                     { }
        | parameter_list ',' parameter  { }
        ;

parameter
        : value                         { $$ = $1; }
        ;

value
        : identifier                    { $$ = $1; }
        | INTEGER                       { $$ = new LNInteger(atoi($1->c_str())); delete $1; }
        | FLOAT                         { $$ = new LNDouble(atof($1->c_str())); delete $1; }
        ;

identifier
        : IDENTIFIER                    { $$ = new LNString(*$1); delete $1; }
        | STRING                        { $$ = new LNString(trim_quote(*$1)); delete $1; }
        | VERSION                       { $$ = new LNString(*$1); delete $1; }
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
/* $Id$ -*- mode: c++ -*- */
/** \file scanner.ll Define the Verilog Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "scanner.h"

/* import the parser's token type into a local typedef */
typedef Verilog::Parser::token token;
typedef Verilog::Parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "VerilogFlexLexer" */
%option prefix="Verilog"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
#define YY_SAVE_TOKEN   yylval->sval = new std::string(yytext, yyleng)
%}

%x IN_COMMENT
%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}
 
 /*** BEGIN EXAMPLE - Change the Verilog lexer rules below ***/

"module"                { return token::MODULE; }
"endmodule"             { return token::ENDMODULE; }
"task"                  { return token::TASK; }
"endtask"               { return token::ENDTASK; }
"function"              { return token::FUNCTION; }
"endfunction"           { return token::ENDFUNCTION; }
"assign"                { return token::ASSIGN; }
"specify"               { return token::SPECIFY; }
"endspecify"            { return token::ENDSPECIFY; }
"parameter"             { return token::PARAMETER; }
"integer"               { return token::INTEGER; }
"real"                  { return token::REAL; }
"always"                { return token::ALWAYS; }
"initial"               { return token::VINITIAL; }
"if"                    { return token::IF; }
"else"                  { return token::ELSE; }
"begin"                 { return token::VBEGIN; }
"end"                   { return token::VEND; }
"repeat"                { return token::REPEAT; }
"posedge"               { return token::POSEDGE; }
"negedge"               { return token::NEGEDGE; }
"input"                 { return token::INPUT; }
"output"                { return token::OUTPUT; }
"inout"                 { return token::INOUT; }
"scalared"              { return token::SCALARED; }
"vectored"              { return token::VECTORED; }
"reg"                   { return token::REG; }

"and"                   { YY_SAVE_TOKEN; return token::GATETYPE; }
"nand"                  { YY_SAVE_TOKEN; return token::GATETYPE; }
"or"                    { YY_SAVE_TOKEN; return token::GATETYPE; }
"nor"                   { YY_SAVE_TOKEN; return token::GATETYPE; }
"xor"                   { YY_SAVE_TOKEN; return token::GATETYPE; }
"xnor"                  { YY_SAVE_TOKEN; return token::GATETYPE; }
"buf"                   { YY_SAVE_TOKEN; return token::GATETYPE; }
"bufif0"                { YY_SAVE_TOKEN; return token::GATETYPE; }
"bufif1"                { YY_SAVE_TOKEN; return token::GATETYPE; }
"not"                   { YY_SAVE_TOKEN; return token::GATETYPE; }
"notif0"                { YY_SAVE_TOKEN; return token::GATETYPE; }
"notif1"                { YY_SAVE_TOKEN; return token::GATETYPE; }
"pulldown"              { YY_SAVE_TOKEN; return token::GATETYPE; }
"pullup"                { YY_SAVE_TOKEN; return token::GATETYPE; }
"nmos"                  { YY_SAVE_TOKEN; return token::GATETYPE; }
"rnmos"                 { YY_SAVE_TOKEN; return token::GATETYPE; }
"pmos"                  { YY_SAVE_TOKEN; return token::GATETYPE; }
"rpmos"                 { YY_SAVE_TOKEN; return token::GATETYPE; }
"cmos"                  { YY_SAVE_TOKEN; return token::GATETYPE; }
"rcmos"                 { YY_SAVE_TOKEN; return token::GATETYPE; }
"tran"                  { YY_SAVE_TOKEN; return token::GATETYPE; }
"rtran"                 { YY_SAVE_TOKEN; return token::GATETYPE; }
"tranif0"               { YY_SAVE_TOKEN; return token::GATETYPE; }
"rtranif0"              { YY_SAVE_TOKEN; return token::GATETYPE; }
"tranif1"               { YY_SAVE_TOKEN; return token::GATETYPE; }
"rtranif1"              { YY_SAVE_TOKEN; return token::GATETYPE; }

"strong0"               { YY_SAVE_TOKEN; return token::STRENGTH0; }
"pull0"                 { YY_SAVE_TOKEN; return token::STRENGTH0; }
"weak0"                 { YY_SAVE_TOKEN; return token::STRENGTH0; }
"highz0"                { YY_SAVE_TOKEN; return token::STRENGTH0; }
"strong1"               { YY_SAVE_TOKEN; return token::STRENGTH1; }
"pull1"                 { YY_SAVE_TOKEN; return token::STRENGTH1; }
"weak1"                 { YY_SAVE_TOKEN; return token::STRENGTH1; }
"highz1"                { YY_SAVE_TOKEN; return token::STRENGTH1; }

"wire"                  { YY_SAVE_TOKEN; return token::NETTYPE; }
"tri"                   { YY_SAVE_TOKEN; return token::NETTYPE; }
"tri1"                  { YY_SAVE_TOKEN; return token::NETTYPE; }
"supply0"               { YY_SAVE_TOKEN; return token::NETTYPE; }
"wand"                  { YY_SAVE_TOKEN; return token::NETTYPE; }
"triand"                { YY_SAVE_TOKEN; return token::NETTYPE; }
"tri0"                  { YY_SAVE_TOKEN; return token::NETTYPE; }
"supply1"               { YY_SAVE_TOKEN; return token::NETTYPE; }
"wor"                   { YY_SAVE_TOKEN; return token::NETTYPE; }
"trior"                 { YY_SAVE_TOKEN; return token::NETTYPE; }
"trireg"                { YY_SAVE_TOKEN; return token::NETTYPE; }
'[bBoOdDhH]             { YY_SAVE_TOKEN; return token::BASE; }

[+-]*[0-9]+\.[0-9]+     { YY_SAVE_TOKEN; return token::FLOAT_NUMBER; }
[+-]*[0-9]+(\.[0-9]+)?[eE][+-]*[0-9]+ { YY_SAVE_TOKEN; return token::FLOAT_NUMBER; }
[0-9]+                  { YY_SAVE_TOKEN; return token::UNSIGNED_NUMBER; }
[a-zA-Z][a-zA-Z0-9_$]*  { YY_SAVE_TOKEN; return token::IDENTIFIER; }
"$"[a-zA-Z][a-zA-Z0-9_$]* { YY_SAVE_TOKEN; return token::SYSTEM_IDENTIFIER; }
\"('\"'|[^"])*\"        { YY_SAVE_TOKEN; return token::STRING; }
"<="                    { return token::NON_BLOCK_ASSIGN; }
"=>"                    { return token::SPECIFY_ASSIGN; }

 /* gobble up white-spaces */
[ \t\r]+ {
    yylloc->step();
}

 /* gobble up end-of-lines */
\n {
    yylloc->lines(yyleng); yylloc->step();
}

"//".*\n                yylloc->lines(yyleng); yylloc->step();
<INITIAL>{
"/*"                    BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
"*/"                    BEGIN(INITIAL);
[^*\n]+                 // eat comment in chunks
"*"                     // eat the long star
\n                      yylloc->lines(yyleng); yylloc->step();
}

 /* pass all other characters up to bison */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the Verilog lexer rules above ***/

%% /*** Additional Code ***/

namespace Verilog {

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
    : VerilogFlexLexer(in, out)
{
}

Scanner::~Scanner()
{
}

void Scanner::set_debug(bool b)
{
    yy_flex_debug = b;
}

}

/* This implementation of VerilogFlexLexer::yylex() is required to fill the
 * vtable of the class VerilogFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int VerilogFlexLexer::yylex()
{
    std::cerr << "in VerilogFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int VerilogFlexLexer::yywrap()
{
    return 1;
}

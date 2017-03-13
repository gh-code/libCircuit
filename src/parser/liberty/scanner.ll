/* $Id$ -*- mode: c++ -*- */
/** \file scanner.ll Define the Liberty Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "scanner.h"

/* import the parser's token type into a local typedef */
typedef Liberty::Parser::token token;
typedef Liberty::Parser::token_type token_type;

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

/* change the name of the scanner class. results in "LibertyFlexLexer" */
%option prefix="Liberty"

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

O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP  (u|U|L)
SP  (u8|u|U|L)
ES  (\\(['"\?\\\nabfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\n\f]

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}
 
 /*** BEGIN EXAMPLE - Change the Liberty lexer rules below ***/

"library"                           { return token::LIBRARY; }
"define"                            { return token::DEFINE; }

{L}({L}|{D})*                       { YY_SAVE_TOKEN; return token::IDENTIFIER; }
({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+   { YY_SAVE_TOKEN; return token::STRING; }

{HP}{H}+{IS}?                       { YY_SAVE_TOKEN; return token::INTEGER; }
{NZ}{D}*{IS}?                       { YY_SAVE_TOKEN; return token::INTEGER; }
"0"{O}*{IS}?                        { YY_SAVE_TOKEN; return token::INTEGER; }
{CP}?"'"([^'\\\n]|{ES})+"'"         { YY_SAVE_TOKEN; return token::INTEGER; }

{D}+{E}{FS}?                        { YY_SAVE_TOKEN; return token::FLOAT; }
{D}*"."{D}+{E}?{FS}?                { YY_SAVE_TOKEN; return token::FLOAT; }
{D}+"."{E}?{FS}?                    { YY_SAVE_TOKEN; return token::FLOAT; }
{HP}{H}+{P}{FS}?                    { YY_SAVE_TOKEN; return token::FLOAT; }
{HP}{H}*"."{H}+{P}{FS}?             { YY_SAVE_TOKEN; return token::FLOAT; }
{HP}{H}+"."{P}{FS}?                 { YY_SAVE_TOKEN; return token::FLOAT; }

{D}+("."{D}+)+                      { YY_SAVE_TOKEN; return token::VERSION; }
({L}|{D})+                          { YY_SAVE_TOKEN; return token::IDENTIFIER; }

 /* gobble up white-spaces */
[ \t\r\\]+ {
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

 /*** END EXAMPLE - Change the Liberty lexer rules above ***/

%% /*** Additional Code ***/

namespace Liberty {

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
    : LibertyFlexLexer(in, out)
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

/* This implementation of LibertyFlexLexer::yylex() is required to fill the
 * vtable of the class LibertyFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int LibertyFlexLexer::yylex()
{
    std::cerr << "in LibertyFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int LibertyFlexLexer::yywrap()
{
    return 1;
}

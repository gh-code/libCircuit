include( ../common.pri )
TEMPLATE = lib
TARGET = verilog
HEADERS = driver.h parser.h scanner.h expression.h \
    y.tab.h location.hh position.hh stack.hh
SOURCES = parser.cc scanner.cc driver.cc
CONFIG += debug
QMAKE_EXTRA_TARGETS += parser lexer distclean extraclean
PRE_TARGETDEPS += parser.o scanner.o

distclean.depends = extraclean
unix: extraclean.commands =  rm -f scanner.o parser.o scanner.cc parser.cc

parser = parser.yy
lexer = scanner.ll

lexer.target = scanner.cc
lexer.depends = scanner.ll
lexer.commands = flex -o scanner.cc scanner.ll

parser.target = parser.cc
parser.depends = parser.yy
parser.commands = bison -o parser.cc --defines=parser.h parser.yy

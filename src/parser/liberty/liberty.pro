include( ../common.pri )
TEMPLATE = lib
TARGET = liberty
HEADERS = driver.h parser.h scanner.h expression.h \
    y.tab.h location.hh position.hh stack.hh
SOURCES = parser.cc scanner.cc driver.cc
QMAKE_EXTRA_TARGETS += parser lexer distclean extraclean
PRE_TARGETDEPS += parser.o scanner.o

distclean.depends = extraclean
win32: extraclean.commands =  del /f scanner.o scanner.cc position.hh parser.h parser.o parser.cc
unix: extraclean.commands =  rm -f scanner.o scanner.cc position.hh parser.h parser.o parser.cc

parser = parser.yy
lexer = scanner.ll

lexer.target = scanner.cc
lexer.depends = scanner.ll
lexer.commands = flex -o scanner.cc scanner.ll

parser.target = parser.cc
parser.depends = parser.yy
parser.commands = bison -o parser.cc --defines=parser.h parser.yy

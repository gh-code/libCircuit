TEMPLATE = lib

CONFIG += staticlib
CONFIG += debug
CONFIG -= debug_and_release debug_and_release_target

HEADERS = driver.h parser.h scanner.h expression.h \
    y.tab.h location.hh position.hh stack.hh
SOURCES = parser.cc scanner.cc driver.cc
QMAKE_EXTRA_TARGETS += parser lexer clean distclean extraclean
PRE_TARGETDEPS += parser.cc scanner.cc

clean.depends = extraclean
distclean.depends = extraclean
win32: extraclean.commands =  del /f scanner.cc position.hh parser.h parser.cc
unix: extraclean.commands =  rm -f scanner.cc position.hh parser.h parser.cc

parser = parser.yy
lexer = scanner.ll

lexer.target = scanner.cc
lexer.depends = scanner.ll
lexer.commands = flex -o scanner.cc scanner.ll

parser.target = parser.cc
parser.depends = parser.yy
parser.commands = bison -o parser.cc --defines=parser.h parser.yy
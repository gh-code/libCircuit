TARGET = circuit
HEADERS = src/circuit/circuit.h
SOURCES = main.cpp
CONFIG += debug
LIBS += -L./lib -lcircuit -lverilog
INCLUDEPATH += ./src/circuit
PRE_TARGETDEPS += ./lib/libcircuit.a
QMAKE_EXTRA_TARGETS += circuit distclean extraclean

distclean.depends = extraclean
extraclean.commands =  cd src; make distclean

circuit.target = ./lib/libcircuit.a
circuit.depends = FORCE
circuit.commands = cd src; qmake && make

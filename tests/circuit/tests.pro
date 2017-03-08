QT += testlib
TEMPLATE = app
TARGET = tests
INCLUDEPATH += .
HEADERS += circuit.h
SOURCES += testcircuit.cpp ../../src/circuit/circuit.cpp
INCLUDEPATH += ../../src/circuit
LIBS += -L../../lib -lverilog
PRE_TARGETDEPS += ./lib/libcircuit.a
QMAKE_EXTRA_TARGETS += verilog

verilog.target = ./lib/libcircuit.a
verilog.depends = FORCE
verilog.commands = cd ../../src; qmake && make

QT += testlib
TEMPLATE = app
TARGET = tests
INCLUDEPATH += .
HEADERS += circuit.h
SOURCES += testcircuit.cpp ../../src/circuit/circuit.cpp
CONFIG += console
CONFIG -= debug_and_release debug_and_release_target
INCLUDEPATH += ../../src/circuit
LIBS += -L../../lib -lverilog
PRE_TARGETDEPS += ./lib/libcircuit.a
QMAKE_EXTRA_TARGETS += verilog

verilog.target = ./lib/libcircuit.a
verilog.depends = FORCE
win32: verilog.commands = cd ../../src & qmake && make
unix: verilog.commands = cd ../../src; qmake && make

TARGET = circuit
HEADERS = src/circuit/circuit.h
SOURCES = main.cpp
CONFIG += debug
CONFIG += console
CONFIG -= debug_and_release debug_and_release_target
LIBS += -L./lib -lcircuit -lverilog -lliberty
INCLUDEPATH += ./src/circuit
PRE_TARGETDEPS += ./lib/libcircuit.a
QMAKE_EXTRA_TARGETS += circuit clean distclean extraclean

clean.depends = extraclean
distclean.depends = extraclean
win32: extraclean.commands =  cd src & make clean
unix: extraclean.commands =  cd src; make clean

circuit.target = ./lib/libcircuit.a
circuit.depends = FORCE
win32: circuit.commands = cd src & qmake && make
unix: circuit.commands = cd src; qmake && make

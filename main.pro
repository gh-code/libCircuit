TARGET = circuit

SOURCES = main.cpp

CONFIG += console debug
CONFIG -= debug_and_release debug_and_release_target

LIBS += -L./lib -lCircuit
INCLUDEPATH += include

PRE_TARGETDEPS += ./lib/libCircuit.a
QMAKE_EXTRA_TARGETS += circuit clean distclean extraclean extradistclean

clean.depends = extraclean
extraclean.commands = cd src && make clean

distclean.depends = extradistclean
extradistclean.commands = cd src && make distclean

circuit.target = ./lib/libCircuit.a
circuit.depends = FORCE
circuit.commands = cd src && qmake && make
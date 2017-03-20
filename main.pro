TARGET = circuit
HEADERS = src/circuit/circuit.h src/EDAUtils/EDAUtils.h
SOURCES = main.cpp
CONFIG += debug
CONFIG += console
CONFIG -= debug_and_release debug_and_release_target
LIBS += -L./lib -lcircuit -lverilog -lliberty -lEDAUtils
INCLUDEPATH += include
PRE_TARGETDEPS += ./lib/libcircuit.a
QMAKE_EXTRA_TARGETS += circuit clean distclean extraclean extradistclean

clean.depends = extraclean
win32: extraclean.commands = cd src & make clean
unix: extraclean.commands = cd src; make clean

distclean.depends = extradistclean
win32: extradistclean.commands = del /q /f include\\*.h & rmdir include & cd src & make distclean
unix: extradistclean.commands = rm -f include/*.h & rmdir include; cd src; make distclean

circuit.target = ./lib/libcircuit.a
circuit.depends = FORCE
win32: circuit.commands = cd src & qmake && make
unix: circuit.commands = cd src; qmake && make
